#include "tokenizador.h"
#include <iostream>
#include <list>
#include <string>

using namespace std;

int main() {
    // Crear tokenizador con delimitadores personalizados, sin casos especiales, y pasando a min√∫sculas sin acentos
    string delimitadores = " ,.;:-_()[]{}\"'\t\n°ø!?";
    Tokenizador t(delimitadores, false, true);

    cout << "Configuraci√≥n del tokenizador: " << t << endl;

    // 1. Tokenizar una cadena manual
    string frase = "°Hola, MUNDO! øCÛmo est· Èl? ¡…Õ”⁄ Ò — ¸ ‹";
    list<string> tokens;
    t.Tokenizar(frase, tokens);

    cout << "\nTokens de la frase:\n";
    for (const auto& token : tokens) {
        cout << "- " << token << endl;
    }

    return 0;
}
