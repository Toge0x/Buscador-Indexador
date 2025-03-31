#include "tokenizador.h"
#include <iostream>
#include <list>
#include <string>

using namespace std;

int main() {
    // Crear tokenizador con delimitadores personalizados, sin casos especiales, y pasando a minúsculas sin acentos
    string delimitadores = " ,.;:-_()[]{}\"'\t\n��!?";
    Tokenizador t(delimitadores, false, true);

    cout << "Configuración del tokenizador: " << t << endl;

    // 1. Tokenizar una cadena manual
    string frase = "�Hola, MUNDO! �C�mo est� �l? ����� � � � �";
    list<string> tokens;
    t.Tokenizar(frase, tokens);

    cout << "\nTokens de la frase:\n";
    for (const auto& token : tokens) {
        cout << "- " << token << endl;
    }

    return 0;
}
