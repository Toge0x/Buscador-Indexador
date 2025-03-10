#include <string.h>
#include <ostream>
#include <iostream>
#include <list>
using namespace std;

class Tokenizador {
    friend ostream& operator<<(ostream&, const Tokenizador&);
    
    public:
        Tokenizador (const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos);
        Tokenizador (const Tokenizador&);
        Tokenizador ();
        ~Tokenizador ();
        Tokenizador& operator= (const Tokenizador&);
        void TokenizarCasosEspeciales(const string &str, list<string> &tokens) const;   // auxiliar para casos especiales
        void Tokenizar(const string &str, list<string> &tokens) const;
        bool Tokenizar (const string& i, const string& f) const; 
        bool Tokenizar (const string & i) const;
        bool TokenizarListaFicheros (const string& i) const;
        bool TokenizarDirectorio (const string& i) const;
        void DelimitadoresPalabra(const string& nuevoDelimiters); 
        void AnyadirDelimitadoresPalabra(const string& nuevoDelimiters);
        string DelimitadoresPalabra() const;  
        void CasosEspeciales (const bool& nuevoCasosEspeciales);
        bool CasosEspeciales ();
        void PasarAminuscSinAcentos (const bool& nuevoPasarAminuscSinAcentos);
        bool PasarAminuscSinAcentos ();

    private:
        /*
            Delimitadores de términos. Aunque se modifique la forma de almacenamiento interna para mejorar la eficiencia, 
            este campo debe permanecer para indicar el orden en que se introdujeron los delimitadores
        */
        string delimiters;

        /*
            Array de booleanos para trabajar más eficientemente, ISO-8859 (1 byte cada delimitador)
        */
        bool delimitadores[256];    // 256 posibles caracteres del ISO-8859

        /*
            Si true detectará palabras compuestas y casos especiales. 
            Sino, trabajará al igual que el algoritmo propuesto en la sección "Versión del tokenizador vista en clase"

        */
        bool casosEspeciales;

        /*
            Si true pasará el token a minúsculas y quitará acentos, antes de realizar la tokenización
        */
        bool pasarAminuscSinAcentos;
};
