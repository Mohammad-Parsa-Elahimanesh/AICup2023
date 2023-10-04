#include "Game.h"
#include <map>
#include <chrono>

using namespace std::chrono;
const bool DEBUGMODE = false;//set this to false to prevent extra information

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* user_data) {
	size_t real_size = size * nmemb;
	std::string* response = static_cast<std::string*>(user_data);
	response->append(static_cast<char*>(contents), real_size);
	return real_size;
}

json Game::get(std::string url, std::string api, int port) {
	
	CURL* curl = curl_easy_init();
	if (!curl) {
	    throw std::runtime_error("There is something wrong with the curl");
	}
	
	std::string response;

	// Construct the full URL
	std::string fullUrl = this->server + ":" + std::to_string(this->port) + api;

	// Set the URL
	curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());

   

	// Set custom headers
	
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	// Enable HTTP/2
	curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

	// Reuse connections
	curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

	// Set maximum transfer speed
	curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, 102400L); // 100 KB/s
	curl_easy_setopt(curl, CURLOPT_MAX_SEND_SPEED_LARGE, 102400L); // 100 KB/s

	// Set callback function to handle response
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	// Perform the request
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		throw std::runtime_error("couldnt make the request");
		
		curl_easy_cleanup(curl);

	}
	else {
		if (DEBUGMODE)
				std::cout << "Response: " << response << std::endl;
			
		json result = json::parse(response);
		if(result.find("error")!=result.end()){
			throw std::runtime_error(result["error"]);
		}
		
		curl_easy_cleanup(curl);
		return result;
	}

	// it doesnt reach here 
	return json::parse("");
	



}
json Game::post(std::string url, std::string api, std::vector<std::pair<std::string,std::string>> items, int port) {
	//initialized everytime to be thread safe
	CURL* curl= curl_easy_init();
	if(!curl){
		throw std::runtime_error("there is something wrong with the curl");
	}
	std::string response;

	// Set the URL
	curl_easy_setopt(curl, CURLOPT_URL, (this->server+":"+std::to_string(this->port)+api).c_str());
	std::string body="";
	for(auto& i :items){
		body+=i.first;
		body+="=";
		body+=i.second;
		body+="&";

	}
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
	// Set custom headers
	
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	// Set callback function to handle response
	curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
	curl_easy_setopt(curl, CURLOPT_MAXCONNECTS, 5);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	// Perform the request
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
	
		curl_easy_cleanup(curl);
		throw std::runtime_error("couldnt make the request");

	}
	else {
		
		if (DEBUGMODE)
				std::cout << "Response: " << response << std::endl;
			
		
		json result = json::parse(response);
		if(result.find("error")!=result.end()){
			throw std::runtime_error(result["error"]);
		}
		std::cout<<result["message"]<<std::endl;
		
		curl_easy_cleanup(curl);
		return result;
	}
	return json::parse("");


	

}

void Game::SetToken(std::string token) { this->token = token; }
void Game::SetServersPort(int port) { this->port = port; }
void Game::SetServer(std::string server) { this->server = server; }

int Game::getNumberOfTroopsToPut() {
	auto response = this->get(this->server, "/get_number_of_troops_to_put", this->port);
	if (DEBUGMODE)
		std::cout <<"number of troops:" <<response["number_of_troops"]<<std::endl;
	return response["number_of_troops"].get<int>();



}
bool Game::testServer() {

	auto response = this->get(this->server, "/", this->port);

	if (response.find("message")!=response.end()) {
		
		return true;


	}
	return false;

}
std::map<int, std::vector <int>> Game::getAdj() {
	auto response = this->get(this->server, "/get_adj", this->port);
	
	std::map<int, std::vector<int>> adjacent_matrix;
	
	for (const auto& item : response.items()) {
		if (item.key() != "status") {
			const int key = std::stoi(item.key());
			const std::vector<int>& value = item.value();
			adjacent_matrix[key] = value;
		}
	}
	// Print the map of vectors
	if (DEBUGMODE) {
		for (const auto& item : adjacent_matrix) {
			std::cout << "Key: " << item.first << " | Values: ";
			for (int val : item.second) {
				std::cout << val << " ";
			}
		}
		std::cout << std::endl;
	}
	return adjacent_matrix;


	
	return {};
}
std::map<int, int> Game::getOwners() {
	auto response = this->get(this->server, "/get_owners",  this->port);

	std::map<int, int> owners;

	for (const auto& item : response.items()) {
		if (item.key() != "status") {
			const int key = std::stoi(item.key());
			const int value = item.value().get<int>();
			owners[key] = value;
		}
	}
	// Print the map of vectors
	if (DEBUGMODE) {
		for (const auto& item : owners) {
			std::cout << item.first << ": " << item.second<<"     ";
		}
		std::cout << std::endl;
	}
	return owners;
	

}
std::map<int, int> Game::getNumberOfTroops() {
	auto response = this->get(this->server, "/get_troops_count", this->port);

	std::map<int, int> owners;

	for (const auto& item : response.items()) {
		if (item.key() != "status") {
			const int key = std::stoi(item.key());
			const int value = item.value().get<int>();
			owners[key] = value;
		}
	}
	// Print the map of vectors
	if (DEBUGMODE) {
		for (const auto& item : owners) {
			std::cout << item.first << ": " << item.second << "     ";
		}
		std::cout << std::endl;
	}
	return owners;
	

}
int Game::getState() {
	
	auto response = this->get(this->server, "/get_state", this->port);
	
	if (DEBUGMODE)
			std::cout << response["state"] << std::endl;
	return response["state"].get<int>();
	

}
int Game::getTurnNumber() {
	auto response = this->get(this->server, "/get_turn_number", this->port);

	if (DEBUGMODE)
			std::cout << response["turn_number"] << std::endl;
	return response["turn_number"].get<int>();


}
void Game::nextState() {
	auto response = this->get(this->server, "/next_state", this->port);
	if (DEBUGMODE)
			std::cout <<"next state:"<< response["message"] << std::endl;
	
	
}
void Game::fort(int node,int troop_count){
	auto response = post(this->server, "/fort", 
	{ {"node_id",std::to_string(node)}
	,{"troop_count",std::to_string(troop_count)} }, 
	 this->port);

}
std::map<int, int> Game::getNumberOfFortTroops(){
	auto response = this->get(this->server, "/get_number_of_fort_troops", this->port);
	std::map<int, int> fort_troops;

	for (const auto& item : response.items()) {
		if (item.key() != "status") {
			const int key = std::stoi(item.key());
			const int value = item.value().get<int>();
			fort_troops[key] = value;
		}
	}
	// Print the map of vectors
	if (DEBUGMODE) {
		for (const auto& item : fort_troops) {
			std::cout << item.first << ": " << item.second<<"     ";
		}
		std::cout << std::endl;
	}
	return fort_troops;
	
}
int Game::getPlayerID() {
	auto response = this->get(this->server, "/get_player_id", this->port);

	if (DEBUGMODE)
			std::cout <<"playe id:"<< response["player_id"] << std::endl;
	return  response["player_id"];

}
std::vector<std::pair<int,int>> Game::getStrategicNodes() {
	auto response = this->get(this->server, "/get_strategic_nodes", this->port);
	const std::vector<int>& nodes= response["strategic_nodes"];
	const std::vector<int>& scores= response["score"];
	std::vector<std::pair<int, int>> result;
	for (int i = 0; i < int(nodes.size()); i++) {
		result.push_back({ nodes[i],scores[i] });
		if (DEBUGMODE)
			std::cout << nodes[i] <<":"<< scores[i]<<"   ";
	}
	return result;

	

}
void Game::putOneTroop(int node) {
	//posting data to server
	auto response = post(this->server, "/put_one_troop", { {"node_id",std::to_string(node)}}, this->port);



}
void Game::putTroop(int node, int number_of_troops) {
	//posting data to server
	auto response = post(this->server, "/put_troop", { {"node_id",std::to_string(node).c_str()},{"number_of_troops",std::to_string(number_of_troops).c_str()} }, this->port);
	
}
bool Game::attack(int origin_node, int target_node,float fraction,float move_fraction) {
	//posting data to server
	auto response = post(this->server, "/attack", { {"attacking_id",std::to_string(origin_node).c_str()},
		{"target_id",std::to_string(target_node)},
		{"fraction",std::to_string(fraction)},
		{"move_fraction", std::to_string(move_fraction)}
		}, this->port);
	return response["won"].get<int>();//will be cast to bool
}
void Game::moveTroops(int origin_node, int dest_node, int number_of_troops) {
	//posting data to server
	auto response = post(this->server, "/move_troop", { {"source",std::to_string(origin_node).c_str()},
		{"destination",std::to_string(dest_node).c_str()},
		{"troop_count",std::to_string(number_of_troops).c_str()} }, this->port);
}
std::vector<int> Game::getReachable(int node) {
	//posting data to server
	auto response = post(this->server, "/get_reachable", { {"node_id",std::to_string(node).c_str()},},  this->port);
	return response["reachable"];


}
void Game::SetClient() {
	
	headers = curl_slist_append(headers, ("x-access-token: "+this->token).c_str());
	headers = curl_slist_append(headers, "User-Agent: YourUserAgent"); // Replace with your user agent
	headers = curl_slist_append(headers, "Accept-Encoding: gzip, deflate");
	

	
}

	