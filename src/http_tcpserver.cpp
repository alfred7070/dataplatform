#include "../headers/http_tcpserver.hpp"


namespace http
{
    TcpServer::TcpServer(std::string ip_address, int port)
    :m_ip_address(ip_address), m_port(port), m_socket(),
    m_new_socket(), m_incomingMessage(), m_socketAddress(),
    m_socketAddress_len(sizeof(m_socketAddress)),
    m_serverMessage(buildResponse())
    {
        startServer();
    }
    TcpServer::~TcpServer()
    {
        closeServer();
    }
    int TcpServer::startServer()
    {
        if(WSAStartup(MAKEWORD(2, 0), &m_wsaData)!=0)
        {
            exitWithError("WSAStartup failed");
        }
        m_socket=socket(AF_INET, SOCK_STREAM, 0);
        if(m_socket<0)
        {
            exitWithError("Cannot created socket");
            return 1;
        }
        return 0;
    }

    void TcpServer::startListen()
    {
        if(listen(m_socket, 20)<0)
        {
            exitWithError("Socket listen failed");
        }
        std::ostringstream ss;
        ss<<"\n*** Listening on ADDRESS: "
        <<inet_ntoa(m_socketAddress.sin_addr)
        << "PORT: "<<ntohs(m_socketAddress.sin_port)
        <<"***\n\n";
        log(ss.str());
    }
    void TcpServer::acceptConnection(SOCKET &new_socket)
    {
        new_socket=accept(m_socket, (sockaddr*)&m_socketAddress, &m_socketAddress_len);
        if(new_socket<0)
        {
            std::ostringstream ss;
            ss<<"Server failed to accept incoming connection from ADDRESS: "
            <<inet_ntoa(m_socketAddress.sin_addr)<<"; PORT: "
            <<ntohs(m_socketAddress.sin_port);
            exitWithError(ss.str());
        }
    }

    void TcpServer::closeServer()
    {
        closesocket(m_socket);
        closesocket(m_new_socket);
        WSACleanup();
        exit(0);
    }
    void log(const std::string &message){
        std::cout<<message<<std::endl;
    }
    void exitWithError(const std::string& errorMessage) {
    std::cerr << "Error: " << WSAGetLastError() << std::endl;
    log("Error: " + errorMessage);
    exit(1);
}
}