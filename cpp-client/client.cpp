#include <iostream>

#include <string>
#include <map>
#include <sstream>
#include <chrono>
#include "resources/GameHandler.h"
#include <mutex>





int PORT ;
std::string HOST ;
std::string SERVER_IP;

extern Game game;

bool readConfig(){
	std::ifstream file("../config.json"); // Open the json file
    if (!file.is_open()) {
        std::cerr << "Failed to open config.json" << std::endl;
        return false;
    }

    json jsonData; // Create a json object

    try {
        file >> jsonData; // Parse the json data from the file
		PORT=jsonData["server_port"].get<int>();
		SERVER_IP=jsonData["server_ip"];
		HOST=jsonData["server_ip"];
    } catch (json::parse_error& e) {
        std::cerr << "json parse error: " << e.what() << std::endl;
		return false ;
    }
	

    return true;

}

int main() {
	if(!readConfig()){
		return 1;

	}
	
	curl_global_init(CURL_GLOBAL_DEFAULT);
	GameHandler handler(HOST,SERVER_IP,PORT);
	game.SetServersPort(PORT);
	game.SetServer(SERVER_IP);
	
	if(!handler.begin()){
		std::cerr<<"couldnt connect to server";
		return 1;
	}
	
	game.SetToken(handler.GetToken());
	game.SetClient();
	handler.ready();
	
	while (handler.GetGameOn());
	
	handler.join();
	curl_global_cleanup();
   
   
}
