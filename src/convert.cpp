#include "../headers/convert.hpp"


std::vector<std::vector<std::string>> Convert::readCSV(const std::string& filename){
    std::vector<std::vector<std::string>>data;
    std::ifstream file(filename);

    if(!file.is_open()){
        std::cerr<<"Failed to open file:"<<filename<<std::endl;
        return data;
    }
    std::string line;
    while(std::getline(file,line)){
        std::vector<std::string>row;
        std::stringstream ss(line);
        std::string cell;

        while(std::getline(ss, cell,',')){
            row.push_back(cell);
        }
        data.push_back(row);
    }
    file.close();
    return data;
}

void Convert::convert_type(std::string filename){
    
    auto data=readCSV(filename);

    for (const auto& row: data){
        for (const auto& cell : row) {
            std::cout << cell << "\t";
        }
        std::cout << std::endl;
    }
    std::cout<<"Converting the data please wait..."<<std::endl;
}