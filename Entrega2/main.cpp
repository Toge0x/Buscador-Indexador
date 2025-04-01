#include <iostream> 
#include <string>
#include <list> 
#include <fstream>
#include "tokenizador.h"

using namespace std;

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
		bool kCasosEspeciales = true, kpasarAminusculas = true;

		list<string> lt1, lt2, lt3;
        list<string> tokens; 

		Tokenizador a("-#", true, false);
        a.DelimitadoresPalabra("/ ");

		a.PasarAminuscSinAcentos(true);
        a.Tokenizar("pal1#MS-DOSaA#p3 p1 p2", tokens);
	    imprimirListaSTL(tokens);

		
}