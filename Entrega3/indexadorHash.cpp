#include "indexadorHash.h"
#include "stemmer.h"
#include <sys/stat.h>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <string>
#include <iterator>
#include <fstream>

bool EsStopWord(const unordered_set<string>& stopWords, const string& token);   // declaracion para usarla mas arriba

ostream& operator<<(ostream &s, const IndexadorHash &p){
    s << "Fichero con el listado de palabras de parada: " << p.ficheroStopWords << '\n';
    s << "Tokenizador: " << p.tok << '\n';
    s << "Directorio donde se almacenara el indice generado: " << p.directorioIndice << '\n';
    s << "Stemmer utilizado: " << p.tipoStemmer << '\n';
    s << "Informacion de la coleccion indexada: " << p.informacionColeccionDocs << '\n';
    s << "Se almacenaran las posiciones de los terminos: " << p.almacenarPosTerm << '\n';

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

bool TerminoEsNumero(const string& str){
    if (str.empty()) return false;

    for (char c : str) {
        if (!isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }

    return true;
}



void IndexadorHash::procesarTokensDeDocumento(istream& archivoTokens, int idDoc, InfDoc& infoDoc, stemmerPorter& stem) {
    unordered_map<string, InformacionTermino> indice_sub;
    istream_iterator<string> it(archivoTokens);
    istream_iterator<string> end;

    int pos = 0;
    for(; it != end; ++it, ++pos){
        string termino = *it;
        infoDoc.addNumPal();    // metemos 1 palabra

        if(tipoStemmer != 0){ // aplicamos stemming si está activado
            stem.stemmer(termino, tipoStemmer);
        }

        if(EsStopWord(stopWords, termino) || termino.length() <= 1) continue;     // filtrar las palabras no validas para el indice

        // añadir al índice principal si ya existía
        if(indice[termino].getFT() > 0 || TerminoEsNumero(termino) || informacionColeccionDocs.getNumTotalPal() < 50){
            infoDoc.addNumPalSinParada();
            if (indice[termino].agregarPosicionADocumento(idDoc, pos, almacenarPosTerm)) {
                infoDoc.addNumPalDiferentes();
            }
        }
        else {
            if (indice_sub[termino].getFT() == 0) {
                indice_sub[termino].agregarPosicionADocumento(idDoc, pos, almacenarPosTerm);
            }
            else {
                indice_sub.erase(termino);  // moverlo al índice principal
                infoDoc.addNumPalSinParada();
                if (indice[termino].agregarPosicionADocumento(idDoc, pos, almacenarPosTerm)) {
                    infoDoc.addNumPalDiferentes();
                }
            }
        }
    }

    indice_sub.clear();
}


void IndexadorHash::procesarDocumento(const string& rutaDocumento, int& idActual, stemmerPorter& stem) {
    struct stat infoArchivo{};
    if(stat(rutaDocumento.c_str(), &infoArchivo) != 0){     // comprobamos que el fichero existe
        cerr << "ERROR al acceder a " << rutaDocumento << ": " << strerror(errno) << '\n';
        return;
    }

    auto itIndexado = indiceDocs.find(rutaDocumento);   // si el fichero existe comprobamos
    bool yaIndexado = itIndexado != indiceDocs.end();
    if(yaIndexado && infoArchivo.st_mtim.tv_sec <= itIndexado->second.getFechaModificacion()){   // si el archivo no se ha modificado desde que se indexó
        idActual++;     // saltamos el documento (indexacion innecesaria)
        return;
    }

    tok.Tokenizar(rutaDocumento);   // tokenizamos el fichero si ha sido modificado para indexarlo
    ifstream archivoTokens(rutaDocumento + ".tk");
    if(!archivoTokens){     // si no se ha podido abrir el archivo de tokens lo saltamos
        idActual++;
        return;
    }

    if(yaIndexado){             // si ya estaba indexado borramos el documento del indice
        idActual = itIndexado->second.getIdDoc();  // reutilizamos el ID
        BorraDoc(rutaDocumento);                   // lo eliminamos antes de indexar
    }

    InfDoc infoDoc{};                   // creamos el documento a indexar
    infoDoc.setIdDoc(idActual);         // con idActual y el tamaño en bytes
    infoDoc.setTamBytes(infoArchivo.st_size);
    procesarTokensDeDocumento(archivoTokens, idActual, infoDoc, stem);  // leemos los tokens, aplicamos stemming 

    indiceDocs[rutaDocumento] = infoDoc;                            // agregamos el documento a la coleccion
    informacionColeccionDocs.agregarDocumentoAColeccion(infoDoc);
}

bool IndexadorHash::Indexar(const string& ficheroDocumentos){
    if (!ifstream(ficheroDocumentos)) {
        cerr << "No se puede abrir el fichero de documentos: " << ficheroDocumentos << '\n';
        return false;
    }

    ifstream listado(ficheroDocumentos);
    int nextID = 1;
    string rutaDocumento;
    stemmerPorter stem;

    while(getline(listado, rutaDocumento)){
        procesarDocumento(rutaDocumento, nextID, stem);
        ++nextID;
    }
    informacionColeccionDocs.setPalabrasDiferentes(indice.size());      // actualizamos el indice con las palabras totales en la coleccion

    return true;
}

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

    void guardarTerminosIndexadosIndice(ofstream& salida, const unordered_map<string, InformacionTermino>& indice){
        salida << indice.size() << ' ';
        for(const auto& [termino, infoTermino] : indice){
            salida << termino << ' ' << infoTermino.getFT() << ' ';     // frecuencia del termino en TODA LA COLECCION

            const auto& apariciones = infoTermino.getLDocs();           // documentos en los que aparece el termino
            salida << apariciones.size() << ' ';

            for(const auto& [idDoc, datosDoc] : apariciones){
                salida << idDoc << ' ' << datosDoc.getFT() << ' ';  // frecuencia del termino en el documento idDoc

                const auto& posiciones = datosDoc.getPositionTerm();    // donde aparece el termino en el documento idDoc
                salida << posiciones.size() << ' ';
                for(int pos : posiciones){
                    salida << pos << ' ';
                }
            }
        }
        salida << '\n';
    }

    void leerTerminosIndexadosIndice(ifstream& archivo, unordered_map<string, InformacionTermino>& indice){
        int numTerminos;
        archivo >> numTerminos;

        for(int i = 0; i < numTerminos; ++i){
            string termino;
            int ftTotal, numDocs;
            archivo >> termino >> ftTotal >> numDocs;

            for(int j = 0; j < numDocs; ++j){
                int idDoc, ftDoc, numPos;
                archivo >> idDoc >> ftDoc >> numPos;

                for(int k = 0; k < numPos; ++k){
                    int pos;
                    archivo >> pos;
                    indice[termino].agregarPosicionADocumento(idDoc, pos, true);
                }
            }
        }
    }


    void guardarInformacionDocumento(ofstream& salida, const pair<const string, InfDoc>& entradaDoc){
        const string& nombreDocumento = entradaDoc.first;
        const InfDoc& info = entradaDoc.second;

        salida << nombreDocumento << ' '
            << info.getIdDoc() << ' '
            << info.getNumPal() << ' '
            << info.getNumPalSinParada() << ' '
            << info.getNumPalDiferentes() << ' '
            << info.getTamBytes() << ' '
            << info.getFechaModificacion() << '\n';
    }

    void leerInformacionDocumentos(ifstream& entrada, unordered_map<string, InfDoc>& indiceDocs, int cantidadDocs){
        string nombreDocumento;
        int idDocumento, totalPalabras, palabrasSinParada, palabrasDiferentes, bytes = 0;
        time_t fecha = 0;
        for(int i = 0; i < cantidadDocs; i++){ 
            entrada >> nombreDocumento >> idDocumento >> totalPalabras >> palabrasSinParada >> palabrasDiferentes >> bytes >> fecha;
            InfDoc informacionDocumento{};
            informacionDocumento.setIdDoc(idDocumento);
            informacionDocumento.setNumPal(totalPalabras);
            informacionDocumento.setNumPalSinParada(palabrasSinParada);
            informacionDocumento.setNumPalDifernetes(palabrasDiferentes);
            informacionDocumento.setTamBytes(bytes);
            informacionDocumento.setFechaModificacion(fecha);
            indiceDocs.emplace(nombreDocumento, informacionDocumento);  // insertamos directamente en el hashmap <nombreDoc, infoDocumento>
        }

    }

    void guardarInformacionColeccionDocs(ofstream& salida, const InfColeccionDocs& coleccion){
        int nDocs = coleccion.getNumDocs();
        int totalPal = coleccion.getNumTotalPal();
        int sinStop = coleccion.getNumTotalPalSinParada();
        int diferentes = coleccion.getNumTotalPalDiferentes();
        int tam = coleccion.getTamBytes();

        salida << nDocs << ' ' << totalPal << ' ' << sinStop << ' '<< diferentes << ' ' << tam << '\n';
    }

    void leerInformacionColeccionDocs(ifstream& entrada, InfColeccionDocs& coleccionDocumentos){
        int cantidadDocumentos, totalPal, palabrasSinStop, palabrasDiferentes, tamBytes = 0;
        entrada >> cantidadDocumentos >> totalPal >> palabrasSinStop >> palabrasDiferentes >> tamBytes;     // leemos los campos de la coleccion
        coleccionDocumentos.setNumDocs(coleccionDocumentos.getNumDocs() + cantidadDocumentos);              // añadimos los campos leidos a cada uno de los campos de la coleccion
        coleccionDocumentos.setTotalPalabras(coleccionDocumentos.getNumTotalPal() + totalPal);              // existente + leido
        coleccionDocumentos.setPalabrasSinParada(coleccionDocumentos.getNumTotalPalSinParada() + palabrasSinStop);
        coleccionDocumentos.setPalabrasDiferentes(coleccionDocumentos.getNumTotalPalDiferentes() + palabrasDiferentes);
        coleccionDocumentos.setBytes(coleccionDocumentos.getTamBytes() + tamBytes);
    }

    void guardarInformacionPregunta(ofstream& salida, const InformacionPregunta& pregunta){
        salida << pregunta.getNumTotalPal() << ' ' << pregunta.getNumTotalPalSinParada() << ' ' << pregunta.getNumTotalPalDiferentes() << '\n';
    }

    void leerInformacionPregunta(ifstream& entrada, InformacionPregunta& pregunta){
        int totalPalabras, palabrasSinParada, palabrasDiferentes = 0;

        entrada >> totalPalabras >> palabrasSinParada >> palabrasDiferentes;
        pregunta.incrementarNPalabras(totalPalabras);
        pregunta.incrementarNPalabrasSinParada(palabrasSinParada);
        pregunta.incrementarNPalabrasDiferentes(palabrasDiferentes);

    }

    void guardarTerminoPregunta(ofstream& salida, const pair<const string, InformacionTerminoPregunta>& entrada){
        const string& termino = entrada.first;
        const InformacionTerminoPregunta& info = entrada.second;

        salida << termino << ' ' << info.getFT() << ' ';

        const auto& posiciones = info.getSortedPositions();
        for(int pos : posiciones){
            salida << pos << ' ';
        }

        salida << '\n';
    }

    void leerTerminoPregunta(ifstream& entrada, unordered_map<string, InformacionTerminoPregunta>& indicePregunta) {
        string termino;
        int ft;
        while(entrada >> termino >> ft){
            InformacionTerminoPregunta info;
            for (int i = 0; i < ft; ++i) {
                int pos;
                if(!(entrada >> pos)) break;  // por si el archivo termina mal
                info.addItemToPos(pos);
                info.addFT();
            }
            indicePregunta.emplace(termino, info);
        }
    }

    bool IndexadorHash::GuardarIndexacion() const{
        bool correcto = true;
        string fileIndexacion = directorioIndice + "/indexacion-amsm30.ua";
        struct stat info{};
        bool directorioExiste = (stat(directorioIndice.c_str(), &info) == 0) && S_ISDIR(info.st_mode);

        if(directorioExiste == false){
            int resultado = mkdir(directorioIndice.c_str(), 0777);  // resultado != 0 no se ha creado el directorio
            if(resultado != 0){
                std::cerr << "No se pudo crear el directorio \"" << directorioIndice << "\": " << strerror(errno) << '\n';  // error de creacion 
                correcto = false;
            }
        }
        // si todo ha ido bien y se ha creado el directorio de indexacion
        ofstream archivo(fileIndexacion, ios::binary);  // binario para guardar la indexacion
        if(archivo.is_open() == false){
            correcto = false;
            cerr << "No se pudo abrir el archivo" << '\n';
        }
        // guardamos 1 a 1 los campos privados
        archivo << ficheroStopWords << '\n';
        archivo << tok.DelimitadoresPalabra() << '\n';
        archivo << tok.CasosEspeciales() << '\n';
        archivo << tok.PasarAminuscSinAcentos() << '\n';
        archivo << directorioIndice << '\n';
        archivo << tipoStemmer << '\n';
        archivo << almacenarPosTerm << '\n';

        guardarTerminosIndexadosIndice(archivo, indice);
        guardarInformacionColeccionDocs(archivo, informacionColeccionDocs);
        for(const auto& doc : indiceDocs)
            guardarInformacionDocumento(archivo, doc);
        guardarInformacionPregunta(archivo, infPregunta);
        for(const auto& terminoPregunta : indicePregunta)
            guardarTerminoPregunta(archivo, terminoPregunta);

        archivo.close();
        return correcto;
    }

    bool IndexadorHash::RecuperarIndexacion(const string &directorioIndexacion){
        bool correcto = true;
        ifstream archivo(directorioIndexacion + "/indexacion-amsm30.ua", ios::binary);
        string delimitadores;
        bool casosEspeciales, pasarMinusculas;

        indice.clear();     // borramos los posibles datos en estos campos
        indiceDocs.clear();
        indicePregunta.clear();

        if(archivo.is_open() == false){
            cerr << "No se pudo abrir el archivo de indexación." << '\n';   // no se abre el fichero que contiene la indexacion
            correcto = false;
        }else{
            getline(archivo, ficheroStopWords); // leemos todos los campos del fichero de indexacion
            getline(archivo, delimitadores);    // está literalmente arriba como leerlo
            archivo >> casosEspeciales;
            archivo >> pasarMinusculas;
            archivo.ignore();                   // salto de linea
            getline(archivo, directorioIndice);
            archivo >> tipoStemmer;
            archivo >> almacenarPosTerm;

            ifstream stopwordsFile(ficheroStopWords);   // abrimos el fichero de stopwords
            if(stopwordsFile.is_open() == true){
                string palabra;
                while(stopwordsFile >> palabra){
                    stopWords.insert(palabra);          // metemos palabra a palabra en stopWords
                }
            }else{
                cerr << "No se pudo abrir el archivo de stopwords." << '\n';
                correcto = false;
            }
            tok = Tokenizador(delimitadores, casosEspeciales, pasarMinusculas);     // casi se me olvida xd
            // recuperar informacion de los documentos y terminos
            leerTerminosIndexadosIndice(archivo, indice);   // leemos cada uno de los campos privados (clases externas)
            leerInformacionColeccionDocs(archivo, informacionColeccionDocs);
            int cantidadDocumentos = informacionColeccionDocs.getNumDocs();
            leerInformacionDocumentos(archivo, indiceDocs, cantidadDocumentos);     // NO SE PUEDE SABER DESDE INDICEDOCS LA CANTIDAD DE DOCUMENTOS INDEXADOS
            // recuperar informacion de la pregunta
            leerInformacionPregunta(archivo, infPregunta);
            leerTerminoPregunta(archivo, indicePregunta);
            archivo.close();
        }
        
        return correcto;
    }

void IndexadorHash::ImprimirIndexacion() const{
    cout << "Terminos indexados: " << "\n";
    ListarTerminos();
    cout << "Documentos indexados: " << "\n";
    ListarDocs();
}

bool EsStopWord(const unordered_set<string>& stopWords, const string& token){    // devuelve si el token es una stopword
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
        if(EsStopWord(stopWords, token) == false && token.length() > 1){
            indicePregunta[token].addFT();
            infPregunta.incrementarNPalabrasSinParada(1);
            if(almacenarPosTerm){
                indicePregunta[token].addItemToPos(pos);
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

bool IndexadorHash::Devuelve(const string& word, const string& nomDoc, InfTermDoc& i) const {
    string palabra;
    if(tok.PasarAminuscSinAcentos()){
        palabra = tok.TransformarMinusculaSinAcentos(word);
    }else{
        palabra = word;
    }
    if(tipoStemmer != 0){
        stemmerPorter stem;
        stem.stemmer(palabra, tipoStemmer);
    }
    auto doc = indiceDocs.find(nomDoc);
    auto pos_indice = indice.find(palabra);

    if(doc != indiceDocs.end() && pos_indice != indice.end()) {
        int idDoc = doc->second.getIdDoc();
        auto itLdocs = pos_indice->second.getLDocs().find(idDoc);
        if(itLdocs != pos_indice->second.getLDocs().end()) {
            i = itLdocs->second;
            return true;
        }
    }
    i = InfTermDoc{};
    return false;
}

bool IndexadorHash::Existe(const string &word) const{
    return (indice.find(word) != indice.end());
}

bool IndexadorHash::BorraDoc(const string &nomDoc){
    bool eliminado = true;
    auto itIndiceDocs = indiceDocs.find(nomDoc);    // buscamos el documento
    if(itIndiceDocs == indiceDocs.end()){           // si no existe paramos
        eliminado = false;
    }else{
        auto documentoIt = indice.begin();
        while(documentoIt != indice.end()){     // hay que borrar el documento de cada termino del indice para eliminar las referencias
            documentoIt->second.eliminarTerminoDeDocumento(itIndiceDocs->second.getIdDoc());    // eliminamos los terminos 1 a 1 del documento (y cada uno de los l_docs al que pertenece el documento)
            if(documentoIt->second.getFT() > 0){    // si el termino sigue apareciendo en otros documentos lo mantenemos en el indice y avanzamos
                documentoIt++;
            }else{
                documentoIt = indice.erase(documentoIt);    // si tiene FT = 0 borramos el termino se elimina del indice
            }
        }
        informacionColeccionDocs.borrarDocumentoDeColeccion(itIndiceDocs->second);  // borramos el documento de la coleccion
        informacionColeccionDocs.setPalabrasDiferentes(indice.size());              // actualizamos el número de palabras distintas --> cantidad de terminos indexados
        indiceDocs.erase(itIndiceDocs);
    }
    return eliminado;
}

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