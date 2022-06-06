#pragma once
#include <fstream>
#include <string>

bool IsLinePositive(const std::string& line);
void DumpVocabulary(std::ofstream& output);


void ConstructBayes(std::ifstream& trainingSet);
bool QueryBayes(const std::string& unsanitizedLine, std::ofstream& output);
