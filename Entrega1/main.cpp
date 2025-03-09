#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include "tokenizador.h"
#include <chrono>
#include <iomanip>
using namespace std;
using namespace std::chrono;

int main() {
    Tokenizador tokenizador;
    list<string> tokens;
    string s = "lista_ficheros.txt";
    auto inicio = chrono::high_resolution_clock::now();
    tokenizador.TokenizarListaFicheros(s);

    auto fin = chrono::high_resolution_clock::now();
    duration<double> duration = fin - inicio;

    cout << "Tiempo de ejecución: " << duration.count() << " s" << endl;

    return 0;
}
