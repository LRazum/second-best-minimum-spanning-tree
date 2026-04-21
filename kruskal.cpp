#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct edge
{
    int s, e, w; // s = početak, e = kraj, w = težina
    
    // Preopterećenje operatora '<' kako bi funkcija 'sort' znala sortirati bridove po težini
    bool operator<(const struct edge& other) const { return w < other.w; }
};

const int N = 2e5 + 5;
long long res = 0; // Varijabla za čuvanje ukupne težine MST-a
int n, m;
vector<edge> edges; // Vektor za pohranu svih bridova

// Globalne strukture podataka za DSU (Disjoint Set Union)
vector<int> parent(N, -1), sz(N, 0);

// DSU - Pronalazi glavnog predstavnika skupa u kojem se nalazi 'v'
int find_set(int v)
{
    if(parent[v] == v)
        return v;

    return parent[v] = find_set(parent[v]);
}

// DSU - Spaja dva skupa (odnosno dva čvora). Vraća 'true' ako su uspješno spojeni
bool unite_set(int a, int b)
{
   a = find_set(a);
   b = find_set(b);

   if(a != b){
        if(sz[a] < sz[b])
            swap(a,b);
        parent[b] = a;
        sz[a] += sz[b];
        return true;
   }
   return false;
}

int main(void){
   
    // Učitavanje broja čvorova (n) i broja bridova (m)
    cin >> n >> m;
    
    // Inicijalizacija DSU strukture
    for(int i = 1; i <= n; i++) {
        parent[i] = i;
        sz[i] = 1;     // Veličina svakog skupa je u početku 1
    }
    
    // Učitavanje bridova
    for(int i = 0; i < m; i++) {
        int u, v, w;
        cin >> u >> v >> w; 
        edges.push_back({u, v, w});
    }
    
    // 1. Korak Kruskalovog algoritma
    sort(edges.begin(), edges.end());
    
    int edges_used = 0;

    for(edge e : edges){

        if(unite_set(e.s, e.e)){
            res += e.w;
            ++edges_used;

            if(edges_used == n-1)
                break;
        }
    }

    // Ispisujemo ukupnu najmanju težinu stabla
    cout << res << "\n";
    
    return 0;
}