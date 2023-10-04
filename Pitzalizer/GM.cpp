// In the name of Allah
#include <bits/stdc++.h>
using namespace std;

int main() {
    int n = 42, m = n * 2;
    cout << n << ' ' << m << '\n';
    for(int i = 0; i < n; i++)
    {
        cout << i << ' ' << (i+1)%n << '\n';
        cout << i << ' ' << (i+2)%n << '\n';
    }
    cout << "0 7 14 21 28 35\n";
    cout << "2 4 2 4 2 4\n";
    
}
