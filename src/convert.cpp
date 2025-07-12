#include "../headers/convert.hpp"

void EncodedDataFrame::print() const {

    std::cout << "Encoded DataFrame:" << std::endl;
    for(const auto& col : columns) {
        std::cout << col << "\t";
    }
    std::cout << std::endl;

    for(size_t i = 0; i < columns.size(); i++) {
        std::cout << "--------\t";
    }
    std::cout << std::endl;

    for(const auto& row : data) {
        for(const auto& cell : row) {
            std::cout << cell << "\t";
        }
        std::cout << std::endl;
    }
}

void DataFrame::print() const {

    std::cout << "DataFrame:" << std::endl;
    for(const auto& col : columns) {
        std::cout << col << "\t";
    }
    std::cout << std::endl;
  
    for(size_t i = 0; i < columns.size(); i++) {
        std::cout << "--------\t";
    }
    std::cout << std::endl;

    for(const auto& row : data) {
        for(const auto& cell : row) {
            std::cout << cell << "\t";
        }
        std::cout << std::endl;
    }
}

std::vector<std::string> DataFrame::get_column(const std::string& col_name) const {
    std::vector<std::string> column_data;
 
    auto it = std::find(columns.begin(), columns.end(), col_name);
    if(it == columns.end()) {
        std::cerr << "Column '" << col_name << "' not found!" << std::endl;
        return column_data;
    }
    
    size_t col_index = std::distance(columns.begin(), it);
    
    // Extract column data
    for(const auto& row : data) {
        if(col_index < row.size()) {
            column_data.push_back(row[col_index]);
        }
    }
    
    return column_data;
}

std::vector<std::string> DataFrame::get_all_values() const {
    std::vector<std::string> all_values;
    
    for(const auto& row : data) {
        for(const auto& cell : row) {
            all_values.push_back(cell);
        }
    }
    
    return all_values;
}

DataFrame Convert::create_dataframe(const std::vector<std::vector<std::string>>& raw_data) {
    DataFrame df;
    
    if(raw_data.empty()) {
        std::cerr << "No data to create dataframe!" << std::endl;
        return df;
    }

    df.columns = raw_data[0];
  
    for(size_t i = 1; i < raw_data.size(); i++) {
        df.data.push_back(raw_data[i]);
    }
    
    return df;
}

EncodedDataFrame Convert::convert_data_to_dataframe(const std::vector<std::string>& data) {
    EncodedDataFrame encoded_df;
    std::unordered_map<std::string, int> uniqueValues;
    int index = 0;
 
    for(const auto& value : data){
        if(uniqueValues.find(value) == uniqueValues.end()){
            uniqueValues[value] = index++;
        }
    }
    
    std::vector<std::string> value_names(uniqueValues.size());
    for(const auto& pair : uniqueValues){
        value_names[pair.second] = pair.first;
    }
    encoded_df.columns = value_names;

    std::cout << "\nUnique values found: " << uniqueValues.size() << std::endl;
    for(const auto& pair : uniqueValues){
        std::cout << "'" << pair.first << "' -> Column " << pair.second << std::endl;
    }

    for(const auto& value : data){
        std::vector<int> convertData(uniqueValues.size(), 0);
        convertData[uniqueValues[value]] = 1;
        encoded_df.data.push_back(convertData);
    }
    
    return encoded_df;
}

void Convert::convert_data(const std::vector<std::string>& data, std::vector<std::vector<int>>& encodedData){
    std::unordered_map<std::string, int> uniqueValues;
    int index = 0;

    for(const auto& value : data){
        if(uniqueValues.find(value) == uniqueValues.end()){
            uniqueValues[value] = index++;
        }
    }
    
    std::cout << "\nUnique values found: " << uniqueValues.size() << std::endl;
    for(const auto& pair : uniqueValues){
        std::cout << "'" << pair.first << "' -> " << pair.second << std::endl;
    }

    for(const auto& value : data){
        std::vector<int> convertData(uniqueValues.size(), 0);
        convertData[uniqueValues[value]] = 1;
        encodedData.push_back(convertData);
    }
}

std::vector<std::vector<std::string>> Convert::readCSV(const std::string& filename){
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filename);
    
    if(!file.is_open()){
        std::cerr << "Failed to open file: " << filename << std::endl;
        return data;
    }
    
    std::string line;
    while(std::getline(file, line)){
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;
        
        while(std::getline(ss, cell, ',')){
            row.push_back(cell);
        }
        data.push_back(row);
    }
    file.close();
    return data;
}

void Convert::print_encoded_data(const std::vector<std::vector<int>>& encodedData){
    std::cout << "\nOne-Hot Encoded Data:" << std::endl;
    for(size_t i = 0; i < encodedData.size(); i++){
        std::cout << "Value " << i << ": ";
        for(const auto& value : encodedData[i]){
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
}

void Convert::read_csv(std::string filename){

    auto raw_data = readCSV(filename);
    
    if(raw_data.empty()){
        std::cout << "No data found in file." << std::endl;
        return;
    }
  
    DataFrame df = create_dataframe(raw_data);

    df.print();
    
    std::cout << "\nDataFrame Info:" << std::endl;
    std::cout << "Columns: " << df.columns.size() << std::endl;
    std::cout << "Rows: " << df.data.size() << std::endl;
 
    std::vector<std::string> all_values = df.get_all_values();
    
    std::cout << "\nConverting the data please wait..." << std::endl;
    std::cout << "Total data points (excluding headers): " << all_values.size() << std::endl;
    
    EncodedDataFrame encoded_df = convert_data_to_dataframe(all_values);
    
    std::cout << "\n";
    encoded_df.print();
    
    std::cout << "\nEncoded DataFrame Info:" << std::endl;
    std::cout << "Columns: " << encoded_df.columns.size() << std::endl;
    std::cout << "Rows: " << encoded_df.data.size() << std::endl;
   
    std::cout << "\n--- Individual Column Encodings ---" << std::endl;
    for(const auto& col_name : df.columns) {
        std::cout << "\nColumn: " << col_name << std::endl;
        auto col_data = df.get_column(col_name);
        
        EncodedDataFrame col_encoded_df = convert_data_to_dataframe(col_data);
        col_encoded_df.print();
    }
}