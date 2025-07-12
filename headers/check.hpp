#ifndef CHECK_HPP
#define CHECK_HPP
#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>

class Check {
public:
    std::string check_data(const std::string &path, const std::vector<char> &body);
    
private:
    std::string detect_file_type(const std::vector<char> &body);
    bool detect_csv_content(const std::vector<char> &body);
    bool detect_json_content(const std::vector<char> &body);
    bool detect_xml_content(const std::vector<char> &body);
    bool detect_html_content(const std::vector<char> &body);
    bool detect_text_content(const std::vector<char> &body);
};

#endif