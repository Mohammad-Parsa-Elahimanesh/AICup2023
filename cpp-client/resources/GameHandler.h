#ifndef GAMEHANDELER
#define GAMEHANDELER
#include <iostream>
#include <string>
#include <thread>
#include <shared_mutex>
#include <mutex>
#include <string>
#include <functional>
#include <vector>
#include <utility>
#include <fstream>
#include <sstream>
#include <random>
#include <functional>
#include <map>
#include "../externals/httplib.h"
#include "MainProgram.h"


using namespace nlohmann;

using namespace nlohmann;



void handleYourTurn(const httplib::Request& req, httplib::Response& res);
void handleKill(const httplib::Request& req, httplib::Response& res);
void handleEnd(const httplib::Request& req, httplib::Response& res);
void handleInit(const httplib::Request& req, httplib::Response& res);


//there should just be one GamHandeler
class GameHandler {
public:
	GameHandler(std::string host,std::string server_ip,int server_port);
	GameHandler();
	//initialization function
	bool begin();
	void ready();

	int GetPort();
	int GetServersPort();


	std::string GetToken();

	void join();
	bool GetGameOn();
	



	~GameHandler();



private:

	std::string server_ip;
	std::string host;
	std::string token;
	int id;
	int port;
	int server_port;
	std::thread* server_thread;
	httplib::Server* runServer(const char* host, const int PORT, std::vector<std::pair<std::string, std::function<void(const httplib::Request& req, httplib::Response& res)>>> urls);
	std::string generateToken();
	json request(std::string url, std::string api, int port = 12345);
	json SendloginRequest(std::string url, int port = 12345);





};



#endif // !GAMEHANDELER;

