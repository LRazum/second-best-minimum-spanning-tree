#include <iostream>
#include <vector>

using namespace std;

pair<int, int> combine(pair<int, int> a, pair<int, int> b)
{
    vector<int> v = {a.first, a.second, b.first, b.second};

    int topTwo = -3, topOne = -2;

    for(int c : v){
        if(c > topOne){
            topTwo = topOne;
            topOne = c;
        }
        else if(c > topTwo && c < topOne)
            topTwo = c;
    }

    return {topOne, topTwo};
}

const int N = 2e5 + 5;
const int l = 21;
vector<vector<pair<int, int>>> adj(N); // Lista koja pamti tko je s kim stvarno spojen u MST-u
vector<int> h(N, 0);                   // h = visina (dubina) svakog čvora
vector<vector<int>> up(N, vector<int>(l, -1));              // up[čvor][i] = tko je 2^i -ti predak
vector<vector<pair<int, int>>> dp(N, vector<pair<int, int>>(l)); // dp[čvor][i] = 2 najteže ceste do 2^i -tog pretka

void dfs(int u, int par, int d)
{
    h[u] = 1 + h[par];

    up[u][0] = par;

    dp[u][0] = {d, -1};

    for(auto v : adj[u]){
        if(v.first != par)
            dfs(v.first, u, v.second);
    }
}


pair<int, int> lca(int u, int v)
{
    pair<int, int> ans = {-2, -3};

    if(h[u] < h[v])
        swap(u,v);

    for(int i=l-1; i>=0; --i){

        if(h[u] - h[v] >= (1 << i)){
            ans = combine(ans, dp[u][i]);
            u = up[u][i];
        }
    }

    if(u == v)
        return ans;

    for(int i=l-1; i>=0; --i){

        if(up[u][i] != -1 && up[v][i] != -1 && up[u][i] != up[v][i]){
            ans = combine(ans, combine(dp[u][i], dp[v][i]));
            u = up[u][i];
            v = up[v][i];
        }
    }

    ans = combine(ans, combine(dp[u][0], dp[v][0]));

    return ans;
}