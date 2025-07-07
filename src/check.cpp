#include "../headers/check.hpp"
#include "../headers/retrieve.hpp"

Retrieve retrieve;
void Check::check_data(){
    retrieve.retrieve_data();
    std::cout<<"Checking data ... "<<std::endl;
}