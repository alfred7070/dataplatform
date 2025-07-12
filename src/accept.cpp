#include "../headers/accept.hpp"

std::string Accept::accept_data(const std::string &body, const std::string &contentType, int statusCode, const std::string &statusMessage)
{
    std::ostringstream oss;
    oss << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
    oss << "Content-Type: " << contentType << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: close\r\n\r\n";
    oss << body;
    return oss.str();
}