#include "tokenizador.h"
#include "indexadorInformacion.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <unordered_set>

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

/*
// Tokeniza str devolviendo el resultado en el par�metro tokens con los caracteres especiales da igual cual sea
void Tokenizador::TokenizarCasosEspeciales(const string& str, list<string>& tokens) const{
    tokens.clear();
    string palabra;
    char caracter;
    bool esURL = false;
    bool esNumero = false;
    int j;
    string especiales = "_:/.?&-=#@";
    bool copiaDelimitadores[256];   // array a sar en casos especiales
    for(int i = 0; i < 256; i++){
        copiaDelimitadores[i] = delimitadores[i];
    }
    copiaDelimitadores[32] = true;          // A MANO FORZAR EL CARACTER ESPACIO EN BLANCO
    bool delimitadoresURL[256] = {false};   // delimitadores especiales para las URL
    for(int i = 0; i < 256; i++){
        delimitadoresURL[i] = copiaDelimitadores[i];
    }
    for(char caracter : especiales){
        delimitadoresURL[static_cast<unsigned char> (caracter)] = false;    // solo está el resto de delimitadores exceptuando los validos en URL
    }
    for(int i = 0; i < str.length(); i++){
        caracter = str[i];
        if(copiaDelimitadores[static_cast<unsigned char> (caracter)] == true){   // es delimitador, fin de palabra
            if(palabra.empty() == false){
                tokens.push_back(palabra);
                palabra.clear();
            }
        }else{                              // no es un delimitador
            if(palabra.empty() == true){    // inicio de palabra
                if(i + 8 <= str.length() && (str.substr(i, 6) == "ftp://" || str.substr(i, 7) == "http://" || str.substr(i, 8) == "https://")){
                    esURL = true;
                }
                if(i + 8 <= str.length() && pasarAminuscSinAcentos == true
                && (str.substr(i, 6) == "Ftp://" || str.substr(i, 7) == "Http://" || str.substr(i, 8) == "Https://")){
                    esURL = true;
                }
                if(esURL == true){  //  es una URL almacenamos todo el contenido
                    for(j = i; j < str.length(); j++){
                        caracter = str[j];
                        if(delimitadoresURL[static_cast<unsigned char>(caracter)] == false){    // no es delimitador (aunque sea uno especial // excepcion)
                            unsigned char uc = static_cast<unsigned char>(caracter);
                            if((uc >= 'A' && uc <= 'Z') || (uc >= 192 && uc <= 256)) {  // letras latinas y acentuadas
                                    caracter = static_cast<char>(uc + 32);
                            }
                            palabra += caracter;
                        }else{                                  // fin de URL
                            if(palabra.empty() == false){
                                tokens.push_back(palabra);
                                palabra.clear();
                                break;
                            }
                        }
                    }
                    i = j;
                    esURL = false;
                }else{      // no es nada
                    if(copiaDelimitadores[static_cast<unsigned char>(caracter)] == true){
                        if(palabra.length() != 0){
                            tokens.push_back(palabra);
                            palabra.clear();
                        }
                    }else{
                        palabra += caracter;
                    }
                }
            }else{          // no es inicio de palabra
                palabra += caracter;
            }
        }
    }
    if(palabra.empty() == false){   // si la frase no termina con delimitador hay que añadirlo
        tokens.push_back(palabra);
        palabra.clear();
    }
}

*/

// funcion para pasar de mayusculas y con acentos a minusculas y sin acentos
string Tokenizador::TransformarMinusculaSinAcentos(const string& palabra) const{
    string resultado = "";
    for(unsigned char c : palabra){
        switch (c) {
            case 193: case 225: c = 'a'; break; // Á, á
            case 201: case 233: c = 'e'; break; // É, é
            case 205: case 237: c = 'i'; break; // Í, í
            case 211: case 243: c = 'o'; break; // Ó, ó
            case 218: case 250: c = 'u'; break; // Ú, ú
            case 252: case 220: c = 'u'; break; // ü, Ü
            case 209: c = 241; break;           // Ñ → ñ
            default:
                if (c >= 'A' && c <= 'Z') {
                    c = c + 32;     // pasar a minuscula
                }
        }
        resultado += static_cast<char>(c);
    }
    return resultado;
}


// Tokeniza str devolviendo el resultado en tokens. La lista tokens se vaciar� antes de almacenar el resultado de la tokenizaci�n. 
void Tokenizador::Tokenizar(const string& str, list<std::string>& tokens) const{
    tokens.clear();
    string palabra;
    char caracter;

    for (int i = 0; i < str.length(); i++) {
        caracter = str[i];
        if (delimitadores[static_cast<unsigned char>(caracter)]) {
            if (!palabra.empty()) {
                if (pasarAminuscSinAcentos) {
                    palabra = TransformarMinusculaSinAcentos(palabra);
                }
                tokens.push_back(palabra);
                palabra.clear();
            }
        } else {
            palabra += caracter;
        }
    }

    if (!palabra.empty()) {
        if (pasarAminuscSinAcentos) {
            palabra = TransformarMinusculaSinAcentos(palabra);
        }
        tokens.push_back(palabra);
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
bool Tokenizador::TokenizarListaFicheros(const string& i) const {
    bool algunFallo = false;
    ifstream ficheros(i);
    struct stat dir;

    if (!ficheros.is_open() || stat(i.c_str(), &dir) == 0 && S_ISDIR(dir.st_mode)) {
        cerr << "ERROR: El archivo " << i << " no existe o se trata de un directorio" << endl;
        return false;
    }

    string ficheroATokenizar;
    while (getline(ficheros, ficheroATokenizar)) {
        if (stat(ficheroATokenizar.c_str(), &dir) != 0 || S_ISDIR(dir.st_mode)) {
            cerr << "ERROR: El archivo " << ficheroATokenizar << " no existe o es un directorio pero no se interrumpe la ejecución" << endl;
            algunFallo = true;
            continue;
        }

        if (!Tokenizar(ficheroATokenizar)) {
            cerr << "ERROR: No se pudo tokenizar el archivo " << ficheroATokenizar << endl;
            algunFallo = true;
        }
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
    for(int i = 0; i < 256; i++){   // reiniciar delimitadores
        delimitadores[i] = false;
    }
    delimiters.clear();
    for(const auto& caracter : nuevoDelimiters){ // asignar los nuevos
        if(delimitadores[static_cast<unsigned char> (caracter)] == false){  // si no está lo añadimos
            delimiters += caracter;
            delimitadores[static_cast<unsigned char> (caracter)] = true;
        }
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
bool Tokenizador::CasosEspeciales() const{
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
bool Tokenizador::PasarAminuscSinAcentos() const{
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
