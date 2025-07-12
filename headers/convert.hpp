#ifndef CONVERT_HPP
#define CONVERT_HPP
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class Convert{
    public:
    std::vector<std::vector<std::string>> readCSV(const std::string& filename);
    void convert_type(std::string filename);
};

#endif