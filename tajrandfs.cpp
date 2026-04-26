#include<iostream>
#include<vector>
#include<cstdlib>
#include<ctime>
#include<stack>
#include<chrono>
#include<fstream>
using namespace std;

int n,m;
vector<vector<int>> ln; //lista nastepnikow
vector<vector<bool>> ms; //macierz sasiedztwa
vector<pair<int,int>> lk; //lista krawedzi

vector<int> odwiedzone;       // 0 = bialy, 1 = szary (w trakcie), 2 = czarny (gotowy)
vector<int> stos_wynikowy;  //posortowane wierzchołki
vector<int> sciezka;
bool cykl = false;

int zegar = 0;
vector<int> czas_wejscia;
vector<int> czas_wyjscia;

void wczytaj_z_pliku(string pliku) 
{
    ifstream plik(pliku);
    if (!plik.is_open()) {
        cout << "BLAD: Nie mozna otworzyc pliku " << pliku << "!" << endl;
        return;
    }

    plik >> n >> m; //n - wierzcholki, m - krawedzie

    // czyszcze stare dane
    ln.clear();
    ms.clear();
    lk.clear();
    czas_wejscia.clear();
    czas_wyjscia.clear();
    stos_wynikowy.clear();
    odwiedzone.clear();
    sciezka.clear();
    cykl = false;
    zegar = 0;

    //struktury maja rozmiar n
    ln.resize(n + 1);
    ms.resize(n + 1);
    for(int i = 0; i <= n; i++) {
        ms[i].resize(n + 1, false);
    }
    czas_wejscia.resize(n + 1, 0);
    czas_wyjscia.resize(n + 1, 0);

    // wczytuje m krawedzi
    for (int i = 0; i < m; i++) {
        int w1, w2;
        plik >> w1 >> w2;
        
        ln[w1].push_back(w2);
        ms[w1][w2] = true;
        lk.push_back({w1, w2});
    }

    plik.close();
    cout << "SUKCES: Wczytano graf z pliku (" << n << " wierzcholkow, " << m << " krawedzi)." << endl;
}

void tarjan_ln(int v)//v to wierzcholek od ktorego zaczynam wypisywac
{
    if(cykl) return;

    odwiedzone[v] = 1;        //1 - wierzcholek jest odwiedzany
    czas_wejscia[v] = ++zegar; 
    sciezka.push_back(v);

    for(int i=0; i<ln[v].size(); i++) 
    {
        int sasiad = ln[v][i];
        
        if(odwiedzone[sasiad] == 1) // cykl znaleziony
        {
            cykl = true;
            cout << "BLAD: Cykl wykryty!" << endl;
            cout << "Wierzcholki tworzace cykl: ";
            bool w_cyklu = false;
            for(int i = 0; i < sciezka.size(); i++) 
            {
                int w = sciezka[i];
    
                if(w == sasiad) 
                {
                    w_cyklu = true;
                }
                if(w_cyklu) cout << w << " ";
            }
            cout << endl;
            return;
        }
        if(odwiedzone[sasiad] == 0) // nie odwiedzone wczesniej
        {
            tarjan_ln(sasiad);
        }
    }

    odwiedzone[v] = 2;          // 2 to odwiedzony i bez cyklu
    czas_wyjscia[v] = ++zegar;  // Zapisujemy moment przetworzenia
    stos_wynikowy.push_back(v); //dodaje do wyniku
    sciezka.pop_back(); //usuwa ostatni wierzcholek
}
void tarjan_ms(int v) 
{
    if(cykl) return;

    odwiedzone[v] = 1;        
    czas_wejscia[v] = ++zegar; 
    sciezka.push_back(v);

    for(int i = 1; i < ms.size(); i++) 
    {
        if(ms[v][i] == true) 
        {
            int sasiad = i;
            
            if(odwiedzone[sasiad] == 1) 
            {
                cykl = true;
                cout << "BLAD: Cykl wykryty!" << endl;
                cout << "Wierzcholki tworzace cykl: ";
                bool w_cyklu = false;
                for(int j = 0; j < sciezka.size(); j++) 
                {
                    int w = sciezka[j];
                    if(w == sasiad) w_cyklu = true;
                    if(w_cyklu) cout << w << " ";
                }
                cout << endl;
                return;
            }
            if(odwiedzone[sasiad] == 0) 
            {
                tarjan_ms(sasiad); 
            }
        }
    }

    odwiedzone[v] = 2;          
    czas_wyjscia[v] = ++zegar;  
    stos_wynikowy.push_back(v); 
    sciezka.pop_back();
}
void tarjan_lk(int v) 
{
    if(cykl) return;

    odwiedzone[v] = 1;        
    czas_wejscia[v] = ++zegar; 
    sciezka.push_back(v);

    for(int i = 0; i < lk.size(); i++) 
    {
        if(lk[i].first == v) 
        {
            int sasiad = lk[i].second; 
            
            if(odwiedzone[sasiad] == 1) 
            {
                cykl = true;
                cout << "BLAD: Cykl wykryty!" << endl;
                cout << "Wierzcholki tworzace cykl: ";
                bool w_cyklu = false;
                for(int j = 0; j < sciezka.size(); j++) 
                {
                    int w = sciezka[j];
                    if(w == sasiad) w_cyklu = true;
                    if(w_cyklu) cout << w << " ";
                }
                cout << endl;
                return;
            }
            if(odwiedzone[sasiad] == 0) 
            {
                tarjan_lk(sasiad); 
            }
        }
    }

    odwiedzone[v] = 2;          
    czas_wyjscia[v] = ++zegar;  
    stos_wynikowy.push_back(v); 
    sciezka.pop_back();
}
void wypisz_graf(int reprezentacja) {
    cout << "\n--- STRUKTURA GRAFU ---" << endl;
    if (reprezentacja == 1) {
        cout << "Lista Nastepnikow (ln):" << endl;
        for (int i = 1; i <= n; i++) {
            cout << "Wierzcholek " << i << ": ";
            for (int j = 0; j < ln[i].size(); j++) cout << ln[i][j] << " ";
            cout << endl;
        }
    } 
    else if (reprezentacja == 2) {
        cout << "Macierz Sasiedztwa (ms):" << endl;
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) cout << ms[i][j] << " ";
            cout << endl;
        }
    } 
    else if (reprezentacja == 3) {
        cout << "Lista Krawedzi (lk):" << endl;
        for (int i = 0; i < lk.size(); i++) {
            cout << lk[i].first << " -> " << lk[i].second << endl;
        }
    }
}
void generuj_DAG(int liczba_wierzcholkow, double nasycenie) 
{
    n = liczba_wierzcholkow;
    
    long long max_krawedzi = ((long long)n * (n - 1)) / 2; 
    m = max_krawedzi * nasycenie;

    ln.clear(); ms.clear(); lk.clear();
    czas_wejscia.clear(); czas_wyjscia.clear();
    stos_wynikowy.clear(); odwiedzone.clear(); sciezka.clear();
    cykl = false; zegar = 0;

    ln.resize(n + 1);
    ms.resize(n + 1);
    for(int i = 0; i <= n; i++) ms[i].resize(n + 1, false);
    czas_wejscia.resize(n + 1, 0);
    czas_wyjscia.resize(n + 1, 0);

    int dodane_krawedzie = 0;
    while(dodane_krawedzie < m) 
    {
        // u < v aby byl acykliczny
        int u = 1 + rand() % (n - 1);
        int v = u + 1 + rand() % (n - u);

        if(ms[u][v] == false) 
        {
            ms[u][v] = true;
            ln[u].push_back(v);
            lk.push_back({u, v});
            dodane_krawedzie++;
        }
    }
}
int main()
{
    int opcja = -1;
    string nazwa;
    
    while(opcja != 0)
    {
        cout << "1. Wczytaj graf z pliku" << endl;
        cout << "2. Uruchom algorytm Tarjana" << endl;
        cout << "0. Wyjdz z programu" << endl;
        cout << "Wybierz opcje: ";
        cin >> opcja;

        switch(opcja) 
        {
            case 1:
                cout << "Podaj nazwe pliku do wczytania: ";
                cin >> nazwa;
                wczytaj_z_pliku(nazwa);
                break;
                
            case 2:
            {
                if(n == 0) {
                    cout << "BLAD: Najpierw wczytaj graf (opcja 1)!" << endl;
                    break;
                }

                int reprezentacja, start;
                cout << "\nWybierz reprezentacje grafu:" << endl;
                cout << "1 - Lista Nastepnikow (ln)" << endl;
                cout << "2 - Macierz Sasiedztwa (ms)" << endl;
                cout << "3 - Lista Krawedzi (lk)" << endl;
                cout << "Wybierz (1/2/3): ";
                cin >> reprezentacja;

                cout << "Podaj wierzcholek startowy: ";
                cin >> start;

                wypisz_graf(reprezentacja);

                //czyszczenie
                odwiedzone.assign(n + 1, 0);
                czas_wejscia.assign(n + 1, 0);
                czas_wyjscia.assign(n + 1, 0);
                stos_wynikowy.clear();
                sciezka.clear();
                cykl = false;
                zegar = 0;

                auto start_czasu = chrono::high_resolution_clock::now();

                if (reprezentacja == 1) {
                    tarjan_ln(start);
                    for (int i = 1; i < ln.size(); i++) 
                        if (!ln[i].empty() && odwiedzone[i] == 0) tarjan_ln(i); //tarjan od nowa
                } 
                else if (reprezentacja == 2) {
                    tarjan_ms(start);
                    for (int i = 1; i <= n; i++) 
                        if (odwiedzone[i] == 0) tarjan_ms(i);
                } 
                else if (reprezentacja == 3) {
                    tarjan_lk(start);
                    for (int i = 1; i <= n; i++) 
                        if (odwiedzone[i] == 0) tarjan_lk(i);
                }
                else {
                    cout << "Zly wybor reprezentacji!" << endl;
                    break;
                }

                auto koniec_czasu = chrono::high_resolution_clock::now();
                auto czas_trwania = chrono::duration_cast<chrono::microseconds>(koniec_czasu - start_czasu).count();

                if(!cykl) 
                {
                    cout << "\nSortowanie topologiczne: ";
                    for(int i = stos_wynikowy.size() - 1; i >= 0; i--) {
                        cout << stos_wynikowy[i] << " ";
                    }
                    cout << endl;
                    
                    cout << "\nMomenty (wejscia / wyjscia): " << endl;
                    for(int i = 1; i <= n; i++) {
                        if(czas_wejscia[i] > 0)
                            cout << "Wierzcholek " << i << ": " << czas_wejscia[i] << " / " << czas_wyjscia[i] << endl;
                    }

                    cout << "\n-> CZAS WYKONANIA ALGORYTMU: " << czas_trwania << " mikrosekund." << endl;
                }
                    
                break;
            }
            case 0:
                cout << "Zamykanie programu..." << endl;
                break;
            default:
                cout << "Nieznana opcja! Wybierz ponownie." << endl;
        }
    }
    return 0;
}
    
