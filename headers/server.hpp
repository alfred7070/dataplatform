#ifndef SERVER_HPP
#define SERVER_HPP

#include <WinSock2.h>
#include <unordered_map>
#include <string>
#include <vector>           
#include "utils.hpp"
#include "log.hpp"

#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>

#include <filesystem>
#include <fstream>

struct ServerConfig {
    enum AddressFamilies { IPv4 = AF_INET, IPv6 = AF_INET6 };
    enum SocketType { STREAM = SOCK_STREAM, DGRAM = SOCK_DGRAM, RAW = SOCK_RAW, SEQPACKET = SOCK_SEQPACKET };
    int PORT = 8080;
    int socAF = AddressFamilies::IPv4;
    int socType = SocketType::STREAM;
};

class Server {
public:
    Server(ServerConfig config);
    ~Server();

    void setstaticDirectory(std::string path);
    void close(std::string reason = "");
    void _listen();

private:
    int m_Addrlen;
    sockaddr_in m_Address;
    SOCKET m_ServerFD;
    WSADATA m_wsaData;
    std::string m_StaticDIR;
    ServerConfig m_config;
    std::string m_LastPostedData;
    std::string getContentType(const std::string& path);
    std::string handleGetRequest(const std::string& path, const std::unordered_map<std::string, std::string>& queryParams);  
    std::unordered_map<std::string, std::string> parseQueryParams(const std::string& query);
};

#endif
