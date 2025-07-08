#include "../headers/http_tcpserver.hpp"
#include "../headers/accept.hpp"
using namespace http;

void Accept::accept_data(){
    TcpServer server=TcpServer("0.0.0.0", 8080);
    std::cout<<"waiting for data input ..." <<std::endl;
}