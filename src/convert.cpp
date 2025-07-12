#include "../headers/convert.hpp"

void DataFrame::print() const {
    std::cout << "DataFrame:" << std::endl;
    for(const auto& col : columns) std::cout << col << "\t";
    std::cout << "\n";

    for(size_t i = 0; i < columns.size(); i++) std::cout << "--------\t";
    std::cout << "\n";

    for(const auto& row : data) {
        for(const auto& cell : row) std::cout << cell << "\t";
        std::cout << "\n";
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
    for(const auto& row : data) {
        if(col_index < row.size()) column_data.push_back(row[col_index]);
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



void EncodedDataFrame::print() const {
    std::cout << "Encoded DataFrame:\n";
    for(const auto& col : columns) std::cout << col << "\t";
    std::cout << "\n";

    for(size_t i = 0; i < columns.size(); i++) std::cout << "--------\t";
    std::cout << "\n";

    for(const auto& row : data) {
        for(const auto& cell : row) std::cout << cell << "\t";
        std::cout << "\n";
    }
}

void EncodedDataFrame::print_mapping(const std::unordered_map<std::string, int>& mapping) const {
    std::cout << "\nLabel Encoding Mapping:\n";
    std::cout << "Value -> Encoded\n";
    std::cout << "----------------\n";

    std::vector<std::pair<std::string, int>> sorted_mapping(mapping.begin(), mapping.end());
    std::sort(sorted_mapping.begin(), sorted_mapping.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    for(const auto& pair : sorted_mapping) {
        std::cout << "'" << pair.first << "' -> " << pair.second << "\n";
    }
}


std::vector<std::vector<std::string>> Convert::readCSV(const std::string& filename) {
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filename);
    
    if(!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return data;
    }

    std::string line;
    while(std::getline(file, line)) {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;

        while(std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        data.push_back(row);
    }

    file.close();
    return data;
}

DataFrame Convert::create_dataframe(const std::vector<std::vector<std::string>>& raw_data) {
    DataFrame df;
    if(raw_data.empty()) {
        std::cerr << "No data to create dataframe!" << std::endl;
        return df;
    }

    df.columns = raw_data[0]; // headers
    for(size_t i = 1; i < raw_data.size(); i++) {
        df.data.push_back(raw_data[i]);
    }

    return df;
}

EncodedDataFrame Convert::label_encode_dataframe(const DataFrame& df) {
    EncodedDataFrame encoded_df;
    label_mapping.clear();
    int label_index = 0;

    encoded_df.columns = df.columns;

    std::vector<std::string> all_values = df.get_all_values();
    for(const auto& value : all_values) {
        if(label_mapping.find(value) == label_mapping.end()) {
            label_mapping[value] = label_index++;
        }
    }

    for(const auto& row : df.data) {
        std::vector<int> encoded_row;
        for(const auto& cell : row) {
            encoded_row.push_back(label_mapping[cell]);
        }
        encoded_df.data.push_back(encoded_row);
    }

    return encoded_df;
}

std::vector<std::vector<int>> Convert::one_hot_encode(const std::vector<std::string>& data) {
    std::unordered_map<std::string, int> uniqueValues;
    int index = 0;
    std::vector<std::vector<int>> encodedData;

    for(const auto& value : data) {
        if(uniqueValues.find(value) == uniqueValues.end()) {
            uniqueValues[value] = index++;
        }
    }

    std::cout << "\nUnique values found: " << uniqueValues.size() << "\n";
    for(const auto& pair : uniqueValues) {
        std::cout << "'" << pair.first << "' -> " << pair.second << "\n";
    }

    for(const auto& value : data) {
        std::vector<int> convertData(uniqueValues.size(), 0);
        convertData[uniqueValues[value]] = 1;
        encodedData.push_back(convertData);
    }

    return encodedData;
}

void Convert::print_encoded_data(const std::vector<std::vector<int>>& encodedData) {
    std::cout << "\nOne-Hot Encoded Data:\n";
    for(size_t i = 0; i < encodedData.size(); i++) {
        std::cout << "Value " << i << ": ";
        for(const auto& value : encodedData[i]) {
            std::cout << value << " ";
        }
        std::cout << "\n";
    }
}

void Convert::read_csv(std::string filename) {
    auto raw_data = readCSV(filename);
    if(raw_data.empty()) {
        std::cout << "No data found in file." << std::endl;
        return;
    }

    DataFrame df = create_dataframe(raw_data);
    df.print();

    std::cout << "\nOriginal DataFrame Info:\n";
    std::cout << "Columns: " << df.columns.size() << "\n";
    std::cout << "Rows: " << df.data.size() << "\n";

    std::cout << "\nLabel encoding the data please wait...\n";
    EncodedDataFrame encoded_df = label_encode_dataframe(df);

    encoded_df.print_mapping(label_mapping);
    std::cout << "\n";
    encoded_df.print();

    std::cout << "\nEncoded DataFrame Info:\n";
    std::cout << "Columns: " << encoded_df.columns.size() << "\n";
    std::cout << "Rows: " << encoded_df.data.size() << "\n";
    std::cout << "Unique values encoded: " << label_mapping.size() << "\n";
}

