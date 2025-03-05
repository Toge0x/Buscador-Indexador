#include "tokenizador.h"
#include <fstream>
#include <sstream>

/*
    FUNCIONES AMIGAS
*/

/*
    cout << "DELIMITADORES: " << delimiters << " TRATA CASOS ESPECIALES: " << casosEspeciales << " PASAR A MINUSCULAS Y SIN ACENTOS: " << pasarAminuscSinAcentos;
    
    Aunque se modifique el almacenamiento de los delimitadores por temas de eficiencia,
    el campo delimiters se imprimirá con el string leído en el tokenizador
    (tras las modificaciones y eliminación de los caracteres repetidos correspondientes)
*/
ostream& operator<<(ostream& os, const Tokenizador& t){
    os << "DELIMITADORES: " << t.delimiters << " TRATA CASOS ESPECIALES: " << t.casosEspeciales << " PASAR A MINUSCULAS Y SIN ACENTOS: " << t.pasarAminuscSinAcentos;
    return os;
}

/*
    CONSTRUCTORES
*/

/*
    Inicializa delimiters a delimitadoresPalabra filtrando que no se
    introduzcan delimitadores repetidos (de izquierda a derecha, en cuyo
    caso se eliminarían los que hayan sido repetidos por la derecha);
    casosEspeciales a kcasosEspeciales;
    pasarAminuscSinAcentos a minuscSinAcentos;
*/
Tokenizador::Tokenizador(const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos){
    for(int i = 0; i < delimitadoresPalabra.length(); i++){
        char caracter = delimitadoresPalabra[i];
        bool existe = false;
        for(int j = 0; j < delimiters.length(); j++){
            if(delimiters[j] == caracter){      // si está en los que ya tenemos
                existe = true;
                break;
            }
        }
        if(existe == false){        // si no está lo añadimos al final
            delimiters += caracter;
        }
    }
    for(int i = 0; i < delimiters.length(); i++){
        delimitadores[i] = true;
    }
    casosEspeciales = kcasosEspeciales;
    pasarAminuscSinAcentos = minuscSinAcentos;
}

// Constructor de copia
Tokenizador::Tokenizador(const Tokenizador& t){
    this->delimiters = t.delimiters;
    for(int i = 0; i < delimiters.length(); i++){
        delimitadores[i] = true;
    }
    this->casosEspeciales = t.casosEspeciales;
    this->pasarAminuscSinAcentos = t.pasarAminuscSinAcentos;
}

/*
    Inicializa delimiters=",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t@";
    casosEspeciales a true;
    pasarAminuscSinAcentos a false
*/
Tokenizador::Tokenizador(){
    delimiters = ",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t@";
    //this->delimitadores[256] = {false};                     // 256 posibles caracteres del ISO-8859
    casosEspeciales = true;
    pasarAminuscSinAcentos = false;
}

/*
    DESTRUCTOR
*/

// Pone delimiters=""
Tokenizador::~Tokenizador(){
    delimiters = "";
}

/*
    FUNCIONES
*/

// Tokeniza str devolviendo el resultado en tokens. La lista tokens se vaciará antes de almacenar el resultado de la tokenización. 
void Tokenizador::Tokenizar(const std::string& str, std::list<std::string>& tokens) const{
    tokens.clear();         // limpiamos la lista de tokens
    string palabra;         // para almacenar cada palabra
    char caracter;

    for(int i = 0; i < str.length(); i++){
        caracter = str[i];
        if(delimitadores[static_cast<unsigned char>(caracter)] == true){    // si es un delimitador (cast a 256 valores posibles)
            if(palabra.size() == 0){
                tokens.push_back(palabra);
                palabra.clear();
            }
        }else{
            palabra += caracter;
        }
    }

    // si la ultima no está vacia la añadimos
    if(palabra.size() == 0){
        tokens.push_back(palabra);
    }
}

/*
Tokeniza el fichero i guardando la salida en el fichero f (una palabra en cada línea del fichero).
    - Devolverá true si se realiza la tokenización de forma correcta;
    - False en caso contrario enviando a cerr el mensaje correspondiente
(p.ej. que no exista el archivo i)

*/
bool Tokenizador::Tokenizar(const string& i, const string& f) const{
    ifstream entrada(i);
    ofstream salida(f);
    string palabra = "";
    char caracter;

    if(entrada.is_open() == false){
        cerr << "El fichero no se ha podido abrir o no existe\n";
        return false;
    }else{
        while(entrada.get(caracter)){
            if(encontrarCaracterEnDelimitadores(caracter)){     // si es un delimitador
                if(palabra.length() != 0){
                    salida << palabra;                              // printea la palabra
                    palabra.clear();                                // limpiamos para la siguiente
                    salida << "\n";
                }
            }else{
                palabra += caracter;                // concatenamos la palabra
            }
        }

        if(palabra.size() != 0){
            salida << palabra;
            palabra.clear();
        }

        entrada.close();
        salida.close();

        return true;
    }

}

bool Tokenizador::encontrarCaracterEnDelimitadores(const char& caracter) const{
    bool encontrado = false;
    for(int i = 0; i < delimiters.length(); i++){
        if(caracter == delimiters[i]){
            encontrado = true;
        }
    }
    return encontrado;
}

/*
Tokeniza el fichero i guardando la salida en un fichero de nombre i añadiéndole extensión .tk
(sin eliminar previamente la extensión de i por ejemplo, del archivo pp.txt se generaría el resultado en pp.txt.tk),
y que contendrá una palabra en cada línea del fichero.
    - Devolverá true si se realiza la tokenización de forma correcta;
    - False en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i)
*/
bool Tokenizador::Tokenizar(const string & i) const{
    bool correcto = false;
    string nombreSalida = i + ".tk";
    if(this->Tokenizar(i, nombreSalida)){
        correcto = true;
    }
    return correcto;
}

/*
Tokeniza el fichero i que contiene un nombre de fichero por línea guardando la salida en ficheros
(uno por cada línea de i) cuyo nombre será el leído en i añadiéndole extensión .tk,
y que contendrá una palabra en cada línea del fichero leído en i.
    - Devolverá true si se realiza la tokenización de forma correcta de todos los archivos que contiene i;
    - False en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i,
      o que se trate de un directorio, enviando a "cerr" los archivos de i que no existan o que sean directorios;
      luego no se ha de interrumpir la ejecuci�n si hay alg�n archivo en i que no exista)
*/
bool Tokenizador::TokenizarListaFicheros(const string& i) const{
    bool correcto = false;
    stringstream listaFicheros(i);
    string fichero;
    while(getline(listaFicheros, fichero, '\n')){
        ifstream ficheroALeer(fichero);
        if(this->Tokenizar(fichero)){
            correcto = true;
        }else{
            return false;       // si uno falla interrumpir todo
        }
    }
    return correcto;
} 

/*
Tokeniza todos los archivos que contenga el directorio i, incluyendo los de los subdirectorios,
guardando la salida en ficheros cuyo nombre será el de entrada añadiéndole extensión .tk
y que contendrá una palabra en cada línea del fichero. 
    - Devolverá true si se realiza la tokenización de forma correcta de todos los archivos;
    - Devolverá false en caso contrario enviando a cerr el mensaje correspondiente
    (p.ej. que no exista el directorio i, o los ficheros que no se hayan podido tokenizar)
*/
bool Tokenizador::TokenizarDirectorio(const string& i) const{

}

/*
Inicializa delimiters a nuevoDelimiters, filtrando que no se introduzcan delimitadores repetidos
(de izquierda a derecha, en cuyo caso se eliminarían los que hayan sido repetidos por la derecha)
*/
void Tokenizador::DelimitadoresPalabra(const string& nuevoDelimiters){
    delimiters.clear();
    for(int i = 0; i < nuevoDelimiters.size(); i++){
        char delimitador = nuevoDelimiters[i];
        size_t pos = delimiters.find_first_of(delimitador);
        if(pos == string::npos){
            i++;
        }else{
            delimiters.push_back(delimitador);
        }
    }
}

// Añade al final de "delimiters" los nuevos delimitadores que aparezcan en "nuevoDelimiters" (no se almacenarán caracteres repetidos)
void Tokenizador::AnyadirDelimitadoresPalabra(const string& nuevoDelimiters){   // falta revisarlo en casa
    for(int i = 0; i < nuevoDelimiters.size(); i++){
        size_t pos = nuevoDelimiters.find_first_of(this->delimiters);
        if(pos == string::npos){        // ya está en el string de delimitadores
            i++;
        }else{
            
        }
    }
}

// Devuelve "delimiters"
string Tokenizador::DelimitadoresPalabra() const{
    return delimiters;
}

// Cambia la variable privada "casosEspeciales"
void Tokenizador::CasosEspeciales(const bool& nuevoCasosEspeciales){
    casosEspeciales = nuevoCasosEspeciales;
}

// Devuelve el contenido de la variable privada "casosEspeciales"
bool Tokenizador::CasosEspeciales(){
    return casosEspeciales;
}

/*
Cambia la variable privada "pasarAminuscSinAcentos".
Atención al formato de codificación del corpus (comando "file" de Linux). 
Para la corrección de la práctica se utilizará el formato actual (ISO-8859).
*/
void Tokenizador::PasarAminuscSinAcentos(const bool& nuevoPasarAminuscSinAcentos){

}

// Devuelve el contenido de la variable privada "pasarAminuscSinAcentos"
bool Tokenizador::PasarAminuscSinAcentos(){
    return pasarAminuscSinAcentos;
}

/*
    OPERADORES
*/

// No pone nada xd, algo hará
Tokenizador& Tokenizador::operator=(const Tokenizador& t) {
    if (this != &t) {
        delimiters = t.delimiters;
        casosEspeciales = t.casosEspeciales;
        pasarAminuscSinAcentos = t.pasarAminuscSinAcentos;
        std::copy(std::begin(t.delimiters), std::end(t.delimiters), std::begin(delimiters));
    }
    return *this;
}
