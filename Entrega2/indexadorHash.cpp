#include "indexadorHash.h"
#include <sys/stat.h>
#include <cstdlib>
#include <fstream>

ostream& operator<<(ostream &s, const IndexadorHash &p){
    s << "Fichero con el listado de palabras de parada: " << p.ficheroStopWords << endl;
    s << "Tokenizador: " << p.tok << endl;
    s << "Directorio donde se almacenara el indice generado: " << p.directorioIndice << endl;
    s << "Stemmer utilizado: " << p.tipoStemmer << endl;
    s << "Informacion de la coleccion indexada: " << p.informacionColeccionDocs << endl;
    s << "Se almacenaran las posiciones de los terminos: " << p.almacenarPosTerm;

    return s;
}

IndexadorHash::IndexadorHash(const string& fichStopWords, const string& delimitadores, const bool& detectComp, const bool& minuscSinAcentos, const string& dirIndice, const int& tStemmer, const bool& almPosTerm){
    ficheroStopWords = ficheroStopWords;        // fichero que contiene las stopwords
    tok.DelimitadoresPalabra(delimitadores);    // campos del tokenizador
    tok.CasosEspeciales(detectComp);
    tok.PasarAminuscSinAcentos(minuscSinAcentos);
    tipoStemmer = tStemmer;             // tipo de stemmer (0, 1, 2)
    almacenarPosTerm = almPosTerm;
    if(dirIndice == ""){        // obtener el directorio actual si dirIndice == ""
        char ruta[PATH_MAX];
        if(getcwd(ruta, sizeof(ruta)) != nullptr){
            directorioIndice = string(ruta);
        }else{
            directorioIndice = ".";
        }
    }else{
        directorioIndice = dirIndice;
    }
    // meter las stopswords en la variable stopWords list<string>
    ifstream ficherito(fichStopWords);
    if(ficherito.is_open()){
        string stopword;
        while(getline(ficherito, stopword)){
            stopWords.insert(stopword);     // metemos la stopword
        }
        ficherito.close();
    }else{
        cerr << "No se ha podido abrir el fichero de las stopWords\n";
    }
    // el resto de atributos se inicializan por defecto al ser tipos de datos no primitivos
}

IndexadorHash::IndexadorHash(const string& directorioIndexacion){

}