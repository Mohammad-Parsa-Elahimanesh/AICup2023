// In the name of Allah
#include <bits/stdc++.h>
using namespace std;

int main() {
    cout << "{\n    \"number_of_nodes\": "; 
    int n, m;
    cin >> n >> m;
    cout << n << ",\n    \"number_of_edges\": " << m << ",\n    \"list_of_edges\": [";
    for(int u, v, i = 0; i < m; i++)
        cin >> u >> v, cout << "["<< u << ", " << v << "]" << (i+1 == m?"":", ");
    cout << "],\n    \"strategic_nodes\": [";
    for(int node, i = 0; i < 6; i++)
        cin >> node, cout << node << (i+1 == 6?"":",");
    cout << "],\n    \"scores_of_strategic_nodes\": [";
    for(int score, i = 0; i < 6; i++)
        cin >> score, cout << score << (i+1 == 6?"":",");
    cout << "]\n}\n";
}