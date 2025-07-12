#ifndef CONVERT_HPP
#define CONVERT_HPP
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>

struct DataFrame {
    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> data;
    
    void print() const;
    std::vector<std::string> get_column(const std::string& col_name) const;
    std::vector<std::string> get_all_values() const;
};

struct EncodedDataFrame {
    std::vector<std::string> columns;
    std::vector<std::vector<int>> data;
    
    void print() const;
};

class Convert{
    public:
    void read_csv(std::string filename);
    private:
    std::vector<std::vector<std::string>> readCSV(const std::string& filename);
    DataFrame create_dataframe(const std::vector<std::vector<std::string>>& raw_data);
    EncodedDataFrame convert_data_to_dataframe(const std::vector<std::string>& data);
    void convert_data(const std::vector<std::string>& data, std::vector<std::vector<int>>& encodedData);
    void print_encoded_data(const std::vector<std::vector<int>>& encodedData);
};

#endif