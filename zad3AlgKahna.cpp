#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <random>
#include <fstream>

using namespace std;

struct krawedz {
	int u, v;
};

typedef vector<vector<int>> MacierzSasiedztwa;
typedef vector<vector<int>> ListaNastepnikow;
typedef vector<krawedz> ListaKrawedzi;

void wypiszStopnie(const vector<int>& deg, int n) {
	cout << "   Stopnie: ";
	for (int i = 1; i <= n; i++) cout << "V" << i << ":" << deg[i] << " ";
	cout << endl;
}

mt19937 rng(random_device {}());

void generujDAG_ms(int n, double s, MacierzSasiedztwa& ms) {
	ms.assign(n + 1, vector<int>(n + 1, 0));

	vector<pair<int,int>> edges;
	for (int i = 1; i <= n; i++)
		for (int j = i + 1; j <= n; j++)
			edges.push_back({i, j});

	shuffle(edges.begin(), edges.end(), rng);

	int max_edges = n * (n - 1) / 2;
	int target = max_edges * s;

	for (int i = 0; i < target; i++)
		ms[edges[i].first][edges[i].second] = 1;
}

void generujDAG_ln(int n, double s, ListaNastepnikow& ln) {
	ln.assign(n + 1, vector<int>());

	vector<pair<int,int>> edges;
	for (int i = 1; i <= n; i++)
		for (int j = i + 1; j <= n; j++)
			edges.push_back({i, j});

	shuffle(edges.begin(), edges.end(), rng);

	int max_edges = n * (n - 1) / 2;
	int target = max_edges * s;

	for (int i = 0; i < target; i++)
		ln[edges[i].first].push_back(edges[i].second);
}

void generujDAG_lk(int n, double s, ListaKrawedzi& lk) {
	lk.clear();

	vector<pair<int,int>> edges;
	for (int i = 1; i <= n; i++)
		for (int j = i + 1; j <= n; j++)
			edges.push_back({i, j});

	shuffle(edges.begin(), edges.end(), rng);

	int max_edges = n * (n - 1) / 2;
	int target = max_edges * s;

	for (int i = 0; i < target; i++)
		lk.push_back({edges[i].first, edges[i].second});
}

double kahn_ms(const MacierzSasiedztwa& ms, int n, bool demo) {
	auto start = chrono::high_resolution_clock::now();

	vector<int> in_degree(n + 1, 0);

	for (int u = 1; u <= n; u++)
		for (int v = 1; v <= n; v++)
			if (ms[u][v]) in_degree[v]++;

	queue<int> Q;
	for (int i = 1; i <= n; i++)
		if (in_degree[i] == 0) Q.push(i);

	vector<int> result;

	while (!Q.empty()) {
		int u = Q.front();
		Q.pop();
		result.push_back(u);

		for (int v = 1; v <= n; v++) {
			if (ms[u][v]) {
				in_degree[v]--;
				if (in_degree[v] == 0)
					Q.push(v);
			}
		}

		if (demo) {
			cout << "Usunieto: " << u << endl;
			wypiszStopnie(in_degree, n);
		}
	}

	if (result.size() < n) {
		cout << "CYKL! Wierzcholki w cyklu: ";
		for (int i = 1; i <= n; i++)
			if (in_degree[i] > 0) cout << i << " ";
		cout << endl;
	} else if (demo) {
		cout << "Sortowanie: ";
		for (int v : result) cout << v << " ";
		cout << endl;
	}

	auto end = chrono::high_resolution_clock::now();
	return chrono::duration<double, milli>(end - start).count();
}

double kahn_ln(const ListaNastepnikow& ln, int n, bool demo) {
	auto start = chrono::high_resolution_clock::now();

	vector<int> in_degree(n + 1, 0);
	for (int u = 1; u <= n; u++)
		for (int v : ln[u]) in_degree[v]++;

	queue<int> Q;
	for (int i = 1; i <= n; i++) if (in_degree[i] == 0) Q.push(i);

	vector<int> result;
	while (!Q.empty()) {
		int u = Q.front();
		Q.pop();
		result.push_back(u);
		for (int v : ln[u]) {
			in_degree[v]--;
			if (in_degree[v] == 0) Q.push(v);
		}
		if (demo) {
			cout << "Usunieto " << u;
			wypiszStopnie(in_degree, n);
		}
	}

	if (result.size() < n) {
		cout << "CYKL! Wierzcholki w cyklu: ";
		for (int i = 1; i <= n; i++)
			if (in_degree[i] > 0) cout << i << " ";
		cout << endl;
	}

	auto end = chrono::high_resolution_clock::now();
	return chrono::duration<double, milli>(end - start).count();
}

double kahn_lk(const ListaKrawedzi& lk, int n, bool demo) {
	auto start = chrono::high_resolution_clock::now();

	vector<int> in_degree(n + 1, 0);
	for (const auto& e : lk) in_degree[e.v]++;

	queue<int> Q;
	for (int i = 1; i <= n; i++) if (in_degree[i] == 0) Q.push(i);

	vector<int> result;
	while (!Q.empty()) {
		int u = Q.front();
		Q.pop();
		result.push_back(u);
		for (const auto& e : lk) {
			if (e.u == u) {
				in_degree[e.v]--;
				if (in_degree[e.v] == 0) Q.push(e.v);
			}
		}
		if (demo) {
			cout << "Usunieto " << u;
			wypiszStopnie(in_degree, n);
		}
	}
	if (result.size() < n) {
		cout << "CYKL! Wierzcholki w cyklu: ";
		for (int i = 1; i <= n; i++)
			if (in_degree[i] > 0) cout << i << " ";
		cout << endl;
	}
	auto end = chrono::high_resolution_clock::now();
	return chrono::duration<double, milli>(end - start).count();
}

void wypisz_ms(const MacierzSasiedztwa& ms, int n) {
	cout << "Macierz sasiedztwa:\n";
	for (int i = 1; i <= n; i++) {
		for (int j = 1; j <= n; j++)
			cout << ms[i][j] << " ";
		cout << endl;
	}
}

void wypisz_ln(const ListaNastepnikow& ln, int n) {
	cout << "Lista nastepnikow:\n";
	for (int i = 1; i <= n; i++) {
		cout << i << ": ";
		for (int v : ln[i]) cout << v << " ";
		cout << endl;
	}
}

void wypisz_lk(const ListaKrawedzi& lk) {
	cout << "Lista krawedzi:\n";
	for (auto e : lk)
		cout << e.u << " -> " << e.v << endl;
}

void wczytaj_ms(const string& nazwa, MacierzSasiedztwa& ms, int& n) {
	ifstream plik(nazwa);
	int m;
	plik >> n >> m;

	ms.assign(n + 1, vector<int>(n + 1, 0));

	for (int i = 0; i < m; i++) {
		int u, v;
		plik >> u >> v;
		ms[u][v] = 1;
	}

	plik.close();
}

void wczytaj_ln(const string& nazwa, ListaNastepnikow& ln, int& n) {
	ifstream plik(nazwa);
	int m;
	plik >> n >> m;

	ln.assign(n + 1, vector<int>());

	for (int i = 0; i < m; i++) {
		int u, v;
		plik >> u >> v;
		ln[u].push_back(v);
	}

	plik.close();
}

void wczytaj_lk(const string& nazwa, ListaKrawedzi& lk, int& n) {
	ifstream plik(nazwa);
	int m;
	plik >> n >> m;

	lk.clear();

	for (int i = 0; i < m; i++) {
		int u, v;
		plik >> u >> v;
		lk.push_back({u, v});
	}

	plik.close();
}

void uruchomDemo() {
	int tryb_wejscia;
	cout << "1 - losowy graf\n2 - wczytaj z pliku\n";
	cin >> tryb_wejscia;
	if (tryb_wejscia == 1) {
		int n, m;
		cout << "Podaj n (<=12): ";
		cin >> n;
		cout << "Podaj liczbe krawedzi: ";
		cin >> m;

		while (true) {
			int rep;
			cout << "\nReprezentacja: 1-MS 2-LN 3-LK 0-powrot: ";
			cin >> rep;

			if (rep == 0) break;

			double s = (double)m / (n*(n-1)/2.0);

			if (rep == 1) {
				MacierzSasiedztwa ms;
				generujDAG_ms(n, s, ms);
				wypisz_ms(ms, n);
				kahn_ms(ms, n, true);
			} else if (rep == 2) {
				ListaNastepnikow ln;
				generujDAG_ln(n, s, ln);
				wypisz_ln(ln, n);
				kahn_ln(ln, n, true);
			} else {
				ListaKrawedzi lk;
				generujDAG_lk(n, s, lk);
				wypisz_lk(lk);
				kahn_lk(lk, n, true);
			}
		}
	} else {
		string nazwa;
		cout << "Podaj nazwe pliku: ";
		cin >> nazwa;

		ifstream test(nazwa);
		if (!test) {
			cout << "Nie mozna otworzyc pliku!\n";
			return;
		}
		test.close();

		int rep;
		cout << "Reprezentacja: 1-MS 2-LN 3-LK: ";
		cin >> rep;

		if (rep == 1) {
			MacierzSasiedztwa ms;
			int n;
			wczytaj_ms(nazwa, ms, n);
			wypisz_ms(ms, n);
			kahn_ms(ms, n, true);
		} else if (rep == 2) {
			ListaNastepnikow ln;
			int n;
			wczytaj_ln(nazwa, ln, n);
			wypisz_ln(ln, n);
			kahn_ln(ln, n, true);
		} else {
			ListaKrawedzi lk;
			int n;
			wczytaj_lk(nazwa, lk, n);
			wypisz_lk(lk);
			kahn_lk(lk, n, true);
		}
	}
}


void uruchomEksperyment() {
    int n_start, n_step, n_count;
    double nasycenie;

    cout << "Poczatek n: ";
    cin >> n_start;
    cout << "Krok n: ";
    cin >> n_step;
    cout << "Liczba krokow: ";
    cin >> n_count;
    cout << "Nasycenie (0.1 - 0.9): ";
    cin >> nasycenie;

    cout << "\nNasycenie: " << nasycenie << endl;
    cout << "\nN | MS [ms] | LN [ms] | LK [ms]" << endl;
    cout << "-----------------------------------" << endl;

    for (int i = 0; i < n_count; i++) {
        int n = n_start + i * n_step;

        double t_ms = 0, t_ln = 0, t_lk = 0;

        for (int j = 0; j < 10; j++) {

            ListaNastepnikow ln;
            generujDAG_ln(n, nasycenie, ln);

            MacierzSasiedztwa ms(n+1, vector<int>(n+1,0));
            for(int u=1; u<=n; u++)
                for(int v: ln[u])
                    ms[u][v] = 1;

            ListaKrawedzi lk;
            for(int u=1; u<=n; u++)
                for(int v: ln[u])
                    lk.push_back({u,v});

            t_ms += kahn_ms(ms, n, false);
            t_ln += kahn_ln(ln, n, false);
            t_lk += kahn_lk(lk, n, false);
        }

        cout << n << " | "
             << fixed << setprecision(4)
             << t_ms/10.0 << " | "
             << t_ln/10.0 << " | "
             << t_lk/10.0 << endl;
    }
}

int main() {
	while (true) {
		int tryb;
		cout << "\n1. Tryb demonstracyjny\n2. Tryb eksperymentalny\n0. Wyjscie\nWybor: ";
		cin >> tryb;

		if (tryb == 0) break;
		else if (tryb == 1) uruchomDemo();
		else if (tryb == 2) uruchomEksperyment();
	}

	return 0;
}
