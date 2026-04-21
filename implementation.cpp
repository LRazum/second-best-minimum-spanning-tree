#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// Struktura koja predstavlja jedan brid u grafu
struct edge {
    int s, e, w, id; // s = start (početak), e = end (kraj), w = weight (težina), id = originalni redni broj
    
    // Preopterećenje operatora '<' kako bi funkcija 'sort' znala sortirati bridove po težini (od najmanje do najveće)
    bool operator<(const struct edge& other) const { return w < other.w; }
};
// 'typedef' nam omogućuje da umjesto 'struct edge' pišemo samo 'Edge'
typedef struct edge Edge;

const int N = 2e5 + 5; // Maksimalan broj čvorova (200,000 + 5 za sigurnost)
long long res = 0, ans = 1e18; // res = težina 1. najboljeg stabla, ans = težina 2. najboljeg stabla (početno jako veliki broj)
int n, m, a, b, w, id, l = 21; // l = 21 je za Binary Lifting (2^20 je dovoljno za pokriti N)
vector<Edge> edges; // Ovdje spremamo sve bridove

// Globalne strukture podataka:
// h = visina/dubina čvora u stablu, parent = roditelj za DSU, sz = veličina skupa za DSU, present = je li brid u 1. MST-u
vector<int> h(N, 0), parent(N, -1), sz(N, 0), present(N, 0);

// adj = lista susjedstva grafa (stvarnog stabla) gdje par sadrži {susjed, težina_brida}
vector<vector<pair<int, int>>> adj(N);

// dp[u][i] = pamti DVIJE najveće težine bridova na putu od čvora 'u' do njegovog 2^i -tog pretka
vector<vector<pair<int, int>>> dp(N, vector<pair<int, int>>(l));

// up[u][i] = pamti tko je točno 2^i -ti predak čvora 'u'
vector<vector<int>> up(N, vector<int>(l, -1));

// DSU (Disjoint Set Union) - Pronalazi glavnog predstavnika skupa u kojem se nalazi 'v'
int find_set(int v) {
    if (v == parent[v]) // Ako je čvor sam svoj roditelj, on je glavni predstavnik
        return v;
    // "Path compression": usput spajamo čvor direktno na glavnog predstavnika kako bi iduće traženje bilo brže
    return parent[v] = find_set(parent[v]);
}

// DSU - Spaja dva skupa (odnosno dva čvora). Vraća 'true' ako su uspješno spojeni (nisu bili u istom)
bool unite_set(int a, int b) {
    a = find_set(a); // Nađi predstavnika od 'a'
    b = find_set(b); // Nađi predstavnika od 'b'
    if (a != b) { // Ako nisu u istom skupu (nema ciklusa)
        // Spajamo manji skup na veći ("Union by size") kako bi drvo ostalo plitko
        if (sz[a] < sz[b])
            swap(a, b);
        parent[b] = a;
        sz[a] += sz[b];
        return true; // Uspješno spojeno
    }
    return false; // Već su u istom skupu, dodavanje brida stvorilo bi ciklus
}

// Funkcija koja od 4 broja pronalazi 2 NAJVEĆA, ali STROGO RAZLIČITA broja
pair<int, int> combine(pair<int, int> a, pair<int, int> b) {
    vector<int> v = {a.first, a.second, b.first, b.second};
    int topTwo = -3, topOne = -2; // Inicijalne vrijednosti moraju biti jako male
    for (int c : v) {
        if (c > topOne) { // Ako smo našli novi apsolutno najveći
            topTwo = topOne; // Stari najveći postaje drugi najveći
            topOne = c;      // Novi broj postaje najveći
        } else if (c > topTwo && c < topOne) { 
            // Ako je broj manji od najvećeg, ali veći od drugog najvećeg
            topTwo = c;
        }
    }
    return {topOne, topTwo}; // Vraćamo dva najteža brida
}

// Dubinsko pretraživanje (DFS) za pripremu stabla za Binary Lifting
void dfs(int u, int par, int d) {
    h[u] = 1 + h[par]; // Dubina trenutnog čvora je dubina roditelja + 1
    up[u][0] = par;    // Prvi predak (2^0 = 1) je direktni roditelj
    dp[u][0] = {d, -1}; // Na putu do prvog pretka postoji samo jedan brid (težine 'd'), drugog nema (-1)
    
    // Prođi kroz sve susjede
    for (auto v : adj[u]) {
        if (v.first != par) { // Nemoj se vraćati natrag roditelju
            dfs(v.first, u, v.second); // Nastavi pretraživanje
        }
    }
}

// Lowest Common Ancestor (Najniži zajednički predak) - nalazi put između 'u' i 'v' u stablu
pair<int, int> lca(int u, int v) {
    pair<int, int> ans = {-2, -3}; // Tu ćemo skupljati 2 najteža brida
    
    // Želimo da 'u' bude niže u stablu (da ima veću dubinu)
    if (h[u] < h[v]) {
        swap(u, v);
    }
    
    // Prvo izjednačavamo visine čvorova 'u' i 'v'
    for (int i = l - 1; i >= 0; i--) {
        if (h[u] - h[v] >= (1 << i)) { // Ako je razlika u visinama barem 2^i
            ans = combine(ans, dp[u][i]); // Zabilježi teške bridove s ovog skoka
            u = up[u][i]; // Skokni gore
        }
    }
    
    // Ako smo nakon izjednačavanja visina došli do istog čvora, to je bio naš odgovor
    if (u == v) {
        return ans;
    }
    
    // Sada oba čvora skaču gore skupa dok ne dođu tik ispod zajedničkog pretka
    for (int i = l - 1; i >= 0; i--) {
        if (up[u][i] != -1 && up[v][i] != -1 && up[u][i] != up[v][i]) {
            ans = combine(ans, combine(dp[u][i], dp[v][i])); // Bilježi bridove s obje strane
            u = up[u][i]; // Skokni gore s 'u'
            v = up[v][i]; // Skokni gore s 'v'
        }
    }
    
    // Na kraju moramo uračunati još taj zadnji skok od 1 koraka do samog zajedničkog pretka
    ans = combine(ans, combine(dp[u][0], dp[v][0]));
    return ans; // Vraća 2 najteža, strogo različita brida na cijelom putu između originalnih 'u' i 'v'
}

int main(void) {
    cin >> n >> m; // n = broj čvorova, m = broj bridova
    
    // Inicijalizacija DSU strukture
    for (int i = 1; i <= n; i++) {
        parent[i] = i; // U početku je svaki čvor sam svoj roditelj
        sz[i] = 1;     // Veličina svakog skupa je 1
    }
    
    // Učitavanje bridova
    for (int i = 1; i <= m; i++) {
        cin >> a >> b >> w; 
        edges.push_back({a, b, w, i - 1});
    }
    
    // 1. KRUSKALOV ALGORITAM - Sortiramo bridove po težini
    sort(edges.begin(), edges.end());
    
    // Gradimo 1. najbolje minimalno razapinjuće stablo (MST)
    for (int i = 0; i <= m - 1; i++) {
        a = edges[i].s;
        b = edges[i].e;
        w = edges[i].w;
        id = edges[i].id;
        
        // Pokušavamo spojiti 'a' i 'b'
        if (unite_set(a, b)) {
            adj[a].emplace_back(b, w); // Gradimo stablo za DFS
            adj[b].emplace_back(a, w);
            present[id] = 1; // Označavamo da je ovaj brid ušao u glavni MST
            res += w; // Zbrajamo težinu MST-a
        }
    }
    
    // 2. PRIPREMA ZA UPITE
    dfs(1, 0, 0); // Pokrećemo DFS iz čvora 1 da postavimo direktne roditelje
    
    // Gradimo "Binary Lifting" tablicu
    for (int i = 1; i <= l - 1; i++) { // Za svaki potencijalni skok (2^1, 2^2, 2^3...)
        for (int j = 1; j <= n; ++j) { // Za svaki čvor u grafu
            if (up[j][i - 1] != -1) { // Ako postoji predak na pola puta
                int v = up[j][i - 1]; // Taj predak na pola puta (skok 2^(i-1))
                up[j][i] = up[v][i - 1]; // Predak za puni skok 2^i je predak mog pretka
                // Najteži bridovi za puni skok su kombinacija najtežih bridova iz prve i druge polovice skoka
                dp[j][i] = combine(dp[j][i - 1], dp[v][i - 1]); 
            }
        }
    }
    
    // 3. TRAŽENJE DRUGOG NAJBOLJEG MST-a
    // Pokušavamo na silu uvesti jedan po jedan brid koji NIJE u glavnom MST-u
    for (int i = 0; i <= m - 1; i++) {
        id = edges[i].id;
        w = edges[i].w;
        
        if (!present[id]) { // Ako brid NIJE u glavnom stablu
            // Nađimo 2 najteža brida na trenutnom putu u stablu između čvorova s i e
            auto rem = lca(edges[i].s, edges[i].e);
            
            // Želimo izbaciti najteži brid (rem.first) i ubaciti novi brid (w).
            // Uvjet: Težina mora biti STROGO veća. Znači, ako je novi brid jednak najtežem,
            // ne možemo izbaciti najteži jer nećemo dobiti STROGO veće stablo (dobit ćemo isto).
            if (rem.first != w) {
                // Ako možemo, provjeravamo jel ovo rješenje bolje od dosad nađenog "drugog najboljeg"
                if (ans > res + w - rem.first) {
                    ans = res + w - rem.first;
                }
            } 
            // Ako je najteži brid baš jednak novom bridu, onda moramo izbaciti onaj DRUGI najteži
            else if (rem.second != -1) {
                if (ans > res + w - rem.second) {
                    ans = res + w - rem.second;
                }
            }
        }
    }
    
    cout << ans << "\n";
    return 0;
}