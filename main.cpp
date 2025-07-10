#include "./headers/result.hpp"
#include "./headers/server.hpp"
#include "./headers/log.hpp"

void parseCommandArgs(ServerConfig& config, int& argc, char* argv[]){
   
    if(argc > 1){
        if (std::string(argv[1]) == "-p"){
            config.PORT = std::atoi(argv[2]);
            if (config.PORT == 0){
                Warn("Not a valid port, defaulting back to 8080.");
                config.PORT = 8080;
            }
        }
    }
}

Result result;
ServerConfig config;
int main(int argc, char* argv[]){
    result.result_data();
    parseCommandArgs(config, argc, argv);

    Server* server = new Server(config);
    server->setstaticDirectory("./public"); 
    server->_listen();

}