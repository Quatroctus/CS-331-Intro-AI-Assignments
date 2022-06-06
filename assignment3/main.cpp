#include <iostream>
#include <fstream>
#include <cerrno>
#include <cstring>

#include "naive_bayes.h"

int main(int argc, char** args) {
    if (argc < 3) {
        std::cerr << "Usage: " << args[0] << " <training_set> <test_set>\n";
        return 1;
    }

    std::ifstream trainingSet{args[1]};
    if (!trainingSet) {
        std::cerr << "Couldn't open: " << args[1] << " " << strerror(errno) << "\n";
        return 2;
    }
    
    std::ifstream testingSet{args[2]};
    if (!testingSet) {
        std::cerr << "Couldn't open: " << args[2] << " " << strerror(errno) << "\n";
        return 2;
    }

    ConstructBayes(trainingSet);

    std::ofstream output{"preprocessing_test.txt"};
    DumpVocabulary(output);
    std::string line;
    size_t correct = 0, total = 0;
    while (!testingSet.eof()) {
        std::getline(testingSet, line);
        if (line.empty()) continue;
        bool positive = IsLinePositive(line);
        bool bayes = QueryBayes(line, output);
        if (bayes == positive) correct++;
        total++;
    }

    std::cout << "Accuracy: " << (double(correct) / double(total)) << std::endl;

    return 0;
}
