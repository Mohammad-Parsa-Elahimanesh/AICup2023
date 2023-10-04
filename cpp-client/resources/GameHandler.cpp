#include "GameHandler.h"
httplib::Server* server;
std::string server_token;
std::vector<std::thread> threads;

const bool DEBUGMODE = false;
bool game_on = true;
std::mutex game_on_mutex;

void handleYourTurn(const httplib::Request& req, httplib::Response& res) {
	//executed when turn request has arrived
	if (req.headers.find("x-access-token") != req.headers.end() 
		&& req.headers.find("x-access-token")->second == server_token) {
		threads.push_back(std::thread(yourTurn));
		res.set_content("everything okay", "text/plain");
		
	}
	else {
		res.set_content("token missing or wrong", "text/plain");

	}
}
void handleInit(const httplib::Request& req, httplib::Response& res) {
	//executed when turn request init arrived
	if (req.headers.find("x-access-token") != req.headers.end() 
		&& req.headers.find("x-access-token")->second == server_token) {
		threads.push_back(std::thread(init));
		res.set_content("everything okay", "text/plain");
	}
	else {
		res.set_content("token missing or wrong", "text/plain");

	}

}
void handleEnd(const httplib::Request& req, httplib::Response& res) {
	//executed when turn request turn arrived
	if (req.headers.find("x-access-token") != req.headers.end()
		&& req.headers.find("x-access-token")->second == server_token) {
		threads.push_back(std::thread(end));
		res.set_content("everything okay", "text/plain");
	}
	else {
		res.set_content("token missing or wrong", "text/plain");

	}

}
void handleKill(const httplib::Request& req, httplib::Response& res) {
	//executed when turn request has arrived
	std::cout<<"kill has been called";
	if (req.headers.find("x-access-token") != req.headers.end()
		&& req.headers.find("x-access-token")->second == server_token) {
		
		game_on_mutex.lock();
		game_on = false;
		game_on_mutex.unlock();
	}
	else {
		res.set_content("token missing or wrong", "text/plain");

	}
}
GameHandler::GameHandler() {
	//default constructor:)
	this->token = "";
	this->id = -1;
	this->port = -1;
	this->host = "127.0.0.1";
	this->server_ip = "127.0.0.1";
	this->server_thread = nullptr;

	server_token = this->generateToken();
	game_on = true;
}
GameHandler::GameHandler(std::string host,std::string server_ip,int server_port) {
	this->token = "";
	this->id = -1;
	this->port = -1;
	this->host = host;
	this->server_port = server_port;
	this->server_thread = nullptr;
	this->server_ip=server_ip;
	server_token = this->generateToken();
	game_on = true;
}

bool GameHandler::begin() {

	json response = this->SendloginRequest(this->server_ip,this->server_port);
	if (response["status"]) {
		if (response.find("player_id") == response.end()) {

			return false;
		}
		else {
			this->port = response["port"].get<int>();
			this->token = response["token"];
			this->id = response["player_id"];
			
			
			if (DEBUGMODE)
				std::cout << "my token:"<<this->token;
			
			

			return true;
		}
	}
	return false;
}
void GameHandler::ready() {
	//we could use shared mutex but I didnt want to get in trouble because of c++ various versions
	std::string message;

	//running server in a new thread
	server = this->runServer(this->host.c_str(), this->port, { {"/turn",handleYourTurn},{"/end",handleEnd},{"/init",handleInit}, {"/kill",handleKill} });
	//waits untill server is running
	while (!server->is_running()) {
		
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	}


	json response = this->request(this->server_ip, "/ready", this->server_port);

	if (response["status"]) {

		message = response["message"];

	}
	else {
		message = "request failed";
	}
	if (DEBUGMODE)
		std::cout << message << std::endl;
	

}


int GameHandler::GetPort() { return this->port; }

int GameHandler::GetServersPort() { return this->server_port; }
bool GameHandler::GetGameOn() {
	bool temp;
	game_on_mutex.lock();
	temp =game_on;
	game_on_mutex.unlock();
	return temp;
}
std::string GameHandler::GetToken() { return this->token; }

httplib::Server* GameHandler::runServer(const char* host, const int PORT, std::vector<std::pair<std::string, std::function<void(const httplib::Request& req, httplib::Response& res)>>> urls) {
	//running server
	server = new httplib::Server();
	for (auto itr : urls)
		server->Get(itr.first, itr.second);

	server_thread = new std::thread([this, host, PORT]() {
		server->listen(host, PORT);
		});




	return server;
}
json GameHandler::SendloginRequest(std::string url, int port) {
	//creating headers including token we send to server for its authentication
	httplib::Client client(url, port);
	httplib::MultipartFormDataItems items = { {"token",generateToken()} };


	//sending request itself
	auto res = client.Post("/login", items);
	json result;
	//validating response and adding status for clearity
	if (res) {
		if (res->status == 200) {
			if (DEBUGMODE)
				std::cout << "login complete" << std::endl;



			result = json::parse(res->body);
			result.push_back({ "status",true });
			return result;
		}
		else {

			result = json::parse(res->body);
			result.push_back({ "status",false });
			return result;
		}
	}
	else {
		result = json::parse("{}");
		result.push_back({ "status",false });
		return result;
	}

	//will not come to this
	return result;
}

json GameHandler::request(std::string url, std::string api, int port) {
	//sends get requests
	
	//creating headers including our token
	httplib::Headers headers;
	headers.emplace("x-access-token", this->token);
	httplib::Client client(url, port);

	auto res = client.Get(api, headers);

	json result;
	//validating response
	if (res) {

		if (res->status == 200) {

			if (DEBUGMODE)
				std::cout << "request sent to " << api << "successfully" << std::endl;
			result = json::parse(res->body);
			result.push_back({ "status",true });
			return result;
		}
		else {
			result = json::parse(res->body);
			result.push_back({ "status",false });
			return result;
		}
	}
	else {
		result = json::parse("{}");
		result.push_back({ "status",false });
		return result;
	}

	//will not come to this
	return result;


}
void GameHandler::join() {
	//joining all threads that has been started
	if (server_thread != nullptr) {
		server->stop();
		server_thread->join();
	}
	for (int i = 0; i < int(threads.size()); i++)
		threads[i].join();


}



std::string GameHandler::generateToken() {
	//generating a random token 
	const int otpLength = 32;
	const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	std::string otp;
	std::mt19937 rng(std::time(0));
	std::uniform_int_distribution<int> dist(0, chars.size() - 1);

	for (int i = 0; i < otpLength; ++i) {
		otp += chars[dist(rng)];
	}
	if (DEBUGMODE)
		std::cout << "server token: " << otp << std::endl;
	
	return otp;



}

GameHandler::~GameHandler() {


}


