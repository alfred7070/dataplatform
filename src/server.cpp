#include "../headers/server.hpp"
#include <sstream>

Server::Server(ServerConfig config) : m_config(config) {

    if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0) {
        Error("WSA initialization failed.");
    }

    m_ServerFD = socket(config.socAF, config.socType, 0);
    if (m_ServerFD == INVALID_SOCKET) {
        close("Socket creation failed.");
    }

    m_Addrlen = sizeof(m_Address);
    m_Address.sin_family = AF_INET;
    m_Address.sin_addr.S_un.S_addr = INADDR_ANY;
    m_Address.sin_port = htons(config.PORT);

    if (bind(m_ServerFD, (struct sockaddr*)&m_Address, m_Addrlen) == SOCKET_ERROR) {
        close("Bind failed.");
    };

    if (listen(m_ServerFD, 3) == SOCKET_ERROR) {
        close("Listen failed.");
    }

    Info("Server is listening on port " + std::to_string(config.PORT));
}

void Server::setstaticDirectory(std::string path) {
    m_StaticDIR = path;
}

void Server::_listen() {

    while (true) {

        SOCKET client_fd = accept(m_ServerFD, (struct sockaddr*)&m_Address, &m_Addrlen);
        if (client_fd == INVALID_SOCKET) {
            close("Accept failed.");
            return;
        }

        char buffer[4096] = { 0 };
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received < 0) {
            Error("Receive failed.");
        }
        else {

            std::vector<std::string> lines = split(buffer, '\n');
            if (lines.size() > 0) {

                std::vector<std::string> request_line = split(lines[0], ' ');
                if (request_line.size() == 3) {
                    std::string method = request_line[0];
                    std::string full_path = request_line[1];
                    std::string http_version = request_line[2];
                    std::string path = full_path;
                    std::string response = "";

                    std::unordered_map<std::string, std::string> queryParams;
                    size_t query_start = full_path.find("?");
                    if (query_start != std::string::npos) {
                        path = full_path.substr(0, query_start);
                        std::string query_string = full_path.substr(query_start + 1);
                        queryParams = parseQueryParams(query_string);
                    }

                    Info("Method: " + method);
                    Info("Path: " + path);
                    Info("HTTP Version: " + http_version);

                    try
                    {
                        if (method == "GET") {
                            response = handleGetRequest(path, queryParams);
                        }
                        else if (method == "POST") {

                            int content_length = 0;
                            for (const auto& line : lines) {
                                std::string lower_line = toLower(line);
                                if (lower_line.find("content-length:") != std::string::npos) {
                                    std::vector<std::string> parts = split(line, ' ');
                                    if (parts.size() == 2) {
                                        content_length = std::stoi(parts[1]);
                                    }
                                }
                            }

                            if (content_length > 0) {
                                size_t header_end = std::string(buffer).find("\r\n\r\n");
                                if (header_end != std::string::npos) {
                                    std::string body(buffer + header_end + 4, content_length);
                                    response = handlePostRequest(path, body);
                                }
                                else {
                                    Error("Failed to find end of headers.");
                                    response = createHttpResponse("{\"error\":\"400 Bad Request - no header end found\"}", "application/json", 400, "Bad Request");
                                }
                            }
                            else {
                                Error("Content-Length is 0 or not found.");
                                response = createHttpResponse("{\"error\":\"400 Bad Request - Content-Length missing or zero\"}", "application/json", 400, "Bad Request");
                            }

                        }
                        else {
                            response = createHttpResponse("{\"error\":\"405 Method Not Allowed\"}", "application/json", 405, "Method Not Allowed");
                        }
                    }
                    catch (const std::exception& e)
                    {
                        response = createHttpResponse("{\"error\":\"Exception: " + std::string(e.what()) + "\"}", "application/json", 500, "Internal Server Error");
                    }

                    if (response != "") {
                        int bytes_sent = send(client_fd, response.c_str(), (int)response.size(), 0);
                        if (bytes_sent < 0) {
                            Error("Send failed.");
                        }
                    }
                }

            }
        }

        closesocket(client_fd);
    }
    close();
}

void Server::close(std::string reason) {
    if (reason != "") {
        Error(reason.c_str());
    }
    if (m_ServerFD != INVALID_SOCKET) {
        closesocket(m_ServerFD);
    }
    WSACleanup();
}

std::string Server::getContentType(const std::string& path) {
    if (path.find(".html") != std::string::npos) {
        return "text/html";
    }
    else if (path.find(".css") != std::string::npos) {
        return "text/css";
    }
    else if (path.find(".js") != std::string::npos) {
        return "application/javascript";
    }
    else if (path.find(".jpg") != std::string::npos) {
        return "image/jpeg";
    }
    else if (path.find(".png") != std::string::npos) {
        return "image/png";
    }
    else {
        return "text/plain";
    }
}

std::string Server::createHttpResponse(const std::string& body, const std::string& contentType, int statusCode, const std::string& statusMessage) {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
    oss << "Content-Type: " << contentType << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: close\r\n";
    oss << "\r\n";
    oss << body;
    return oss.str();
}

std::string Server::handleGetRequest(const std::string& path, const std::unordered_map<std::string, std::string>& queryParams) {

    if (path == "/api/data") {
        if (m_LastPostedData.empty()) {
            return createHttpResponse("{\"error\":\"No data posted yet\"}", "application/json", 404, "Not Found");
        }
        else {
            return createHttpResponse(m_LastPostedData, "application/json", 200, "OK");
        }
    }
    else if (path == "/") {
        std::string staticDir = "./public";
        std::string indexFileContent = readFile(staticDir + "/index.html");
        if (indexFileContent.empty()) {
            return createHttpResponse("{\"error\":\"Could not find index.html in /public\"}", "application/json", 404, "Not Found");
        }
        return createHttpResponse(indexFileContent, "text/html", 200, "OK");
    }
    else if (path == "/echo") {
        std::string body = "{ \"queryParameters\": {";
        bool first = true;
        for (const auto& param : queryParams) {
            if (!first) body += ",";
            body += "\"" + param.first + "\":\"" + param.second + "\"";
            first = false;
        }
        body += "} }";
        return createHttpResponse(body, "application/json", 200, "OK");
    }
    else if (path == "/welcome") {
        return createHttpResponse("{\"message\":\"Welcome to my server!\"}", "application/json", 200, "OK");
    }
    else {
        return createHttpResponse("{\"error\":\"404 Not Found\"}", "application/json", 404, "Not Found");
    }
}

std::string Server::handlePostRequest(const std::string& path, const std::string& body) {
  
    m_LastPostedData = body;

    std::string responseBody = "{ \"status\": \"received\", \"data\": " + body + " }";
    return createHttpResponse(responseBody, "application/json", 200, "OK");
}

std::unordered_map<std::string, std::string> Server::parseQueryParams(const std::string& query) {
    std::unordered_map<std::string, std::string> params;
    std::vector<std::string> pairs = split(query, '&');
    for (const std::string& pair : pairs) {
        std::vector<std::string> keyValue = split(pair, '=');
        if (keyValue.size() == 2) {
            params[keyValue[0]] = keyValue[1];
        }
    }
    return params;
}
