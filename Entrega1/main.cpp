#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include "tokenizador.h"
#include <chrono>
#include <iomanip>
using namespace std;
using namespace std::chrono;

///////// Comprobación de que vacíe la lista resultado

void imprimirListaSTL(const list<string>& cadena)
{
        list<string>::const_iterator itCadena;
        for(itCadena=cadena.begin();itCadena!=cadena.end();itCadena++)
        {
                cout << (*itCadena) << ", ";
        }
        cout << endl;
}



int
main(void)
{
        string s = "lista_tokens.txt";
        Tokenizador a;
        a.TokenizarListaFicheros(s);
        return 0;

}