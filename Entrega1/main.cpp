#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <locale>
#include "tokenizador.h"

int main() {
    std::locale loc("en_US.ISO8859-1"); // ISO 8859-1
    std::locale::global(loc);
    std::cout.imbue(loc);

    // prueba TokenizarListaFicheros(lista)
    std::string listaFicheros = "lista1.txt\nlista2.txt";

    // prueba TokenizarFicheros(input, output)
    std::string nombreEntrada = "entrada.txt";
    std::string nombreSalida = "salida.txt";

    // prueba DelimitadoresPalabra(nuevosDelimitadores)
    std::string nuevos = "894";

    Tokenizador tokenizador;
    cout << "Delimitadores actuales: " << tokenizador.DelimitadoresPalabra() << "\n";
    tokenizador.DelimitadoresPalabra(nuevos);
    cout << "Delimitadores nuevos: " << tokenizador.DelimitadoresPalabra() << "\n";
    
    return 0;
}
