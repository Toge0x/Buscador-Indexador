#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include "tokenizador.h"
#include <chrono>
using namespace std;
using namespace std::chrono;

int main() {
    Tokenizador t;
    string tokenizar = "lista_ficheros.txt";

    auto start = high_resolution_clock::now();

    bool resultado = t.TokenizarListaFicheros(tokenizar);
    
    auto stop = high_resolution_clock::now();
    duration<double> duration = stop - start;
    cout << "Tiempo de ejecución: " << duration.count() << " s" << endl;

    return 0;
}