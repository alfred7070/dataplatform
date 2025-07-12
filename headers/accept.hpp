#ifndef ACCEPT_HPP
#define ACCEPT_HPP
#include <iostream>
#include <sstream>
#include <string>


class Accept{
    public:
    std::string accept_data(const std::string& body, const std::string& contentType, int statusCode = 200, const std::string& statusMessage = "OK");
};

#endif
extern Accept acceptor;