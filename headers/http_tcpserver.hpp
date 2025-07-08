#ifndef INCLUDED_HTTP_TCPSERVER_HPP
#define INCLUDED_HTTP_TCPSERVER_HPP
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>

namespace http
{
    class TcpServer
    {
    public:
        TcpServer(std::string ip_address, int port);
        ~TcpServer();
      
    private:
        WSADATA m_wsaData;
        std::string m_ip_address;
        int m_socket;
        int m_new_socket;
        int m_port;
        long m_incomingMessage;
        struct sockaddr_in m_socketAddress;
        int m_socketAddress_len;
        std::string m_serverMessage;
        
        int startServer();
        void closeServer();
        void startListen();
        void acceptConnection(SOCKET &new_socket);
        
    };
}

#endif