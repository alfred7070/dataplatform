#ifndef SERVER_HPP
#define SERVER_HPP

#include <WinSock2.h>
#include <unordered_map>
#include <string>
#include <vector>             // Required for binary body (std::vector<char>)
#include "utils.hpp"
#include "log.hpp"

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
    std::string createHttpResponse(const std::string& body, const std::string& contentType, int statusCode = 200, const std::string& statusMessage = "OK");
    std::string handleGetRequest(const std::string& path, const std::unordered_map<std::string, std::string>& queryParams);
    std::string handlePostRequest(const std::string& path, const std::string& body);
    std::string handleBinaryPostRequest(const std::string& path, const std::vector<char>& body);  // Added
    std::unordered_map<std::string, std::string> parseQueryParams(const std::string& query);
};

#endif
