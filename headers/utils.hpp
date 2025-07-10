#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>

std::vector<std::string> split(const std::string& str, char delimiter);

std::string readFile(const std::string& filePath);

std::string toLower(const std::string& str);

#endif