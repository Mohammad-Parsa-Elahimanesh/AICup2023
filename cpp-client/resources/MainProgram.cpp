// In the name of Allah the most compassionate the most merciful

#include "MainProgram.h"

#include <bits/stdc++.h>

using namespace std;

Game game;

//////////////////////////////////////// Defines ////////////////////////////////////////

typedef long long ll;
typedef long double ld;

#define sz(x) (int)x.size()
#define all(x) x.begin(), x.end()

//////////////////////////////////////// Variables ////////////////////////////////////////

const int MAX_SOLDIERS = 150 , MIN_SOLDIERS = 1;
const int MAX_DISTANCE = 20;

const ld ATTACK_THRESHOLD = 2.0L / 3.0L;
const ld STRATEGIC_UNSTABILITY_THRESHOLD = 0.6L, FAST_ATTACK_STRATEGIC_UNSTABILITY_THRESHOLD = 0.7L;
;
const ld FIRST_DEFENSE_THRESHOLD = 0.5L , SECOND_DEFENSE_THRESHOLD = 0.75L;

const ld FAST_ATTACK_THRESHOLD = 2.0L / 3.0L , FAST_ATTACK_RETURN_RATE = 1.0L;
const ld FAILURE_PROBABILITY_THRESHOLD = 0.05L;

const ld MOVE_RATE = 0.6L , MOVE_RATE_TO_STRATEGIC = 0.8L, MOVE_RATE_FROM_STRATEGIC = 0.3L;
const ld MOVE_RATE_TO_STRATEGIC_FROM_STRATEGIC = 0.55L;
const ld RETURN_RATE = 1.0L;

bool isInitialized = false;

int numberOfNodes, numberOfEdges , myID;
vector<vector<int>> graph;

vector<int> score;

int turnNumber, state, unusedTroops;

vector<int> owner, numberOfTroops, numberOfForts, totalNumberOfTroops;

vector<vector<int>> nodesDistance;

ld probabilityOfWinning[MAX_SOLDIERS][MAX_SOLDIERS]; // No. of defensive soldiers - No. of attacking soldiers
ld expectedAttackLostSoldiers[MAX_SOLDIERS][MAX_SOLDIERS];

int fortTurn = -1 , fortNode = -1 , fortTroops = 0;
bool successfulAttack = false;

//////////////////////////////////////// Initialize ////////////////////////////////////////

inline void CalculateDistances(){
	nodesDistance.resize(numberOfNodes);
	for(int v = 0 ; v < numberOfNodes ; v++) nodesDistance[v].resize(numberOfNodes , numberOfNodes);
	for(int v = 0 ; v < numberOfNodes ; v++){
		for(int u : graph[v]){
			nodesDistance[v][u] = 1;
		}
	}
	for(int k = 0 ; k < numberOfNodes ; k++){
		for(int v = 0 ; v < numberOfNodes ; v++){
			for(int u = 0 ; u < numberOfNodes ; u++){
				nodesDistance[v][u] = min(nodesDistance[v][u] , nodesDistance[v][k] + nodesDistance[k][u]);
			}
		}
	}
}

inline void CalculateAttackParameters(){
	ld war[3][4][3][3] = {};
	for(int d = 1 ; d <= 2 ; d++){
		for(int a = 1 ; a <= 3 ; a++){
			int total = pow(6 , a + d);
			for(int msk = 0 ; msk < total ; msk++){
				vector<int> ddice , adice;
				int x = msk;
				for(int i = 0 ; i < d ; i++){
					ddice.push_back(x % 6);
					x /= 6;
				}
				for(int i = 0 ; i < a ; i++){
					adice.push_back(x % 6);
					x /= 6;
				}
				sort(all(ddice) , greater<int>()) , sort(all(adice) , greater<int>());
                int dk = 0 , ak = 0;
				for(int i = 0 ; i < min(d , a) ; i++){
					if(ddice[i] >= adice[i]) ak++;
					else dk++;
				}
				war[d][a][dk][ak] += 1.00L / (ld)total;
			}
		}
	}
    for(int a = 2 ; a < MAX_SOLDIERS ; a++) probabilityOfWinning[0][a] = 1.00L;
	
	for(int d = 1 ; d < MAX_SOLDIERS ; d++){
		for(int a = 2 ; a < MAX_SOLDIERS ; a++){
			if((ld)a <= RETURN_RATE * (ld)d) continue;
			int dw , aw;
			if(d == 1) dw = 1;
			else dw = 2;
			if(a == 2) aw = 1;
			else if(a == 3) aw = 2;
			else aw = 3;
			for(int dk = 0 ; dk <= dw ; dk++){
				for(int ak = 0 ; ak <= aw ; ak++){
                    if(dk + ak != min(dw , aw)) continue;
					ld poc = war[dw][aw][dk][ak];
					ld winP = probabilityOfWinning[d-dk][a-ak]; 
					probabilityOfWinning[d][a] += poc * winP;
					expectedAttackLostSoldiers[d][a] += poc * winP * (ak + expectedAttackLostSoldiers[d-dk][a-ak]);
				}
			}
			probabilityOfWinning[d][a] = max(0.01L, min(0.99L, probabilityOfWinning[d][a]));
			expectedAttackLostSoldiers[d][a] /= probabilityOfWinning[d][a];
		}
	}
}

inline void Init(){
	isInitialized = true;

	myID = game.getPlayerID();

	map<int,vector<int>> _graph = game.getAdj();
	numberOfNodes = _graph.size();
	graph.resize(numberOfNodes);
	for(int v = 0 ; v < numberOfNodes ; v++){
		graph[v] = _graph[v];
	}

	score.resize(numberOfNodes , 0);
	vector<pair<int,int>> _strategicNodes = game.getStrategicNodes();
	for(auto [v , p] : _strategicNodes){
		score[v] = p;
	}

	owner.resize(numberOfNodes , -1);
	numberOfTroops.resize(numberOfNodes , 0);
	numberOfForts.resize(numberOfNodes , 0);
	totalNumberOfTroops.resize(numberOfNodes , 0);

	CalculateDistances();
	CalculateAttackParameters();
}

//////////////////////////////////////// Updates ////////////////////////////////////////

inline void UpdateBasicData(){
	turnNumber = game.getTurnNumber();
	state = game.getState();
	unusedTroops = game.getNumberOfTroopsToPut();
	successfulAttack = false;
}

inline void UpdateOwners(){
	map<int,int> _owner = game.getOwners();
	for(int v = 0 ; v < numberOfNodes ; v++){
		owner[v] = _owner[v];
	}
}
inline void UpdateNumberOfTroops(){
	map<int,int> _numberOfTroops = game.getNumberOfTroops();
	for(int v = 0 ; v < numberOfNodes ; v++){
		numberOfTroops[v] = _numberOfTroops[v];
	}
	map<int,int> _numberOfForts = game.getNumberOfFortTroops();
	for(int v = 0 ; v < numberOfNodes ; v++){
		numberOfForts[v] = _numberOfForts[v];
	}
	for(int v = 0 ; v < numberOfNodes ; v++){
		totalNumberOfTroops[v] = numberOfTroops[v] + numberOfForts[v];
	}
}


inline void UpdateData(){
	UpdateOwners();
	UpdateNumberOfTroops();
}


//////////////////////////////////////// Tools ////////////////////////////////////////

inline void NextState(){
	game.nextState();
	state += 1;
	UpdateData();
}


void PutTroop(int v , int cnt){
	game.putTroop(v , cnt);
	unusedTroops -= cnt;
	owner[v] = myID;
	numberOfTroops[v] += cnt;
	totalNumberOfTroops[v] += cnt;
}

void Attack(int from , int to , ld returnRate , ld moveRate) {
	game.attack(from , to , returnRate , moveRate);
	UpdateData();
	if(owner[to] == myID) successfulAttack = true;
}

inline int MaxEmptyStrategicNode(){
	int res = -1;
	for(int i = 0; i < numberOfNodes; i++)
		if(owner[i] == -1 && score[i] > 0 && (res == -1 or score[i] > score[res]))
			res = i;
	return res;
}

inline int NumberOfAttackersInDistance(int center, int radius, int _owner) {
	int res = 0;
	for(int i = 0; i < numberOfNodes; i++)
		if(owner[i] == _owner && nodesDistance[center][i] <= radius)
			res += numberOfTroops[i];
	return res;
}

inline int NumberOfAttackersAtDistanceAgaints(int center, int radius, int defender) {
	int res = 0;
	for(int i = 0; i < 3; i++)
		if(i != defender)
			res += NumberOfAttackersInDistance(center, radius, i);
	return res;
}

inline int NumberOfDefenserAtDistance(int center, int radius, int _owner) {
	int res = 0;
	for(int i = 0; i < numberOfNodes; i++)
		if(owner[i] == _owner && nodesDistance[center][i] <= radius)
			res += totalNumberOfTroops[i];
	return res;
}

inline vector<int> AtDistanse(int v, int r) {
	vector<int> res;
	for(int i = 0; i < numberOfNodes; i++)
		if(nodesDistance[v][i] <= r)
			res.push_back(i);
	return res;
}

inline int Power(int player){
	int countGivenX4 = 0;
	int countStrategic = 0;
	int countSolider = 0;
	for(int i = 0; i < numberOfNodes; i++)
		if(owner[i] == player) {
			countGivenX4 += 1+score[i]*4;
			countStrategic += (score[i] ? 1 : 0);
			countSolider += numberOfTroops[i] + numberOfForts[i]/2;
		}
	return (7 * countGivenX4 + 28 * countStrategic + 4 * countSolider) / 28;
}

const bool isStronger(int a, int b) {
	int as = Power(a), bs = Power(b);
	return (as-bs) > 2;
}

inline int weakestExceptMe() {
	int a = (myID+1)%3, b = (myID+2)%3;
	if(isStronger(a, b)) return b;
	else if(isStronger(b, a)) return a;
	return -1;
}

struct troopsToPutOrder{
	int node;
	int troopsAtDis , dis;

	troopsToPutOrder(int _node , int _troopsAtDis , int _dis){
		node = _node;
		troopsAtDis = _troopsAtDis , dis = _dis;
	}
	bool operator<(troopsToPutOrder& a){
		if(troopsAtDis != a.troopsAtDis) return (troopsAtDis < a.troopsAtDis);
		if(dis != a.dis) return (dis < a.dis);
		return (node < a.node);
	}
};

struct attackOption{
	int from , to;
	int neededTroops;
	
	attackOption(int _from , int _to , int _neededTroops){
		from = _from;
		to = _to;
		neededTroops = _neededTroops;
	}
	int value(){
		return Power(owner[to]) - neededTroops + 2 * score[to];
	}
	bool operator<(attackOption& a){
		if(value() != a.value()) return (value() > a.value());
		return (from < a.from);
	}
};

struct strategicNodeDefenseOption{
	int node;
	int neededTroops;

	strategicNodeDefenseOption(int _node , int _neededTroops){
		node = _node;
		neededTroops = _neededTroops;
	}
	int priority(){
		return neededTroops - 2 * score[node];
	}
	bool operator<(strategicNodeDefenseOption& a){
		if(priority() != a.priority()) return (priority() < a.priority());
		return (node < a.node);
	}
};

inline vector<int> Nodes(int _owner) {
	vector<int> res;
	for(int i = 0; i < numberOfNodes; i++)
		if(owner[i] == _owner)
			res.push_back(i);
	return res;
}

inline vector<int> MyStrategicNodes() {
	vector<int> res;
	for(auto u: Nodes(myID))
		if(score[u])
			res.push_back(u);
	for(int i = 1; i < sz(res); i++)
		for(int j = 0; j < i; j++)
			if(score[res[j]] < score[res[i]])
				swap(res[i], res[j]);
	return res;	
}

inline vector<int> EmptyNeighbours() {
	vector<int> res;
	for(auto u: Nodes(-1))
		for(auto v: Nodes(myID))
			if(nodesDistance[u][v] == 1)
			{
				res.push_back(u);
				break;
			}
	return res;
}

struct threatTree {
	int node, updater = -1;
	ld need;
	bool operator<(const threatTree &other) const {
		return (need == other.need? node < other.node : need < other.need);
	}
};

inline vector<threatTree> getThreatTree(int v) {
	vector<threatTree> t[3];
	for(int U = 0; U < 3; U++) {
		if(U == myID) continue;
		t[U].resize(numberOfNodes);
		set<threatTree> s;
		for(int i = 0; i < numberOfNodes; i++)
			t[U][i].node = i, t[U][i].need = (i == v?0: MAX_SOLDIERS), s.insert(t[U][i]);
		
		while (!s.empty())
		{
			auto top = (*s.begin()).node;
			s.erase(s.begin());
			if(owner[top] == -1 || owner[top] == U || t[U][top].need >= MAX_SOLDIERS) continue;
			for(auto u: graph[top]) {
				int countHold = max(1, (score[u]? 6 - (owner[u] == U? numberOfForts[u] : 0) : 0));
				ld _need = t[U][top].need+ceil(expectedAttackLostSoldiers[totalNumberOfTroops[top]][MAX_SOLDIERS-1])+countHold;
				if(t[U][u].need > _need) {
					s.erase(t[U][u]);
					t[U][u].updater = top;
					t[U][u].need = _need;
					s.insert(t[U][u]);
				}
			}
		}
	}
	pair<int, int> mainEnemy = {-1, -1};
	for(int U = 0; U < 3; U++) if(U != myID) {
		for(int i = 0; i < numberOfNodes; i++) {
			if(owner[i] == U or owner[i] == -1) {
				if(t[U][i].need < MAX_SOLDIERS && (mainEnemy == make_pair(-1, -1) ||
					(t[mainEnemy.first][mainEnemy.second].need-numberOfTroops[mainEnemy.second] > t[U][i].need-numberOfTroops[i])))
					mainEnemy = {U, i};
			}
		}
	}
	return t[mainEnemy.first];
}

inline int NewEnemyPutsToGet(int v){return 3+3*score[v];}

ld Instability(int v , int defTroops , vector<int> ignore = {}){
	vector<int> _total = totalNumberOfTroops, _owner = owner;	
	owner[v] = myID;
	totalNumberOfTroops[v] = defTroops;
	for(auto u: ignore) if(owner[u] != myID) owner[u] = myID, totalNumberOfTroops[u] = 1+(score[u]?6:0);

	vector<threatTree> threats = getThreatTree(v);
	int mainEnemy = -1;
	for(int i = 0; i < numberOfNodes; i++)
		if(owner[i] != myID && threats[i].need < MAX_SOLDIERS && (mainEnemy == -1 ||
			(threats[mainEnemy].need-numberOfTroops[mainEnemy] > threats[i].need-numberOfTroops[i])))
			mainEnemy = i;
	ld _reachP = 0.0L;
	if(mainEnemy != -1) {
		_reachP = 1.0L;
		int _countTroops = numberOfTroops[mainEnemy]+NewEnemyPutsToGet(v);
		while (mainEnemy != v)
		{
			if(_countTroops < 1) {
				_reachP = 0.0L;
				break;
			}
			_reachP *= probabilityOfWinning[totalNumberOfTroops[threats[mainEnemy].updater]][_countTroops];
			int countHold = max(1, (score[mainEnemy]?6-numberOfForts[mainEnemy]:0));
			_countTroops -= countHold+expectedAttackLostSoldiers[totalNumberOfTroops[threats[mainEnemy].updater]][_countTroops];
			mainEnemy = threats[mainEnemy].updater;
		}
	}
	totalNumberOfTroops = _total, owner = _owner;
	return _reachP;
}

ld Instability(int v){
	return Instability(v , totalNumberOfTroops[v], {});
}

int MinNeededTotalTroopsToReachInstability(int v , ld instability , vector<int> ignore = {}){ // Independent with node's troops
	int l = -1, r = MAX_SOLDIERS;
	while (l+1 < r)
	{
		int mid = (l+r)/2;
		if(Instability(v, mid, ignore) <= instability) r = mid;
		else l = mid;
	}
	return r;
}

int MinNeededTroopsToReachInstability(int v , ld instability){ // it uses previous troops of node
	return max(0, MinNeededTotalTroopsToReachInstability(v, instability)-totalNumberOfTroops[v]);
}

bool isGoodToAttack(int def , int att){
	return (probabilityOfWinning[def][att] >= ATTACK_THRESHOLD and (att - expectedAttackLostSoldiers[def][att]) * MOVE_RATE >= MIN_SOLDIERS);
}
bool isGoodToAttackFromStrategic(int to , int from , int toTroops , int fromTroops){
	bool ret = true;
	ret &= (probabilityOfWinning[toTroops][fromTroops] >= ATTACK_THRESHOLD); // in case of changing ATTACK_THRESHOLD for stratgic nodes
	ret &= ((fromTroops - expectedAttackLostSoldiers[toTroops][fromTroops]) * MOVE_RATE_FROM_STRATEGIC >= MIN_SOLDIERS);
	ret &= (Instability(from , (int)((1.0L -  MOVE_RATE_FROM_STRATEGIC) * ((ld)fromTroops - expectedAttackLostSoldiers[toTroops][fromTroops])) + numberOfForts[from], {to}) <= STRATEGIC_UNSTABILITY_THRESHOLD);
	return ret;
}
bool isGoodToAttackToStrategic(int to , int from , int toTroops , int fromTroops){ // from new nodes view
	bool ret = true;
	ret &= (probabilityOfWinning[toTroops][fromTroops] >= ATTACK_THRESHOLD);
	ret &= (Instability(to , (int)(MOVE_RATE_TO_STRATEGIC * ((ld)fromTroops - expectedAttackLostSoldiers[toTroops][fromTroops]))) <= STRATEGIC_UNSTABILITY_THRESHOLD);
	return ret;
}
bool isGoodToAttackToStrategicFromStrategic(int to , int from , int toTroops , int fromTroops){ // from old and new nodes view
	bool ret = true;
	ret &= (probabilityOfWinning[toTroops][fromTroops] >= ATTACK_THRESHOLD);
	ret &= (Instability(to , (int)(MOVE_RATE_TO_STRATEGIC_FROM_STRATEGIC * ((ld)fromTroops - expectedAttackLostSoldiers[toTroops][fromTroops]))) <= STRATEGIC_UNSTABILITY_THRESHOLD);
	ret &= (Instability(from , (int)((1.0L - MOVE_RATE_TO_STRATEGIC_FROM_STRATEGIC) * ((ld)fromTroops - expectedAttackLostSoldiers[toTroops][fromTroops])) + numberOfForts[from] , {to}) <= STRATEGIC_UNSTABILITY_THRESHOLD);
	return ret;
}

struct attack{
	int from , to;
	ld moveRate;

	attack(int _from , int _to , ld _moveRate){
		from = _from , to = _to;
		moveRate = _moveRate;
	}
};

struct strategicNodeEmergency{
	int node;

	strategicNodeEmergency(int _node){
		node = _node;
	}
	bool isLessThanMin(){
		return (totalNumberOfTroops[node] < 7 ? true : false);
	}
	ld emergency(){
		return Instability(node) + (ld)score[node] / 10.0L - (ld)totalNumberOfTroops[node] / 20.0L;
	}
	bool operator<(strategicNodeEmergency& a){
		if(isLessThanMin() != a.isLessThanMin()) return isLessThanMin();
		if(emergency() != a.emergency()) return (emergency() > a.emergency());
		return (node < a.node);
	}
};

vector<int> MyNodes(){
	vector<int> ret;
	for(int v = 0 ; v < numberOfNodes ; v++){
		if(owner[v] == myID){
			ret.push_back(v);
		}
	}
	return ret;
}
vector<int> OpponentStrategicNodes(){
	vector<int> ret;
	for(int v = 0 ; v < numberOfNodes ; v++){
		if(owner[v] != -1 and owner[v] != myID and score[v] > 0){
			ret.push_back(v);
		}
	}
	return ret;
}

int NumberOfMyTroops(int node){
	if(owner[node] != myID) return 0;
	return numberOfTroops[node];
}
int TotalNumberOfEnemyTroops(int node){
	if(owner[node] == myID) return 0;
	return totalNumberOfTroops[node];
}

//////////////////////////////////////// Game Strategy ////////////////////////////////////////

/*-------------- Arrange Solider --------------*/

void PutOneTroop(int v){
	game.putOneTroop(v);
	game.nextState();
}

inline void ArrangeSoldiers(){
	int maxEmptyStrategicNode = MaxEmptyStrategicNode();
	if(maxEmptyStrategicNode != -1){
		PutOneTroop(maxEmptyStrategicNode);
		return;
	}

	vector<strategicNodeEmergency> strategicNodesToDefense;
	for(int v : MyStrategicNodes()){
		strategicNodeEmergency a(v);
		strategicNodesToDefense.push_back(v);
	}
	sort(all(strategicNodesToDefense));
	PutOneTroop(strategicNodesToDefense[0].node);
}

/*-------------- Arrange Soliders --------------*/

void DefenseStrategicNodes(){
	vector<strategicNodeDefenseOption> options;
	for(int node : MyStrategicNodes()){
		strategicNodeDefenseOption option(node , MinNeededTroopsToReachInstability(node , FIRST_DEFENSE_THRESHOLD));
		options.push_back(option);
	}
	sort(all(options));
	for(strategicNodeDefenseOption option : options){
		if(unusedTroops >= option.neededTroops){
			if(option.neededTroops) PutTroop(option.node , option.neededTroops);
		}
		else{
			int secondNeededTroops = MinNeededTroopsToReachInstability(option.node , SECOND_DEFENSE_THRESHOLD);
			if(unusedTroops >= secondNeededTroops){
				if(unusedTroops) PutTroop(option.node , unusedTroops);
			}
		}
	}
}

void SpreadSoldiers(){
	bool flag = false;
	for(int d = 1 ; d < MAX_DISTANCE ; d++){
		for(int myNode : MyNodes()){
			for(int newNode = 0 ; newNode < numberOfNodes ; newNode++){
				if(nodesDistance[myNode][newNode] == d){
					if(owner[newNode] == -1 and unusedTroops >= MIN_SOLDIERS){
						PutTroop(newNode , MIN_SOLDIERS);
						flag = true;
					}
				}
			}
		}
		if(flag) break;
	}
}

inline vector<attack> AddTroopsToAttack(){
	vector<attack> attacks;
	bool isUsedToAttackFrom[numberOfNodes] = {};
	bool isUsedToAttackTo[numberOfNodes] = {};

	vector<attackOption> strategicOptions;
	for(int from : Nodes(myID)){
		for(int to : graph[from]){
			if(from == fortNode or isUsedToAttackFrom[from] or isUsedToAttackTo[to]) continue;
			if(owner[to] != -1 and owner[to] != myID and score[to] > 0){
				for(int addedTroops = 0 ; addedTroops <= unusedTroops ; addedTroops++){
					int totalTroops = numberOfTroops[from] + addedTroops;
					if(score[from] == 0){
						if(isGoodToAttackToStrategic(to , from , totalNumberOfTroops[to] , totalTroops)){
							attackOption option(from , to , addedTroops);
							strategicOptions.push_back(option);
							break;
						}
					}
					else{
						if(isGoodToAttackToStrategicFromStrategic(to , from , totalNumberOfTroops[to] , totalTroops)){
							attackOption option(from , to , addedTroops);
							strategicOptions.push_back(option);
							break;
						}
					}
				}
			}
		}
	}
	sort(all(strategicOptions));
	for(attackOption option : strategicOptions){
		if(unusedTroops >= option.neededTroops){
			if(!isUsedToAttackFrom[option.from] and !isUsedToAttackTo[option.to]){
				isUsedToAttackFrom[option.from] = isUsedToAttackTo[option.to] = true;
				if(option.neededTroops) PutTroop(option.from , option.neededTroops);
				attack attck(option.from , option.to , (score[option.from]?MOVE_RATE_TO_STRATEGIC_FROM_STRATEGIC : MOVE_RATE_TO_STRATEGIC));
				attacks.push_back(attck);
			}
		}
	}

	SpreadSoldiers();
	
	vector<attackOption> options;
	for(int from : Nodes(myID)){
		for(int to : graph[from]){
			if(from == fortNode or isUsedToAttackFrom[from] or isUsedToAttackTo[to]) continue;
			if(owner[to] != -1 and owner[to] != myID){
				for(int addedTroops = 0 ; addedTroops <= unusedTroops ; addedTroops++){
					int totalTroops = numberOfTroops[from] + addedTroops;
					if(score[from] == 0){
						if(isGoodToAttack(totalNumberOfTroops[to] , totalTroops)){
							attackOption option(from , to , addedTroops);
							options.push_back(option);
							break;
						}
					}
					else{
						if(isGoodToAttackFromStrategic(to , from , totalNumberOfTroops[to] , totalTroops)){
							attackOption option(from , to , addedTroops);
							options.push_back(option);
							break;
						}
					}
				}
			}
		}
	}
	ld failProbability = 1.0L;
	sort(all(options));
	for(attackOption option : options){
		if(owner[option.to] == weakestExceptMe() and failProbability <= FAILURE_PROBABILITY_THRESHOLD) continue;
		if(unusedTroops >= option.neededTroops){
			if(!isUsedToAttackFrom[option.from] and !isUsedToAttackTo[option.to]){
				isUsedToAttackFrom[option.from] = isUsedToAttackTo[option.to] = true;
				if(option.neededTroops) PutTroop(option.from , option.neededTroops);
				failProbability *= (1 - probabilityOfWinning[totalNumberOfTroops[option.to]][numberOfTroops[option.from]]);
				attack attck(option.from , option.to , (score[option.from]? MOVE_RATE_FROM_STRATEGIC : MOVE_RATE));
				attacks.push_back(attck);
			}
		}
	}
	
	return attacks;
}

void SlowBuildupAttack(vector<attack> attacks){
	for(auto [from , to , moveRate] : attacks){
		int attackTroops = numberOfTroops[from];
		if(score[from] == 0){
			if(isGoodToAttack(totalNumberOfTroops[to] , attackTroops)){
				Attack(from , to , RETURN_RATE , moveRate);
			}
		}
		else{
			if(isGoodToAttackFromStrategic(to , from , totalNumberOfTroops[to] , attackTroops)){
				Attack(from , to , RETURN_RATE , moveRate);
			}
		}
	}
}

int NeededTroopsForStability(int node , vector<int> ignore = {}){
	if(score[node] == 0) return 1;
	return max(6 , MinNeededTotalTroopsToReachInstability(node , FAST_ATTACK_STRATEGIC_UNSTABILITY_THRESHOLD , ignore));
}

int newTroopsUpperBound(int node){
	return (7 * score[node] + 3);
}

bool FastAttack(){
	int minTroops[numberOfNodes][numberOfNodes] , updater[numberOfNodes][numberOfNodes];
	ld probabilityForMin[numberOfNodes][numberOfNodes] = {};
	ld moveRateForMin[numberOfNodes][numberOfNodes] = {};
	for(int i = 0 ; i < numberOfNodes ; i++) for(int j = 0 ; j < numberOfNodes ; j++) minTroops[i][j] = MAX_SOLDIERS;

	for(int dest = 0 ; dest < numberOfNodes ; dest++){
		if(score[dest] == 0 || owner[dest] == myID) continue;

		minTroops[dest][dest] = NeededTroopsForStability(dest);
		
		probabilityForMin[dest][dest] = 1.0L;
		updater[dest][dest] = -1;

		priority_queue<pair<int,int> , vector<pair<int,int>> , greater<pair<int,int>>> pq;
		pq.push({minTroops[dest][dest] , dest});
		
		while(pq.size()){
			auto [dv , v] = pq.top();
			pq.pop();
			if(minTroops[dest][v] != dv) continue;

			if(TotalNumberOfEnemyTroops(v) == 0) continue;

			int vTroops = TotalNumberOfEnemyTroops(v);
			ld vProbability = probabilityForMin[dest][v];
			for(int u : graph[v]){
				vector<int> ignored;
				int lastInPath = v;
				while (lastInPath != -1) ignored.push_back(lastInPath), lastInPath = updater[dest][lastInPath];
				int uStabilityNeed = max(1 , NeededTroopsForStability(u , ignored) - numberOfForts[u]);
				for(ld moveRate = 0.99L ; moveRate > 0.0L ; moveRate -= 0.05L){
					int offer = 100;
					for(int x = 0 ; x < 100 ; x++){
						ld uProbability = probabilityOfWinning[vTroops][x] * vProbability;
						if(uProbability >= FAST_ATTACK_THRESHOLD and
						floor((ld)x - expectedAttackLostSoldiers[vTroops][x]) * moveRate >= minTroops[dest][v] and
						((ld)x - expectedAttackLostSoldiers[vTroops][x]) * (1.0L - moveRate) + 1 >= uStabilityNeed){
							offer = x;
							break;
						}
					}
					
					if(offer < minTroops[dest][u]){
						minTroops[dest][u] = offer , updater[dest][u] = v;
						probabilityForMin[dest][u] = probabilityOfWinning[vTroops][offer] * vProbability;
						moveRateForMin[dest][u] = moveRate;
						pq.push({minTroops[dest][u] , u});
					}
				}
			}
		}
	}
	
	vector<attackOption> options;
	for(int attackDest : OpponentStrategicNodes()){
		for(int attackSource = 0 ; attackSource < numberOfNodes ; attackSource++){
			if(TotalNumberOfEnemyTroops(attackSource) > 0) continue;
			int newTroops = max(0 , minTroops[attackDest][attackSource] - NumberOfMyTroops(attackSource));
			if(newTroops <= unusedTroops){
				attackOption option(attackSource , attackDest , newTroops);
				options.push_back(option);
			}
		}
	}

	sort(all(options));
	vector<pair<int,int>> attacks;
	bool isAttackedFrom[numberOfNodes] = {} , isAttackedTo[numberOfNodes] = {};
	for(auto [attackSource , attackDest , newTroops] : options){
		if(!isAttackedFrom[attackSource] and !isAttackedTo[attackDest] and newTroops <= unusedTroops){
			if(newTroops <= newTroopsUpperBound(attackDest)){
				isAttackedFrom[attackSource] = isAttackedTo[attackDest] = true;
				attacks.push_back({attackSource , attackDest});
				if(newTroops > 0) PutTroop(attackSource , newTroops);
			}
		}
	}

	if(!attacks.empty()){
		NextState();
		for(auto [attackSource , attackDest] : attacks){
			int currentNode = attackSource;
			while(currentNode != attackDest and owner[currentNode] == myID){
				int nextNode = updater[attackDest][currentNode];
				ld moveRate = moveRateForMin[attackDest][currentNode];
				Attack(currentNode , nextNode , FAST_ATTACK_RETURN_RATE , moveRate);
				currentNode = nextNode;
			}
		}
		return true;
	}
	return false;
}

//////////////////////////////////////// Finish It ////////////////////////////////////////

struct FinishNode {
	int node, expectedSoliders, updater;
	ld reachP; // possebilityOfReach
	bool operator<(const FinishNode &a) const {
		if(reachP == a.reachP)
			return expectedSoliders == a.expectedSoliders ? node < a.node : expectedSoliders > a.expectedSoliders;
		return reachP > a.reachP;
	}
	bool operator==(const FinishNode &other) {
		return other.node == node && other.expectedSoliders == expectedSoliders && other.updater == updater;
	}
};

inline deque<int> PathOfAttack(int node, vector<FinishNode> finishNode) {
	deque<int> pathOfAttack = {node};
	while(finishNode[pathOfAttack.front()].updater != pathOfAttack.front())
		pathOfAttack.push_front(finishNode[pathOfAttack.front()].updater);
	return pathOfAttack;
}

inline int countOtherStrategicInPath(int node, vector<FinishNode> finishNode) {
	int res = 0;
	for(auto u: PathOfAttack(node, finishNode))
		if(score[u] > 0 && owner[u] != myID)
			res++;
	return res;
}

constexpr bool active = true;

inline void JustAttack() {
	if(!active) return;
	if(state == 1) {
		pair<int, int> bestToPut = {-MAX_SOLDIERS, -1};
		for(int i = 0; i < numberOfNodes; i++)
			if(owner[i] == -1 && unusedTroops > 0)
				PutTroop(i, 1);
			
		for(int i = 0; i < numberOfNodes; i++) {
			if(owner[i] == myID)
				for(auto u: graph[i])
					if(owner[u] != myID && owner[u] != -1)
						bestToPut = max(bestToPut, {numberOfTroops[i]-totalNumberOfTroops[u],i});
		}
		if(bestToPut.second >= 0 && unusedTroops > 0)
			PutTroop(bestToPut.second, unusedTroops);
		NextState();
	}
	successfulAttack = true;
	while (successfulAttack)
	{
		successfulAttack = false;
		vector<pair<ld, pair<int, int>>> _all;
		for(int i = 0; i < numberOfNodes; i++)
			for(auto u: graph[i])
				if(owner[i] == myID && owner[u] != myID && owner[u] != -1)
					_all.push_back({-probabilityOfWinning[totalNumberOfTroops[u]][numberOfTroops[i]]+0.001*totalNumberOfTroops[u]-0.001*numberOfTroops[i],{i, u}});
		sort(all(_all));
		for(auto [p, w]: _all)
			if(owner[w.second] != myID && numberOfTroops[w.first] > 1)
				Attack(w.first, w.second, 0.01, 0.99);
	}
}

inline bool FinishIt() {
	if(!active) return false;
	if(turnNumber < 126) return false;
	vector<FinishNode> finishNode(numberOfNodes);
	set<FinishNode> _nearest;
	for(int i = 0; i < numberOfNodes; i++) {
		finishNode[i].node = finishNode[i].updater = i;
		if(owner[i] == myID or owner[i] == -1) {
			finishNode[i].expectedSoliders = unusedTroops+numberOfTroops[i];
			finishNode[i].reachP = 1;
		} else finishNode[i].expectedSoliders = finishNode[i].reachP = 0;
		_nearest.insert(finishNode[i]);
	}
	while(!_nearest.empty()) {	
		auto _v = *_nearest.begin();
		_nearest.erase(_nearest.begin());
		if(!(_v == finishNode[_v.node]))continue;
		for(auto _u: graph[_v.node]) {
			if(owner[_u] != myID && owner[_u] != -1 && finishNode[_u].reachP < _v.reachP*probabilityOfWinning[totalNumberOfTroops[_u]][_v.expectedSoliders]) {
				_nearest.erase(finishNode[_u]);
				finishNode[_u].reachP = _v.reachP*probabilityOfWinning[totalNumberOfTroops[_u]][_v.expectedSoliders];
				finishNode[_u].expectedSoliders = _v.expectedSoliders-expectedAttackLostSoldiers[totalNumberOfTroops[_u]][_v.expectedSoliders]-1;
				finishNode[_u].updater = _v.node;
				_nearest.insert(finishNode[_u]);
			}
		}
	}
	pair<pair<int, ld>, int> node = {{0, 0}, -1};
	for(int i = 0; i < numberOfNodes; i++)
		if(finishNode[i].reachP > ATTACK_THRESHOLD && owner[i] != -1 && owner[i] != myID)
			node = max(node, {{countOtherStrategicInPath(i, finishNode), finishNode[i].reachP}, i});
	
	if((node.first.first+sz(MyStrategicNodes()) >= 4 || turnNumber == 165 ) && node.first.first > 0) {
		deque<int> pathOfAttack = PathOfAttack(node.second, finishNode);
		if(unusedTroops) PutTroop(pathOfAttack.front() , unusedTroops);
		NextState();
		while(pathOfAttack.size() > 1 && owner[pathOfAttack.front()] == myID) {
			Attack(pathOfAttack[0], pathOfAttack[1], FAST_ATTACK_RETURN_RATE , 0.99L);
			pathOfAttack.pop_front();
		}
		UpdateData();
		if(sz(MyStrategicNodes()) >= 4 || turnNumber == 165)
			unusedTroops = 0, JustAttack();
		return true;
	}
	return false;
}

//////////////////////////////////////// Move ////////////////////////////////////////

vector<int> head;
int getHead(int v){return v == head[v]? v : head[v] = getHead(head[v]);}

inline bool isInEdge(int v) {
	for(auto u: graph[v])
		if(owner[u] != -1 && owner[u] != owner[v])
			return true;
	return false;
}

struct moveOption
{
	int from, to, count;
	moveOption(const int from, const int to) {
		this -> from = from;
		this -> to = to;
		this -> count = Count();
	}
	inline int disFromStrategic() const {
		int dis = MAX_DISTANCE;
		for(auto s: MyStrategicNodes())
			if(head[s] == head[to])
				dis = min(dis, nodesDistance[s][to]);
		return dis;
	}

	int Count() const {
		if(score[from]) {
			int l = 0, r = numberOfTroops[from];
			while (l+1 < r)
			{
				int mid = (l+r)/2;
				if(Instability(from, totalNumberOfTroops[from]-mid) <= 0.2L) l = mid;
				else r = mid;
			}
			return l;
		}
		return numberOfTroops[from]-1;
	}

	int value() const {
		int val = 0;
		if(score[to] && Instability(to, totalNumberOfTroops[to]) > FIRST_DEFENSE_THRESHOLD)
			val += 10000000*(Instability(to, totalNumberOfTroops[to])-Instability(to, totalNumberOfTroops[to]+count));
		val += 10000*(2*count-5*disFromStrategic());
		val += 100*to;
		val += from;
		return val;
	}

	bool operator<(const moveOption &other) const {
		return value() > other.value();
	}
};


inline void MoveTroops(const moveOption &option) {
	game.moveTroops(option.from, option.to, option.count);
	UpdateData();
}

inline void DSU() {
	head.resize(numberOfNodes);
	for(int i = 0; i < numberOfNodes; i++) head[i] = i;
	for(auto i: MyNodes())
		for(auto u: graph[i]) if(owner[u] == myID)
			head[getHead(i)] = getHead(u);
	for(int i = 0; i < numberOfNodes; i++) head[i] = getHead(i);
}

inline void Move() {
	DSU();
	vector<moveOption> options;
	for(auto v: MyNodes()) if(!isInEdge(v))
		for(auto u: MyNodes()){
			moveOption newOption = moveOption(v, u);
			if(u != v && (score[u] > 0 || isInEdge(u)) && head[u] == head[v] && newOption.count > 0)
				options.push_back(newOption);
		}
	sort(all(options));
	if(!options.empty())
		MoveTroops(options[0]);
}

//////////////////////////////////////// Fort ////////////////////////////////////////

int TroopsToFort(int node){
  return 4 + 2 * score[node];
}
struct fortOption{
  int node;
  int neededTroops;

  fortOption(int _node){
    node = _node;
    neededTroops = max(0 , TroopsToFort(node) + 1 - numberOfTroops[node]);
  }
  bool operator<(fortOption &a){
    if(score[node] != score[a.node]) return (score[node] > score[a.node]);
    if(neededTroops != a.neededTroops) return (neededTroops < a.neededTroops);
    return (node < a.node);
  }
};
void FortPreparation(){
  if(fortTurn != -1) return;
  vector<fortOption> options;
  for(int strategicNode : MyStrategicNodes()){
    fortOption option(strategicNode);
    if(score[strategicNode] >= 3 and unusedTroops >= option.neededTroops){
      options.push_back(option);
    }
  }
  if(!options.empty()){
    sort(all(options));
    fortNode = options[0].node;
    fortTroops = TroopsToFort(fortNode);
    int neededTroops = options[0].neededTroops;

    numberOfTroops[fortNode] -= fortTroops;
    numberOfForts[fortNode] += 2 * fortTroops;
    totalNumberOfTroops[fortNode] += 2 * fortTroops;
    if(neededTroops > 0) PutTroop(fortNode , neededTroops);
  }
}
void Fort(){
  if(fortTurn != -1) return;
  vector<fortOption> options;
  for(int strategicNode : MyStrategicNodes()){
    fortOption option(strategicNode);
    if(score[strategicNode] >= 3 and option.neededTroops == 0){
      options.push_back(option);
    }
  }
  if(!options.empty()){
    sort(all(options));
    fortNode = options[0].node;
    fortTroops = TroopsToFort(fortNode);
    int neededTroops = options[0].neededTroops;

    game.fort(fortNode , fortTroops);

    fortNode = -1 , fortTroops = 0;
    fortTurn = turnNumber;
  }
}

//////////////////////////////////////// Ends ////////////////////////////////////////

inline void End() {
	
}

////////////////////////////////////////  Default Functions ////////////////////////////////////////

void init(){
	try{
		if(!isInitialized) Init();
		UpdateBasicData();
		UpdateData();
		ArrangeSoldiers();
	}
	catch(const exception& e){
		cerr << e.what() <<	 endl;
	}
	
}
void yourTurn(){
	try{
		if(!isInitialized) Init();
		UpdateBasicData();
		UpdateData();
		if(!FinishIt()){
			if((sz(MyStrategicNodes()) >= 4 && turnNumber >= 126) || turnNumber == 165)
				JustAttack();
			else {
				FortPreparation();
				DefenseStrategicNodes();
				UpdateData();
				if(!FastAttack()){
					vector<attack> attacks = AddTroopsToAttack();
					NextState();
					SlowBuildupAttack(attacks);
				}
			}
		}
		NextState();
		Move();
		NextState();
		Fort();
		game.nextState();
	}
	catch(const exception& e){
		cerr << e.what() << endl;
	}
}
void end(){
	try{
		End();
	}
	catch(const exception& e){
		cerr << e.what() << endl;
	}
}

// Thank God