#include "tokenizador.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <unordered_set>

const unordered_set<char> caracteresURL = {'_', ':', '/', '.', '?', '&', '-', '=', '#', '@'};
/*
    FUNCIONES AMIGAS
*/

/*
    cout << "DELIMITADORES: " << delimiters << " TRATA CASOS ESPECIALES: " << casosEspeciales << " PASAR A MINUSCULAS Y SIN ACENTOS: " << pasarAminuscSinAcentos;
    
    Aunque se modifique el almacenamiento de los delimitadores por temas de eficiencia,
    el campo delimiters se imprimir� con el string le�do en el tokenizador
    (tras las modificaciones y eliminaci�n de los caracteres repetidos correspondientes)
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
    caso se eliminar�an los que hayan sido repetidos por la derecha);
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
    Inicializa delimiters=",;:.-/+*\\ '\"{}[]()<>�!�?&#=\t@";
    casosEspeciales a true;
    pasarAminuscSinAcentos a false
*/
Tokenizador::Tokenizador(){
    delimiters = ",;:.-/+*\\ '\"{}[]()<>�!�?&#=\t@";
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
    METODOS
*/

// Tokeniza str devolviendo el resultado en el par�metro tokens con los caracteres especiales da igual cual sea
void Tokenizador::TokenizarCasosEspeciales(const string& str, list<string>& tokens) const{
    string url;
    bool esURL = false;
    string palabra;
    char caracter;
    // mismo método que con delimitadores pero para el caso de URLs
    bool caracteresValidosURL[256] = {false}; 
    for (char c = 'a'; c <= 'z'; c++) caracteresValidosURL[static_cast<unsigned char> (c)] = true;
    for (char c = 'A'; c <= 'Z'; c++) caracteresValidosURL[static_cast<unsigned char> (c)] = true;
    for (char c = '0'; c <= '9'; c++) caracteresValidosURL[static_cast<unsigned char> (c)] = true;
    caracteresValidosURL[static_cast<unsigned char> ('_')] = true;
    caracteresValidosURL[static_cast<unsigned char> (':')] = true;
    caracteresValidosURL[static_cast<unsigned char> ('/')] = true;
    caracteresValidosURL[static_cast<unsigned char> ('.')] = true;
    caracteresValidosURL[static_cast<unsigned char> ('?')] = true;
    caracteresValidosURL[static_cast<unsigned char> ('&')] = true;
    caracteresValidosURL[static_cast<unsigned char> ('-')] = true;
    caracteresValidosURL[static_cast<unsigned char> ('=')] = true;
    caracteresValidosURL[static_cast<unsigned char> ('#')] = true;
    caracteresValidosURL[static_cast<unsigned char> ('@')] = true;

    for(int i = 0; i < str.length(); i++){
        caracter = str[i];
        if(delimitadores[static_cast<unsigned char>(caracter)] == false){
            if(palabra.empty() == true){    // inicio de token
                bool esFTP, esHTTP, esHTTPS = false;
                if(i + 8 < str.length()){       // si esta pos + 8 < length puede tener https:// y el resto
                    string posibleHTTPS = str.substr(i, 5);
                    string posibleHTTP = str.substr(i, 4);
                    string posibleFTP = str.substr(i, 3);
                    if(pasarAminuscSinAcentos == true){
                        for(auto& caracter : posibleFTP) caracter = tolower(caracter);
                        for(auto& caracter : posibleHTTP) caracter = tolower(caracter);
                        for(auto& caracter : posibleHTTPS) caracter = tolower(caracter);
                    }
                    if(posibleFTP == "ftp"){
                        esFTP = true;
                    }else if(posibleHTTP == "http"){
                        esHTTP = true;
                    }else if (posibleHTTPS == "https"){
                        esHTTPS = true;
                    }
                }
                if(esFTP == true){
                    palabra += str.substr(i, 6); // agregamos ftp://
                    i += 5;
                    esURL = true;
                    esFTP = false;
                }else if(esHTTP == true){
                    palabra += str.substr(i, 7); // agregamos http://
                    i += 6;
                    esURL = true;
                    esHTTP = false;
                }else if(esHTTPS == true){
                    palabra += str.substr(i, 8); // agregamos https://
                    i += 7;
                    esURL = true;
                    esHTTPS = false;
                }else{
                    palabra += caracter;    // no es caso especial concatena igual
                }
                if(esURL == true){  // hay que leer toda la URL
                    while(i < str.length()){    // no es anidado, el indice avanzará
                        i++;
                        caracter = str[i];
                        if(caracteresValidosURL[static_cast<unsigned char>(caracter)] == true){
                            palabra += caracter;
                        }else{
                            break;  // delimitador no válido
                        }
                    }
                    tokens.push_back(palabra);
                    palabra.clear();
                    esURL = false;
                    i--;
                    continue;
                }
            }else{                      // no es inicio de token
                palabra += caracter;    // concatenar caracteres
            }
        }else{
            if(palabra.empty() == false){   // delimitador encontrado
                tokens.push_back(palabra);
                palabra.clear();
            }
        }
    }

    if(palabra.empty() == false){   // si antes de terminar queda una palabra con contenido se agrega
        tokens.push_back(palabra);
        palabra.clear();
    }
}

// Tokeniza str devolviendo el resultado en tokens. La lista tokens se vaciar� antes de almacenar el resultado de la tokenizaci�n. 
void Tokenizador::Tokenizar(const string& str, list<std::string>& tokens) const{
    string palabra;         // para almacenar cada palabra
    char caracter;
    
    if(casosEspeciales == true){
        TokenizarCasosEspeciales(str, tokens);
    }else{
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

        // si la ultima no est� vacia la a�adimos
        if(palabra.length() != 0){
            tokens.push_back(palabra);
        }
    }
}

/*
Tokeniza el fichero i guardando la salida en el fichero f (una palabra en cada l�nea del fichero).
    - Devolver� true si se realiza la tokenizaci�n de forma correcta;
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
Tokeniza el fichero i guardando la salida en un fichero de nombre i a�adi�ndole extensi�n .tk
(sin eliminar previamente la extensi�n de i por ejemplo, del archivo pp.txt se generar�a el resultado en pp.txt.tk),
y que contendr� una palabra en cada l�nea del fichero.
    - Devolver� true si se realiza la tokenizaci�n de forma correcta;
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
Tokeniza el fichero i que contiene un nombre de fichero por l�nea guardando la salida en ficheros
(uno por cada l�nea de i) cuyo nombre ser� el le�do en i a�adi�ndole extensi�n .tk,
y que contendr� una palabra en cada l�nea del fichero le�do en i.
    - Devolver� true si se realiza la tokenizaci�n de forma correcta de todos los archivos que contiene i;
    - False en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i,
      o que se trate de un directorio, enviando a "cerr" los archivos de i que no existan o que sean directorios;
      luego no se ha de interrumpir la ejecuci?n si hay alg?n archivo en i que no exista)
*/
bool Tokenizador::TokenizarListaFicheros(const string& i) const{
    bool algunFallo = false;
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
                        for(const auto& token : tokens){
                            salida << token << "\n";
                        }
                    }
                    fichero.close();
                    salida.close();
                }else{
                    cerr << "ERROR: No existe el archivo " << ficheroATokenizar << " pero no se interrumpe la ejecuci�n" << endl;
                    algunFallo = true;
                    continue;
                }
            }else{
                cerr << "ERROR: El archivo " << ficheroATokenizar << " es un directorio pero no se interrumpe la ejecuci�n" << endl;
                algunFallo = true;
                continue;
            }
        }
    }else{
        cerr << "ERROR: El archivo " << i << " no existe o se trata de un directorio" << endl;
        algunFallo = true;
    }
    return !algunFallo;
} 

/*
Tokeniza todos los archivos que contenga el directorio i, incluyendo los de los subdirectorios,
guardando la salida en ficheros cuyo nombre ser� el de entrada a�adi�ndole extensi�n .tk
y que contendr� una palabra en cada l�nea del fichero. 
    - Devolver� true si se realiza la tokenizaci�n de forma correcta de todos los archivos;
    - Devolver� false en caso contrario enviando a cerr el mensaje correspondiente
    (p.ej. que no exista el directorio i, o los ficheros que no se hayan podido tokenizar)
*/
bool Tokenizador::TokenizarDirectorio(const string& i) const{
    bool correcto = true;
    struct stat dir;
    int error = stat(i.c_str(), &dir);
    if(error != -1 && S_ISDIR(dir.st_mode) == true){
        string cmd = "find " + i + " -follow |sort > .lista_fich";
        system(cmd.c_str());
        return TokenizarListaFicheros(".lista_fich");
    }else{
        cerr << "ERROR: La ruta " << i << " no es un directorio o los ficheros no se han podido tokenizar" << endl;
        return false;
    }
}

/*
Inicializa delimiters a nuevoDelimiters, filtrando que no se introduzcan delimitadores repetidos
(de izquierda a derecha, en cuyo caso se eliminar�an los que hayan sido repetidos por la derecha)
*/
void Tokenizador::DelimitadoresPalabra(const string& nuevoDelimiters){
    delimiters = nuevoDelimiters;
    for(int i = 0; i < 256; i++){   // reiniciar delimitadores
        delimitadores[i] = false;
    }
    for(const auto& caracter : delimiters){ // asignar los nuevos
        delimitadores[static_cast<unsigned char>(caracter)] = true;
    }
}

// A�ade al final de "delimiters" los nuevos delimitadores que aparezcan en "nuevoDelimiters" (no se almacenar�n caracteres repetidos)
void Tokenizador::AnyadirDelimitadoresPalabra(const string& nuevoDelimiters){
    for(const auto& caracter : nuevoDelimiters){
        if(delimitadores[static_cast<unsigned char> (caracter)] == false){  // si no est� lo a�adimos
            delimiters += caracter;
            delimitadores[static_cast<unsigned char> (caracter)] = true;
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
Atenci�n al formato de codificaci�n del corpus (comando "file" de Linux). 
Para la correcci�n de la pr�ctica se utilizar� el formato actual (ISO-8859).
*/
void Tokenizador::PasarAminuscSinAcentos(const bool& nuevoPasarAminuscSinAcentos){
    pasarAminuscSinAcentos = nuevoPasarAminuscSinAcentos;
}

// Devuelve el contenido de la variable privada "pasarAminuscSinAcentos"
bool Tokenizador::PasarAminuscSinAcentos(){
    return pasarAminuscSinAcentos;
}

/*
    OPERADORES
*/

// Sobrecarga del operador=
Tokenizador& Tokenizador::operator=(const Tokenizador& t) {
    if (this != &t) {
        delimiters = t.delimiters;
        for(const auto& caracter : delimiters){
            delimitadores[static_cast<unsigned char>(caracter)] = true;
        }
        casosEspeciales = t.casosEspeciales;
        pasarAminuscSinAcentos = t.pasarAminuscSinAcentos;
    }
    return *this;
}
