#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
#define pll pair<ll, ll>
#define loop(i, n) for (ll i = 0; i < n; i++)
#define FOR(i, n, m) for (ll i = n; i <= m; i++)
#define isIn(vec, item) find(vec.begin(), vec.end(), item) != vec.end()
#define fs first
#define sc second
#define pb push_back
#define mp make_pair
#define all(v) v.begin(), v.end()
#define inf 1000000000
#define mod 1000000007

void solve()
{
    string line;
    vector<char> prev, now;
    while (getline(cin, line))
    {   
        int i = 0;
        for(auto c : line){
            if((int)c < 44 or 122 <  (int) c){
                c = prev[i];
            }
            i++;
            now.pb(c);
            cout << c;
        }

        prev = now;
        now.clear();
        cout << endl;
    }
}

int main()
{

    ios::sync_with_stdio(0);
    cin.tie(0);

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    solve();
}