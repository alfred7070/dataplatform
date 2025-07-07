#include "../headers/clean.hpp"
#include "../headers/check.hpp"

Check check;
void Clean::clean_data(){
    check.check_data();
    std::cout<<"Cleaning data in progress... your data will be available soon"<<std::endl;
}