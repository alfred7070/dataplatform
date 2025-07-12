#ifndef CHECK_HPP
#define CHECK_HPP
#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>


class Check{
    public:
    std::string check_data(const std::string& path, const std::vector<char>& body);
};

#endif