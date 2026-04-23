#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// Struktura za brid
struct Edge
{
    int u, v;
    long long w;
    int id; // Originalni redni broj za ispis
    
    // Sortiramo po težini
    bool operator<(const Edge& other) const {
        return w < other.w;
    }
};

const int N = 200005;
const int LOG = 21; // Dovoljno za 200,000 čvorova

// DSU strukture
int parent_dsu[N], sz[N];

// Strukture za stablo i Binary Lifting
vector<pair<int, long long>> adj[N];
int up[N][LOG];             // up[u][i] = 2^i-ti predak čvora u
long long mx_edge[N][LOG];  // mx_edge[u][i] = najteži brid do 2^i-tog pretka
int h[N];                   // dubina čvora
bool in_mst[N];             // označava je li brid dio glavnog MST-a
long long ans[N];           // sprema konačna rješenja za svaki brid

// DSU - pronalazak predstavnika
int find_set(int v)
{
    if (v == parent_dsu[v]) return v;
    return parent_dsu[v] = find_set(parent_dsu[v]);
}

// DSU - spajanje skupova
bool unite_set(int a, int b)
{
    a = find_set(a);
    b = find_set(b);
    if (a != b) {
        if (sz[a] < sz[b]) swap(a, b);
        parent_dsu[b] = a;
        sz[a] += sz[b];
        return true;
    }
    return false;
}

// DFS za postavljanje dubine i prvih predaka
void dfs(int u, int p, long long edge_w)
{
    up[u][0] = p;
    mx_edge[u][0] = edge_w;
    
    for(pair<int, long long> edge : adj[u]){
        int v = edge.first;
        long long w = edge.second;
        
        if(v != p){
            h[v] = h[u] + 1;
            dfs(v, u, w);
        }
    }
}

// LCA + traženje najtežeg brida na putu od u do v
long long get_max_edge(int u, int v)
{
    long long res = 0;
    
    // Želimo da u bude dublji
    if(h[u] < h[v])
        swap(u, v);
    
    // 1. Izjednačavanje dubina
    for(int i = LOG-1; i >= 0; --i){
        int skok = 1 << i; // 1 << i je isto što i 2 na potenciju i
        if(h[u] - skok >= h[v]){
            res = max(res, mx_edge[u][i]);
            u = up[u][i];
        }
    }
    
    if(u == v) 
        return res;
    
    // 2. Skakanje paralelno prema gore
    for (int i = LOG - 1; i >= 0; i--) {
        if (up[u][i] != -1 && up[u][i] != up[v][i]) {
            res = max(res, mx_edge[u][i]);
            res = max(res, mx_edge[v][i]);
            u = up[u][i];
            v = up[v][i];
        }
    }
    
    // 3. Zadnji korak do samog zajedničkog pretka
    res = max(res, mx_edge[u][0]);
    res = max(res, mx_edge[v][0]);
    
    return res;
}

int main(void){
    int n, m;
    cin >> n >> m;

    vector<Edge> edges(m);

    for(int i=0; i<m; ++i){
        cin >> edges[i].u >> edges[i].v >> edges[i].w;
        edges[i].id = i;
        in_mst[i] = false;
    }

    // Sortiramo bridove po težini kako bismo napravili MST
    vector<Edge> sorted_edges = edges;
    sort(sorted_edges.begin(), sorted_edges.end());

    // Inicijalizacija DSU i Binary Lifting tablice na -1
    for(int i=1; i<=n; ++i){
        parent_dsu[i] = i;
        sz[i] = 1;
        for(int j=0; j<LOG; ++j){
            up[i][j] = -1;
            mx_edge[i][j] = 0;
        }
    }

    long long mst_weight = 0;

    // 1. KORAK: Gradimo glavno Minimalno razapinjuće stablo
    for(Edge e : sorted_edges){
        if(unite_set(e.u, e.v)){
            in_mst[e.id] = true;
            mst_weight += e.w;

            // Gradimo pravo stablo za DFS
            adj[e.u].push_back({e.v, e.w});
            adj[e.v].push_back({e.u, e.w});
        }
    }

    // 2. KORAK: Priprema za Binary Lifting
    h[1] = 0;
    dfs(1, -1, 0); // Korijen je 1, nema roditelja (-1), težina do roditelja je 0

    // Popunjavanje tablice za veće skokove
    for(int i=1; i<LOG; ++i){
        for(int j = 1; j <= n; ++j){
            if(up[j][i - 1] != -1){
                int half_way_parent = up[j][i - 1];
                up[j][i] = up[half_way_parent][i - 1];
                mx_edge[j][i] = max(mx_edge[j][i - 1], mx_edge[half_way_parent][i - 1]);
            }
        }
    }

    // 3. KORAK: Računanje rješenja za svaki pojedini brid
    for(Edge e : edges){
        if(in_mst[e.id]){
            // Ako je brid već u MST-u, rješenje je jednostavno ukupna težina MST-a
            ans[e.id] = mst_weight;
        }
        else{
            // Ako nije, nađemo najteži brid na originalnom putu u MST-u
            long long max_w_on_path = get_max_edge(e.u, e.v);
            
            // Nova težina je = Stari MST + dodajemo novi brid - mičemo najteži s puta
            ans[e.id] = mst_weight + e.w - max_w_on_path;
        }
    }

    // 4. KORAK: Ispis rješenja
    for(int i=0; i<m; ++i){
        cout << ans[i] << "\n";
    }

    return 0;
}