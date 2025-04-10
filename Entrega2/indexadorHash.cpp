#include "indexadorHash.h"
#include "stemmer.h"
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
    ficheroStopWords = fichStopWords;        // fichero que contiene las stopwords
    tok.DelimitadoresPalabra(delimitadores);    // campos del tokenizador
    tok.CasosEspeciales(detectComp);
    tok.PasarAminuscSinAcentos(minuscSinAcentos);
    tipoStemmer = tStemmer;             // tipo de stemmer (0, 1, 2)
    almacenarPosTerm = almPosTerm;
    if(dirIndice == ""){        // obtener el directorio actual si dirIndice == ""
        char ruta[PATH_MAX];
        if(getcwd(ruta, sizeof(ruta)) != nullptr){      // si puede obtener el directorio actual
            directorioIndice = string(ruta);
        }else{
            directorioIndice = ".";     // si no lo pondrá a "."
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
    RecuperarIndexacion(directorioIndexacion);
}

// Constrcutor de copia
IndexadorHash::IndexadorHash(const IndexadorHash &ind){
    ficheroStopWords = ind.ficheroStopWords;
    stopWords = ind.stopWords;
    tok = ind.tok;
    tipoStemmer = ind.tipoStemmer;
    directorioIndice = ind.directorioIndice;
    informacionColeccionDocs = ind.informacionColeccionDocs;
    almacenarPosTerm = ind.almacenarPosTerm;
    indice = ind.indice;
    indiceDocs = ind.indiceDocs;
    indicePregunta = ind.indicePregunta;
    infPregunta = ind.infPregunta;
    pregunta = ind.pregunta;
}


IndexadorHash::~IndexadorHash(){
    indice.clear();         // vaciamos los indices
    indiceDocs.clear();
    indicePregunta.clear();
    pregunta.clear();
    stopWords.clear();
    ficheroStopWords.clear();
    directorioIndice.clear();
    tipoStemmer = 0;
    almacenarPosTerm = false;
}

// operador asignacion, copia campo a campo
IndexadorHash &IndexadorHash::operator=(const IndexadorHash &ind){
    if(this != &ind){
        ficheroStopWords = ind.ficheroStopWords;
        stopWords = ind.stopWords;
        tok = ind.tok;
        tipoStemmer = ind.tipoStemmer;
        directorioIndice = ind.directorioIndice;
        informacionColeccionDocs = ind.informacionColeccionDocs;
        almacenarPosTerm = ind.almacenarPosTerm;
        indice = ind.indice;
        indiceDocs = ind.indiceDocs;
        indicePregunta = ind.indicePregunta;
        infPregunta = ind.infPregunta;
        pregunta = ind.pregunta;
    }

    return *this;
}

bool IndexadorHash::Indexar(const string &ficheroDocumentos){}


bool IndexadorHash::IndexarDirectorio(const string &dirAIndexar){
    struct stat dir;
    int err = stat(dirAIndexar.c_str(), &dir);

    if (err == -1 || !S_ISDIR(dir.st_mode)){
        return false;
    }else{
        string cmd = "find " + dirAIndexar + " -follow -type f | sort > .lista_ficheros_indexador";
        system(cmd.c_str());

        return Indexar(".lista_ficheros_indexador");
    }
}

bool IndexadorHash::GuardarIndexacion() const{}

bool IndexadorHash::RecuperarIndexacion(const string &directorioIndexacion){}

void IndexadorHash::ImprimirIndexacion() const{
    cout << "Terminos indexados: " << "\n";
    ListarTerminos();
    cout << "Documentos indexados: " << "\n";
    ListarDocs();
}

bool IndexadorHash::EsStopWord(const string& token){    // devuelve si el token es una stopword
    return stopWords.find(token) != stopWords.end();
}

bool IndexadorHash::IndexarPregunta(const string &preg){
    indicePregunta.clear();
    infPregunta = InformacionPregunta();
    stemmerPorter stem;

    list<string> lista_tokens;
    tok.Tokenizar(preg, lista_tokens);    // tokenizamos la pregunta
    infPregunta.incrementarNPalabras(lista_tokens.size());    // actualizamos el total de palabras de la pregunta con la cantidad de tokens

    if(lista_tokens.empty()){
        return false;
    }

    // para cada token obtenido le aplicamos stem y lo indexamos si no es stopword
    list<string>::iterator it = lista_tokens.begin();
    int pos = 0;

    while(it != lista_tokens.end()){
        string token = *it;
        if(tipoStemmer != 0){
            stem.stemmer(token, tipoStemmer);
        }
        if(!EsStopWord(token) && token.length() > 1){
            indicePregunta[token].addFT();
            infPregunta.incrementarNPalabrasSinParada(1);
            if(almacenarPosTerm){
                indicePregunta[token].addPosition(pos);
            }
        }
        ++it;
        ++pos;
    }

    infPregunta.incrementarNPalabrasDiferentes(indicePregunta.size());  // incrementamos la cantidad de palabras diferentes de la pregunta
    pregunta = preg;

    return true;
}

bool IndexadorHash::DevuelvePregunta(string &preg) const{
    if(pregunta.size() != 0){
        preg = this->pregunta;
        return true;
    }else{
        return false;
    }
}

bool IndexadorHash::DevuelvePregunta(const string &word, InformacionTerminoPregunta &inf) const{
    auto pos = indicePregunta.find(word);
    if(pos != indicePregunta.end()){
        inf = pos->second;
        return true;
    }else{
        return false;
    }
}

bool IndexadorHash::DevuelvePregunta(InformacionPregunta& inf) const{
    if(indicePregunta.empty() == false){
        inf = infPregunta;
        return true;
    }

    return false;
}

void IndexadorHash::ImprimirIndexacionPregunta(){
    unordered_map<string, InformacionTerminoPregunta>::const_iterator it;
    cout << "Pregunta indexada: " << pregunta << '\n';
    cout << "Terminos indexados en la pregunta: ";
    for(it = indicePregunta.begin(); it != indicePregunta.end(); ++it){
        cout << it->first << '\t' << it->second << '\n';
    }

    cout << "Informacion de la pregunta: " << infPregunta << '\n';
}

void IndexadorHash::ImprimirPregunta(){
    cout << "Pregunta indexada : " << pregunta << '\n';
    cout << "Informacion de la pregunta : " << infPregunta << '\n';
}

bool IndexadorHash::Devuelve(const string &word, InformacionTermino &inf) const{
    auto pos = indice.find(word);
    if(pos != indice.end()){        // vemos si la palabra está indexada en el indice
        inf = pos->second;          // cogemos la InformacionTermino
        return true;
    }else{                          // si no lo está devolvemos uno vacio
        inf = InformacionTermino{};
        return false;
    }
}

bool IndexadorHash::Devuelve(const string& word, const string& nomDoc, InfTermDoc& i) const{
    auto doc = indiceDocs.find(nomDoc);     //                                     word
    auto pos_indice = indice.find(word);    // si lo encuentra sera un hash map <string, InformacionTermino>

    if(doc != indiceDocs.end() && pos_indice != indice.end()){     // si el documento esta indexado y la palabra tambien
        auto aux = pos_indice->second.getLDocs().find(doc->second.getIdDoc());  // buscamos en los documentos en los que aparece word con id de documento IdDoc
        if(aux != pos_indice->second.getLDocs().end()){                         // el documento en el que aparece word existe
            i = aux->second;
            return true;
        }
    }else{                  // si alguno no devolvemos uno vacio
        i = InfTermDoc{};
        return false;
    }
}

bool IndexadorHash::Existe(const string &word) const{
    return (indice.find(word) != indice.end());
}

bool IndexadorHash::BorraDoc(const string &nomDoc){}

void IndexadorHash::VaciarIndiceDocs(){
    indiceDocs.clear();
}

void IndexadorHash::VaciarIndicePreg(){
    indicePregunta.clear();
}

int IndexadorHash::NumPalIndexadas() const{
    return indice.size();
}

string IndexadorHash::DevolverFichPalParada() const{
    return ficheroStopWords;
}

void IndexadorHash::ListarPalParada() const{
    std::unordered_set<std::string>::const_iterator it;
    for(it = stopWords.begin(); it != stopWords.end(); ++it)
        cout << (*it) << "\n";
}

int IndexadorHash::NumPalParada() const{
    return stopWords.size();
}

string IndexadorHash::DevolverDelimitadores() const{
    return tok.DelimitadoresPalabra();
}

bool IndexadorHash::DevolverCasosEspeciales(){
    return tok.CasosEspeciales();
}

bool IndexadorHash::DevolverPasarAminuscSinAcentos(){
    return tok.PasarAminuscSinAcentos();
}

bool IndexadorHash::DevolverAlmacenarPosTerm() const{
    return almacenarPosTerm;
}

string IndexadorHash::DevolverDirIndice() const{
    return directorioIndice;
}

int IndexadorHash::DevolverTipoStemming() const{
    return tipoStemmer;
}

void IndexadorHash::ListarInfColeccDocs() const{
    cout << informacionColeccionDocs << "\n";
}

void IndexadorHash::ListarTerminos() const {
    unordered_map<string, InformacionTermino>::const_iterator it;
    for (it = indice.begin(); it != indice.end(); ++it) {
        cout << it->first << '\t' << it->second << '\n';
    }
}

bool IndexadorHash::ListarTerminos(const string& documento) const{

    auto itIndiceDocs = indiceDocs.find(documento);  // devuelve un hash map iterator <string, InfDoc> con el documento encontrado
    if(itIndiceDocs != indiceDocs.end()){            // comprobamos que sí lo ha encontrado
        for(auto itIndice = indice.begin(); itIndice != indice.end(); ++itIndice){  // recorremos los terminos indexados
            auto itLdocs = itIndice->second.getLDocs().find(itIndiceDocs->second.getIdDoc());
            if(itLdocs != itIndice->second.getLDocs().end())
                cout << itLdocs->first << '\t' << itIndice->second << "\n";
        }
        return true;
    }else{                  // no existe el documento indexado
        return false;
    }
}

void IndexadorHash::ListarDocs() const {
    unordered_map<string, InfDoc>::const_iterator it;
    for (it = indiceDocs.begin(); it != indiceDocs.end(); ++it) {
        cout << it->first << '\t' << it->second << '\n';
    }
}

bool IndexadorHash::ListarDocs(const string& nomDoc) const{
    auto itIndiceDocs = indiceDocs.find(nomDoc);    // devuelve un hash map iterator <string, InfDoc> con el documento encontrado
    if(itIndiceDocs != indiceDocs.end()){           // si encuentra el documento indexado en los indices
        cout << itIndiceDocs->first << '\t' << itIndiceDocs->second << "\n";    // recorremos los documentos indexados
        return true;
    }else{
        return false;
    }
}