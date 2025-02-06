#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include "tokenizador.h"

int main() {
    std::string nombreEntrada = "entrada.txt";
    std::string nombreSalida = "salida.txt";

    Tokenizador tokenizador;
    tokenizador.Tokenizar(nombreEntrada, nombreSalida);

    return 0;
}
