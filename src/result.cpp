#include "../headers/result.hpp"
#include "../headers/clean.hpp"

Clean clean;
void Result::result_data(){
    clean.clean_data();
    std::cout<<"here is your result data"<<std::endl;
}