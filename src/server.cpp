#include "../headers/server.hpp"
#include "../headers/check.hpp"
#include "../headers/accept.hpp"
Check check;
Accept acceptor;
Server::Server(ServerConfig config) : m_config(config)
{
    if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
    {
        Error("WSA initialization failed.");
    }

    m_ServerFD = socket(config.socAF, config.socType, 0);
    if (m_ServerFD == INVALID_SOCKET)
    {
        close("Socket creation failed.");
    }

    m_Addrlen = sizeof(m_Address);
    m_Address.sin_family = AF_INET;
    m_Address.sin_addr.S_un.S_addr = INADDR_ANY;
    m_Address.sin_port = htons(config.PORT);

    if (bind(m_ServerFD, (struct sockaddr *)&m_Address, m_Addrlen) == SOCKET_ERROR)
    {
        close("Bind failed.");
    }

    if (listen(m_ServerFD, 3) == SOCKET_ERROR)
    {
        close("Listen failed.");
    }

    Info("Server is listening on port " + std::to_string(config.PORT));

    if (!std::filesystem::exists("./uploads"))
    {
        std::filesystem::create_directory("./uploads");
    }
}

void Server::setstaticDirectory(std::string path)
{
    m_StaticDIR = path;
}

std::vector<char> readFullRequest(SOCKET client_fd, int total_expected, const char *initial_buffer, int initial_len)
{
    std::vector<char> request_data(initial_buffer, initial_buffer + initial_len);
    while ((int)request_data.size() < total_expected)
    {
        char temp_buffer[4096];
        int bytes = recv(client_fd, temp_buffer, sizeof(temp_buffer), 0);
        if (bytes <= 0)
            break;
        request_data.insert(request_data.end(), temp_buffer, temp_buffer + bytes);
    }
    return request_data;
}

void Server::_listen()
{
    while (true)
    {
        SOCKET client_fd = accept(m_ServerFD, (struct sockaddr *)&m_Address, &m_Addrlen);
        if (client_fd == INVALID_SOCKET)
        {
            close("Accept failed.");
            return;
        }

        char buffer[4096];
        int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0)
        {
            Error("Receive failed or connection closed.");
            closesocket(client_fd);
            continue;
        }

        std::string request_str(buffer, bytes_received);
        size_t header_end = request_str.find("\r\n\r\n");
        if (header_end == std::string::npos)
        {
            std::string resp = acceptor.accept_data("{\"error\":\"400 Bad Request - no header end found\"}", "application/json", 400, "Bad Request");
            send(client_fd, resp.c_str(), (int)resp.size(), 0);
            closesocket(client_fd);
            continue;
        }

        std::string headers_str = request_str.substr(0, header_end);
        std::vector<std::string> lines = split(headers_str, '\n');
        std::vector<std::string> request_line = split(lines[0], ' ');

        if (request_line.size() != 3)
        {
            closesocket(client_fd);
            continue;
        }

        std::string method = request_line[0];
        std::string full_path = request_line[1];
        std::string http_version = request_line[2];
        std::string path = full_path;

        std::unordered_map<std::string, std::string> queryParams;
        size_t qmark = full_path.find('?');
        if (qmark != std::string::npos)
        {
            path = full_path.substr(0, qmark);
            queryParams = parseQueryParams(full_path.substr(qmark + 1));
        }

        int content_length = 0;
        for (const auto &line : lines)
        {
            std::string lower_line = toLower(line);
            if (lower_line.find("content-length:") != std::string::npos)
            {
                size_t colon = line.find(':');
                if (colon != std::string::npos)
                {
                    content_length = std::stoi(line.substr(colon + 1));
                }
            }
        }

        std::string response;

        try
        {
            if (method == "GET")
            {
                response = handleGetRequest(path, queryParams);
            }
            else if (method == "POST")
            {
                int total_size = (int)header_end + 4 + content_length;
                auto full_request = readFullRequest(client_fd, total_size, buffer, bytes_received);

                if ((int)full_request.size() < total_size)
                {
                    response = acceptor.accept_data("{\"error\":\"Incomplete POST body received\"}", "application/json", 400, "Bad Request");
                }
                else
                {
                    std::vector<char> body(full_request.begin() + header_end + 4, full_request.begin() + header_end + 4 + content_length);
                    response = check.check_data(path, body);
                }
            }
            else
            {
                response = acceptor.accept_data("{\"error\":\"405 Method Not Allowed\"}", "application/json", 405, "Method Not Allowed");
            }
        }
        catch (const std::exception &e)
        {
            response = acceptor.accept_data("{\"error\":\"" + std::string(e.what()) + "\"}", "application/json", 500, "Internal Server Error");
        }

        send(client_fd, response.c_str(), (int)response.size(), 0);
        closesocket(client_fd);
    }

    close();
}

void Server::close(std::string reason)
{
    if (!reason.empty())
        Error(reason.c_str());
    if (m_ServerFD != INVALID_SOCKET)
    {
        closesocket(m_ServerFD);
    }
    WSACleanup();
}

std::string Server::getContentType(const std::string &path)
{
    if (path.find(".html") != std::string::npos)
        return "text/html";
    if (path.find(".css") != std::string::npos)
        return "text/css";
    if (path.find(".js") != std::string::npos)
        return "application/javascript";
    if (path.find(".jpg") != std::string::npos)
        return "image/jpeg";
    if (path.find(".png") != std::string::npos)
        return "image/png";
    return "text/plain";
}

std::string Server::handleGetRequest(const std::string &path, const std::unordered_map<std::string, std::string> &queryParams)
{
    if (path == "/api/data")
    {
        if (m_LastPostedData.empty())
        {
            return acceptor.accept_data("{\"error\":\"No data posted yet\"}", "application/json", 404, "Not Found");
        }
        return acceptor.accept_data(m_LastPostedData, "application/json", 200, "OK");
    }
    else if (path == "/")
    {
        std::string content = readFile("./public/index.html");
        return content.empty() ? acceptor.accept_data("{\"error\":\"index.html not found\"}", "application/json", 404, "Not Found")
                               : acceptor.accept_data(content, "text/html", 200, "OK");
    }
    else if (path == "/echo")
    {
        std::string body = "{ \"queryParameters\": {";
        bool first = true;
        for (const auto &param : queryParams)
        {
            if (!first)
                body += ",";
            body += "\"" + param.first + "\":\"" + param.second + "\"";
            first = false;
        }
        body += "} }";
        return acceptor.accept_data(body, "application/json", 200, "OK");
    }
    else if (path == "/welcome")
    {
        return acceptor.accept_data("{\"message\":\"Welcome to my server!\"}", "application/json", 200, "OK");
    }
    return acceptor.accept_data("{\"error\":\"404 Not Found\"}", "application/json", 404, "Not Found");
}

std::unordered_map<std::string, std::string> Server::parseQueryParams(const std::string &query)
{
    std::unordered_map<std::string, std::string> params;
    for (const auto &pair : split(query, '&'))
    {
        auto kv = split(pair, '=');
        if (kv.size() == 2)
        {
            params[kv[0]] = kv[1];
        }
    }
    return params;
}
