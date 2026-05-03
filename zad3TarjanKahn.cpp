#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <random>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <stack>
#include<cmath>

using namespace std;

struct krawedz {
	int u, v;
};

typedef vector<vector<int>> MacierzSasiedztwa;
typedef vector<vector<int>> ListaNastepnikow;
typedef vector<krawedz> ListaKrawedzi;

vector<int> odwiedzone; //0 to nieodwiedzony, 1 szary (w trakcie odwiedzania), 2 odwiedzony
vector<int> czas_wejscia;
vector<int> czas_wyjscia;
vector<int> stos_wynikowy; //posortowane wierzcholki
vector<int> sciezka; // sluzy do wypisania cyklu na ekran

bool cykl = false;
int licznik = 0;

//generatory

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

//tarjan

void tarjan_ln(const ListaNastepnikow& ln, int v) { //v to wierzcholek od ktorego zaczynam wypisywac
	if(cykl) return;

	odwiedzone[v] = 1;        //1 - wierzcholek jest odwiedzany
	czas_wejscia[v] = ++licznik;
	sciezka.push_back(v);

	for(int i=0; i<ln[v].size(); i++) {
		int sasiad = ln[v][i];

		if(odwiedzone[sasiad] == 1) { // cykl znaleziony bo to oznacza ze juz bylam na tym wierzcholu wczesniej
			cykl = true;
			cout << "BLAD: Cykl wykryty!" << endl;
			cout << "Wierzcholki tworzace cykl: ";
			bool w_cyklu = false;
			for(int i = 0; i < sciezka.size(); i++) {
				int w = sciezka[i];

				if(w == sasiad) {
					w_cyklu = true; //jezeli natrafilam na ten sam wierzcholek to znaczy ze jest zapetlenie
				}
				if(w_cyklu) cout << w << " ";
			}
			cout << endl;
			return;
		}
		if(odwiedzone[sasiad] == 0) { // nie odwiedzone wczesniej
			tarjan_ln(ln, sasiad);
			if(cykl) return; //zeby raz wypisal cykl i wyszedl
		}
	}

	odwiedzone[v] = 2;          // 2 to odwiedzony i bez cyklu, wszystkie krawedzie wychodzace zostaly sprawdzone
	czas_wyjscia[v] = ++licznik;  // Zapisujemy moment przetworzenia
	stos_wynikowy.push_back(v); //dodaje do wyniku
	sciezka.pop_back(); //usuwa ostatni wierzcholek
}
void tarjan_ms(const MacierzSasiedztwa& ms, int v) {
	if(cykl) return;

	odwiedzone[v] = 1;
	czas_wejscia[v] = ++licznik;
	sciezka.push_back(v);

	for(int i = 1; i < ms.size(); i++) {
		if(ms[v][i] == true) { //inaczej ms[v][i] == 1
			int sasiad = i;

			if(odwiedzone[sasiad] == 1) {
				cykl = true;
				cout << "BLAD: Cykl wykryty!" << endl;
				cout << "Wierzcholki tworzace cykl: ";
				bool w_cyklu = false;
				for(int j = 0; j < sciezka.size(); j++) {
					int w = sciezka[j];
					if(w == sasiad) w_cyklu = true;
					if(w_cyklu) cout << w << " ";
				}
				cout << endl;
				return;
			}
			if(odwiedzone[sasiad] == 0) {
				tarjan_ms(ms, sasiad);
				if(cykl) return;
			}
		}
	}

	odwiedzone[v] = 2;
	czas_wyjscia[v] = ++licznik;
	stos_wynikowy.push_back(v);
	sciezka.pop_back();
}
void tarjan_lk(const ListaKrawedzi& lk, int v) {
	if(cykl) return;

	odwiedzone[v] = 1;
	czas_wejscia[v] = ++licznik;
	sciezka.push_back(v);

	for(int i = 0; i < lk.size(); i++) {
		if(lk[i].u == v) {
			int sasiad = lk[i].v;

			if(odwiedzone[sasiad] == 1) {
				cykl = true;
				cout << "BLAD: Cykl wykryty!" << endl;
				cout << "Wierzcholki tworzace cykl: ";
				bool w_cyklu = false;
				for(int j = 0; j < sciezka.size(); j++) {
					int w = sciezka[j];
					if(w == sasiad) w_cyklu = true;
					if(w_cyklu) cout << w << " ";
				}
				cout << endl;
				return;
			}
			if(odwiedzone[sasiad] == 0) {
				tarjan_lk(lk, sasiad);
				if(cykl) return;
			}
		}
	}

	odwiedzone[v] = 2;
	czas_wyjscia[v] = ++licznik;
	stos_wynikowy.push_back(v);
	sciezka.pop_back();
}

//kahn

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
	} else if (demo) {
		cout << "Sortowanie: ";
		for (int v : result) cout << v << " ";
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
	} else if (demo) {
		cout << "Sortowanie: ";
		for (int v : result) cout << v << " ";
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

double uruchomTarjan(int rep, const MacierzSasiedztwa& ms, const ListaNastepnikow& ln, const ListaKrawedzi& lk, int n, int start,bool demo) {
	odwiedzone.assign(n + 1, 0);
	czas_wejscia.assign(n + 1, 0);
	czas_wyjscia.assign(n + 1, 0);
	stos_wynikowy.clear();
	sciezka.clear();
	cykl = false;
	licznik = 0;

	if (demo) {
        if (rep == 1) wypisz_ms(ms, n);
        else if (rep == 2) wypisz_ln(ln, n);
        else if (rep == 3) wypisz_lk(lk);
    }
	auto start_time = chrono::high_resolution_clock::now();

	if (rep == 1) {
		tarjan_ms(ms, start);
		for (int i = 1; i <= n; i++)
			if (odwiedzone[i] == 0) tarjan_ms(ms, i);
	} else if (rep == 2) {
		tarjan_ln(ln, start);
		for (int i = 1; i <= n; i++)
			if (odwiedzone[i] == 0) tarjan_ln(ln, i);
	} else if (rep == 3) {
		tarjan_lk(lk, start);
		for (int i = 1; i <= n; i++)
			if (odwiedzone[i] == 0) tarjan_lk(lk, i);
	}
	auto end_time = chrono::high_resolution_clock::now();
    double czas_trwania = chrono::duration<double, milli>(end_time - start_time).count();

	if(demo)
	{
		if (!cykl) {
			cout << "Sortowanie: ";
			for (int i = stos_wynikowy.size()-1; i >= 0; i--)
				cout << stos_wynikowy[i] << " ";
			cout << endl;

			cout << "Czasy wejscia/wyjscia:\n";
			for (int i = 1; i <= n; i++) {
				cout << "V" << i << ": "
					<< czas_wejscia[i] << "/"
					<< czas_wyjscia[i] << endl;
			}
		}
		cout << "Czas wykonania: " << czas_trwania << " ms\n";
	}
	return czas_trwania;
}

void uruchomKahn(int rep, const MacierzSasiedztwa& ms, const ListaNastepnikow& ln, const ListaKrawedzi& lk, int n) {
	auto start_time = chrono::high_resolution_clock::now();
	if (rep == 1) {
		wypisz_ms(ms, n);
		kahn_ms(ms, n, true);
	} else if (rep == 2) {
		wypisz_ln(ln, n);
		kahn_ln(ln, n, true);
	} else if (rep == 3) {
		wypisz_lk(lk);
		kahn_lk(lk, n, true);
	}
	auto end_time = chrono::high_resolution_clock::now();
	cout << "Czas wykonania: " << chrono::duration<double, milli>(end_time - start_time).count() << " ms\n";
}

bool wczytajZPliku(string nazwa, MacierzSasiedztwa& ms, ListaNastepnikow& ln, ListaKrawedzi& lk, int& n) {
	ifstream plik(nazwa);

	if (!plik) {
		cout << "BLAD: Nie mozna otworzyc pliku!\n";
		return false;
	}

	int m;
	plik >> n >> m;

	ms.assign(n + 1, vector<int>(n + 1, 0));
	ln.assign(n + 1, vector<int>());
	lk.clear();

	for (int i = 0; i < m; i++) {
		int u, v;
		plik >> u >> v;

		if (u < 1 || u > n || v < 1 || v > n) {
			cout << "Krawedz poza zakresem! Pomijam.\n";
			continue;
		}

		ms[u][v] = 1;
		ln[u].push_back(v);
		lk.push_back({u, v});
	}

	plik.close();
	return true;
}

void demo() {
	cout << "1 - losowy graf\n2 - wczytaj z pliku\n";
	int wybor;
	cin >> wybor;

	int n, m;
	MacierzSasiedztwa ms;
	ListaNastepnikow ln;
	ListaKrawedzi lk;

	if (wybor == 1) {
		do {
			cout << "liczba wierzcholkow n (1-12): ";
			cin >> n;
			if (n < 1 || n > 12) {
				cout << "Tryb demo tylko dla 1-12 wierzcholkow!\n";
			}
		} while (n < 1 || n > 12);

		int ile;
		ile = n*(n-1)/2;
		do {
			cout << "liczba krawedzi m (max " << ile << "): ";
			cin >> m;

			if (m < 1) {
				cout << "m musi byæ wieksze od 1" << endl;
			}

			if (m > ile) {
				cout << "Za duzo krawedzi!\n";
			}

		} while (m > ile);

		double s = (double)m / (n*(n-1)/2.0);

		generujDAG_ms(n, s, ms);
		generujDAG_ln(n, s, ln);
		generujDAG_lk(n, s, lk);
	} else if (wybor == 2) {
		string nazwa;
		cout << "Podaj nazwe pliku: ";
		cin >> nazwa;
		if (!wczytajZPliku(nazwa, ms, ln, lk, n)) {
			return;
		}
		wczytajZPliku(nazwa, ms, ln, lk, n);
	}

	int alg;
	do {
		cout << "Algorytm: 1-Kahn 2-Tarjan: ";
		cin >> alg;
	} while (alg != 1 && alg != 2);

	int rep;
	do {
		cout << "Reprezentacja: 1-MS 2-LN 3-LK: ";
		cin >> rep;
	} while (rep < 1 || rep > 3);

	if (alg == 1) {
		uruchomKahn(rep, ms, ln, lk, n);
	} else if (alg == 2) {
		int start;
		do {
			cout << "Podaj wierzcholek startowy (1-" << n << "): ";
			cin >> start;
			if (start < 1 || start > n) {
				cout << "Niepoprawny wierzcholek!\n";
			}
		} while (start < 1 || start > n);
		uruchomTarjan(rep, ms, ln, lk, n, start,true);
	}
}

/*void eksperyment() {
	int n = 1000;
	double s = 0.5;

	MacierzSasiedztwa ms;
	ListaNastepnikow ln;
	ListaKrawedzi lk;

	generujDAG_ms(n, s, ms);
	generujDAG_ln(n, s, ln);
	generujDAG_lk(n, s, lk);

	cout << "KAHN:\n";
	cout << kahn_ms(ms,n,false) << endl;
	cout << kahn_ln(ln,n,false) << endl;
	cout << kahn_lk(lk,n,false) << endl;

	cout << "TARJAN:\n";
	uruchomTarjan(1,ms,ln,lk,n,1);
	uruchomTarjan(2,ms,ln,lk,n,1);
	uruchomTarjan(3,ms,ln,lk,n,1);
}*/

pair<double, double> oblicz_statystyki(const vector<double>& czasy) {
    double suma = 0;
    for(double c : czasy) suma += c;
    double srednia = suma / czasy.size();

    double wariancja = 0;
    for(double c : czasy) wariancja += (c - srednia) * (c - srednia);
    double odchylenie = sqrt(wariancja / czasy.size());

    return {srednia, odchylenie};
}

void eksperyment() {

    int proby = 10;
    cout << ">>> EKSPERYMENT 1: Wzrost liczby wierzcholkow (n) <<<" << endl;
    
    vector<int> exp1_n = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000}; 
    vector<double> exp1_s = {0.1, 0.5, 0.9}; 

    ofstream plik1("eksperyment_1.csv");
    plik1 << "N;Gestosc;Kahn_MS_sr;Kahn_MS_odch;Kahn_LN_sr;Kahn_LN_odch;Kahn_LK_sr;Kahn_LK_odch;Tarjan_MS_sr;Tarjan_MS_odch;Tarjan_LN_sr;Tarjan_LN_odch;Tarjan_LK_sr;Tarjan_LK_odch\n";

    for (double s : exp1_s) {
        for (int n : exp1_n) {
            vector<double> c_k_ms(proby), c_k_ln(proby), c_k_lk(proby);
            vector<double> c_t_ms(proby), c_t_ln(proby), c_t_lk(proby);

            for (int p = 0; p < proby; p++) {
                MacierzSasiedztwa ms; ListaNastepnikow ln; ListaKrawedzi lk;
                generujDAG_ms(n, s, ms); generujDAG_ln(n, s, ln); generujDAG_lk(n, s, lk);

                c_k_ms[p] = kahn_ms(ms, n, false);
                c_k_ln[p] = kahn_ln(ln, n, false);
                c_k_lk[p] = kahn_lk(lk, n, false);

                c_t_ms[p] = uruchomTarjan(1, ms, ln, lk, n, 1, false);
                c_t_ln[p] = uruchomTarjan(2, ms, ln, lk, n, 1, false);
                c_t_lk[p] = uruchomTarjan(3, ms, ln, lk, n, 1, false);
            }

            auto stat_k_ms = oblicz_statystyki(c_k_ms); auto stat_k_ln = oblicz_statystyki(c_k_ln); auto stat_k_lk = oblicz_statystyki(c_k_lk);
            auto stat_t_ms = oblicz_statystyki(c_t_ms); auto stat_t_ln = oblicz_statystyki(c_t_ln); auto stat_t_lk = oblicz_statystyki(c_t_lk);

            plik1 << n << ";" << s << ";" 
                  << stat_k_ms.first << ";" << stat_k_ms.second << ";" << stat_k_ln.first << ";" << stat_k_ln.second << ";" << stat_k_lk.first << ";" << stat_k_lk.second << ";"
                  << stat_t_ms.first << ";" << stat_t_ms.second << ";" << stat_t_ln.first << ";" << stat_t_ln.second << ";" << stat_t_lk.first << ";" << stat_t_lk.second << "\n";
            
            cout << "Exp 1 - Wykonano: N=" << n << ", Gestosc=" << s * 100 << "%" << endl;
        }
    }
    plik1.close();

    cout << "\n>>> EKSPERYMENT 2: Wzrost nasycenia (s) <<<" << endl;
    
    int exp2_n = 500; 
    vector<double> exp2_s = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};

    ofstream plik2("eksperyment_2.csv");
    plik2 << "N;Gestosc;Kahn_MS_sr;Kahn_MS_odch;Kahn_LN_sr;Kahn_LN_odch;Kahn_LK_sr;Kahn_LK_odch;Tarjan_MS_sr;Tarjan_MS_odch;Tarjan_LN_sr;Tarjan_LN_odch;Tarjan_LK_sr;Tarjan_LK_odch\n";

    for (double s : exp2_s) {
        vector<double> c_k_ms(proby), c_k_ln(proby), c_k_lk(proby);
        vector<double> c_t_ms(proby), c_t_ln(proby), c_t_lk(proby);

        for (int p = 0; p < proby; p++) {
            MacierzSasiedztwa ms; ListaNastepnikow ln; ListaKrawedzi lk;
            generujDAG_ms(exp2_n, s, ms); generujDAG_ln(exp2_n, s, ln); generujDAG_lk(exp2_n, s, lk);

            c_k_ms[p] = kahn_ms(ms, exp2_n, false);
            c_k_ln[p] = kahn_ln(ln, exp2_n, false);
            c_k_lk[p] = kahn_lk(lk, exp2_n, false);

            c_t_ms[p] = uruchomTarjan(1, ms, ln, lk, exp2_n, 1, false);
            c_t_ln[p] = uruchomTarjan(2, ms, ln, lk, exp2_n, 1, false);
            c_t_lk[p] = uruchomTarjan(3, ms, ln, lk, exp2_n, 1, false);
        }

        auto stat_k_ms = oblicz_statystyki(c_k_ms); auto stat_k_ln = oblicz_statystyki(c_k_ln); auto stat_k_lk = oblicz_statystyki(c_k_lk);
        auto stat_t_ms = oblicz_statystyki(c_t_ms); auto stat_t_ln = oblicz_statystyki(c_t_ln); auto stat_t_lk = oblicz_statystyki(c_t_lk);

        plik2 << exp2_n << ";" << s << ";" 
              << stat_k_ms.first << ";" << stat_k_ms.second << ";" << stat_k_ln.first << ";" << stat_k_ln.second << ";" << stat_k_lk.first << ";" << stat_k_lk.second << ";"
              << stat_t_ms.first << ";" << stat_t_ms.second << ";" << stat_t_ln.first << ";" << stat_t_ln.second << ";" << stat_t_lk.first << ";" << stat_t_lk.second << "\n";
        
        cout << "Exp 2 - Wykonano: N=" << exp2_n << ", Gestosc=" << s * 100 << "%" << endl;
    }
    plik2.close();

    cout << "\n--- ZAKONCZONO! ---" << endl;
    cout << "Wyniki sa w plikach 'eksperyment_1.csv' oraz 'eksperyment_2.csv'." << endl;
}

int main() {
	system("chcp 1250");
	while(true) {
		cout << "\n1. Demo\n2. Eksperyment\n0. Wyjscie\n";
		int x;
		cin >> x;

		if (x == 0) break;
		else if (x == 1) demo();
		else if (x == 2) eksperyment();
		else cout << "Niepoprawny wybór. Spróbuj jeszcze raz." << endl;
	}
	return 0;
}
