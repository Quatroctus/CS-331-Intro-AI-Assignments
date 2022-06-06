#include "naive_bayes.h"

#include <algorithm>
#include <cstring>
#include <cmath>
#include <iostream>
#include <set>
#include <numeric>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <vector>

struct Record {
    long double negneg = 0, negpos = 0, posneg = 0, pospos = 0;
    Record(){}
    Record(long double negneg, long double negpos, long double posneg, long double pospos)
        : negneg(negneg), negpos(negpos), posneg(posneg), pospos(pospos) {}
};

long double Total = 0, TotalPositive = 0, TotalNegative;
std::unordered_map<std::string, Record> features;
std::set<std::string> vocabulary;
std::vector<std::pair<std::unordered_set<std::string>, bool>> sentences;

void ParseLine(const std::string& line, bool positive) {
    std::string word;
    std::stringstream lineStream{line};
    std::unordered_set<std::string> words;
    while (getline(lineStream, word, ' ')) {
        if (word.empty()) continue;
        words.insert(word);
        if (features.find(word) == features.end()) {
            vocabulary.insert(word);
            features.insert({word, Record{ Total - TotalPositive, TotalPositive, 0, 0 }});
        }
    }

    sentences.push_back({words, positive});

    for (auto& keyValue : features) {
        const std::string& feature = keyValue.first;
        Record& record = keyValue.second;
        if (words.find(feature) != words.end()) { // feature is in words
            if (positive) {
                record.pospos++;
            } else {
                record.posneg++;
            }
        } else { // feature is not in words
            if (positive) {
                record.negpos++;
            } else {
                record.negneg++;
            }
        }
    }
}

void ConvertToProbability() {
    const long double totalNegative = TotalNegative + 2;
    const long double totalPositive = TotalPositive + 2;
    for (auto& keyValue : features) {
        const std::string& feature = keyValue.first;
        Record& record = keyValue.second;
        record.negneg = std::log((record.negneg + 1) / (totalNegative));
        record.negpos = std::log((record.negpos + 1) / (totalPositive));
        record.posneg = std::log((record.posneg + 1) / (totalNegative));
        record.pospos = std::log((record.pospos + 1) / (totalPositive));
    }
    TotalPositive = std::log((TotalPositive + 1) / (Total + 2));
    TotalNegative = std::log((TotalNegative + 1) / (Total + 2));
}

void Sanitize(std::string& line) {
    line.erase(std::remove_if(line.begin(), line.end(), [](char c) { return !std::isalnum(c) && c != ' '; }), line.end());
    std::transform(line.begin(), line.end(), line.begin(), [](char c) { return std::tolower(c); });
}

bool IsLinePositive(const std::string& line) {
    return std::stoi(line.substr(line.find_last_of('\t'))) == 1;
}

void DumpVocabulary(std::ofstream& output) {
    for (const auto& word : vocabulary) {
        output << word << ",";
    }
    output << "classlabel\n";
}

void ConstructBayes(std::ifstream& trainingSet) {
    std::string line;
    while (!trainingSet.eof()) {
        std::getline(trainingSet, line);
        if (line.empty()) continue;
        size_t tabLocation = line.find_last_of('\t');
        bool positive = std::stoi(line.substr(tabLocation)) == 1;
        line.erase(tabLocation-1);
        Sanitize(line);
        ParseLine(line, positive);
        Total++;
        TotalPositive += positive;
    }
    TotalNegative = Total - TotalPositive;

    // Dump
    std::ofstream output{"preprocessed_train.txt"};
    DumpVocabulary(output);
    for (const auto& sentence : sentences) {
        for (const auto& word : vocabulary) {
            if (sentence.first.find(word) != sentence.first.end()) 
                output << "1,";
            else
                output << "0,";
        }
        output << (sentence.second ? '1' : '0') << '\n';
    }


    #ifdef DEBUG
    std::cout << "TotalNegative: " <<   TotalNegative   << std::endl;
    std::cout << "TotalPositive: " <<   TotalPositive   << std::endl;
    std::cout << "Total: " <<           Total           << std::endl;
    for (auto& keyValue : features) {
        std::cout << keyValue.first << std::endl;
        std::cout << "\t\t" << keyValue.second.negneg;
        std::cout << "\t\t" << keyValue.second.negpos;
        std::cout << "\t\t" << keyValue.second.posneg;
        std::cout << "\t\t" << keyValue.second.pospos << std::endl;
    }
    #endif
    ConvertToProbability();

    #ifdef DEBUG
    std::cout << "\n\n\n";

    std::cout << "TotalNegative: " <<   TotalNegative   << std::endl;
    std::cout << "TotalPositive: " <<   TotalPositive   << std::endl;
    std::cout << "Total: " <<           Total           << std::endl;
    for (auto& keyValue : features) {
        std::cout << keyValue.first << std::endl;
        std::cout << "\t\t" << keyValue.second.negneg;
        std::cout << "\t\t" << keyValue.second.negpos;
        std::cout << "\t\t" << keyValue.second.posneg;
        std::cout << "\t\t" << keyValue.second.pospos << std::endl;
    }
    #endif
}

bool QueryBayes(const std::string& unsanitizedLine, std::ofstream& output) {
    size_t tabLocation = unsanitizedLine.find_last_of('\t');
    std::string line = unsanitizedLine.substr(0, tabLocation-1);
    Sanitize(line);

    std::string word;
    std::stringstream lineStream{line};
    std::unordered_set<std::string> words;
    while (getline(lineStream, word, ' ')) {
        if (word.empty()) continue;
        words.insert(word);
    }

    long double positive = TotalPositive;
    long double negative = TotalNegative;
    for (auto& keyValue : features) {
        const std::string& feature = keyValue.first;
        const Record& record = keyValue.second;
        if (words.find(feature) != words.end()) {
            positive += record.pospos;
            negative += record.posneg;
        } else {
            positive += record.negpos;
            negative += record.negneg;
        }
    }

    for (const auto& word : vocabulary) {
        if (words.find(word) != words.end()) 
            output << "1,";
        else
            output << "0,";
    }
    output << (positive > negative ? '1' : '0') << '\n';

    return positive > negative;
}