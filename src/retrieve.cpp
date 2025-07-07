#include "../headers/retrieve.hpp"
#include "../headers/convert.hpp"

Convert convert;
void Retrieve::retrieve_data(){
    convert.convert_type();
    std::cout<<"retrieving data both the header and the columns"<<std::endl;
}