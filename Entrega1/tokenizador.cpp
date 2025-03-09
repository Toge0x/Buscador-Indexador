#include "tokenizador.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

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
    delimiters = "";
    for(int i = 0; i < 256; i++){   // inicializacion previa
        delimitadores[i] = false;
    }
    for(int i = 0; i < delimitadoresPalabra.size(); i++){
        auto caracter = delimitadoresPalabra[i];
        if(delimitadores[static_cast<unsigned char>(caracter)] == false){   // cambiamos a unsigned para no tener indices negativos
            delimiters += caracter;
            delimitadores[static_cast<unsigned char>(caracter)] = true;     // delimitador introducido ya, evita repes
        }
    }
    casosEspeciales = kcasosEspeciales;
    pasarAminuscSinAcentos = minuscSinAcentos;
}

// Constructor de copia
Tokenizador::Tokenizador(const Tokenizador& t){
    delimiters = t.delimiters;
    for(int i = 0; i < 256; i++){
        delimitadores[i] = t.delimitadores[i];  // copiar el array de delimitadores
    }
    casosEspeciales = t.casosEspeciales;
    pasarAminuscSinAcentos = t.pasarAminuscSinAcentos;
}

/*
    Inicializa delimiters=",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t@";
    casosEspeciales a true;
    pasarAminuscSinAcentos a false
*/
Tokenizador::Tokenizador(){
    delimiters = ",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t@";
    for(int i = 0; i < 256; i++){
        delimitadores[i] = false;
    }
    for(auto delimitador : delimiters){
        delimitadores[static_cast<unsigned char> (delimitador)] = true;
    }
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
    
    string palabra;         // para almacenar cada palabra
    char caracter;

    for(int i = 0; i < str.length(); i++){
        caracter = str[i];
        if(delimitadores[static_cast<unsigned char>(caracter)] == true){    // si es un delimitador (cast a 256 valores posibles)
            if(palabra.length() != 0){
                tokens.push_back(palabra);
                palabra.clear();
            }
        }else{
            palabra += caracter;
        }
    }

    // si la ultima no está vacia la añadimos
    if(palabra.length() != 0){
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
    string linea;
    list<string> tokens;

    if(entrada.is_open() == false){
        cerr << "ERROR: No existe el archivo: " << i << endl;
        return false;
    }else{
        while(entrada.eof() == false){
            linea = "";
            getline(entrada, linea);
            if(linea.length() != 0){
                this->Tokenizar(linea, tokens);
            }
        }
        entrada.close();
        ofstream salida(f);
        list<string>::iterator itS;
        for(itS = tokens.begin(); itS != tokens.end(); itS++){
            salida << (*itS) << "\n";
        }
        salida.close();
        return true;
    }
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
    bool correcto = true;
    ifstream ficheros(i);
    string ficheroATokenizar;
    struct stat dir;
    list<string> tokens;
    string lineaEnFichero = "";

    if(ficheros.is_open() == true && stat(i.c_str(), &dir) != 0 || S_ISDIR(dir.st_mode) == false){
        while(getline(ficheros, ficheroATokenizar)){    // cogemos linea a linea
            tokens.clear();
            if(stat(ficheroATokenizar.c_str(), &dir) != 0 || S_ISDIR(dir.st_mode) == false){ // comprobacion directorio
                ifstream fichero(ficheroATokenizar);
                ofstream salida(ficheroATokenizar + ".tk");
                if(fichero.is_open() == true){
                    while(getline(fichero, lineaEnFichero)){
                        tokens.clear();
                        Tokenizar(lineaEnFichero, tokens);
                        for(auto token : tokens){
                            salida << token << "\n";
                        }
                    }
                    fichero.close();
                    salida.close();
                }else{
                    cerr << "ERROR: No existe el archivo " << ficheroATokenizar << " pero no se interrumpe la ejecución" << endl;
                    continue;
                }
            }else{
                cerr << "ERROR: El archivo " << ficheroATokenizar << " es un directorio pero no se interrumpe la ejecución" << endl;
                continue;
            }
        }
    }else{
        cerr << "ERROR: El archivo " << i << " no existe o se trata de un directorio" << endl;
        return false;
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
