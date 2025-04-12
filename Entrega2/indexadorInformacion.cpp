#include "indexadorInformacion.h"


/*
    IMPLEMENTACIÓN DE LA CLASE InformacionTermino
*/

ostream& operator<<(ostream& s, const InformacionTermino& it){
    s << "Frecuencia total: " << it.ftc << "\tfd: " << it.l_docs.size();
    // A continuaci�n se mostrar�an todos los elementos de p.l_docs: s << "\tId.Doc: " << idDoc << "\t" << InfTermDoc;
    for(const auto& par : it.l_docs){
        s << "\tId.doc: " << par.first << "\t" << par.second;
    }
    return s;
}

// evitar escribir codigo innecesario, copia campo a campo
InformacionTermino::InformacionTermino() : ftc(0){}
InformacionTermino::InformacionTermino(const InformacionTermino& it) = default;
InformacionTermino::~InformacionTermino() = default;
InformacionTermino& InformacionTermino::operator=(const InformacionTermino& it) = default;

// metodos para manejar la parte privada

void InformacionTermino::addFT(){
    ftc++;
}

void InformacionTermino::addTerminoEnDocumento(int idDoc, int pos){
    addFT();  // aumentamos la frecuencia del termino en la coleccion total de documentos
    InfTermDoc& infoDoc = l_docs[idDoc];    // obtenemos la informacion del termino en documento = idDoc
    infoDoc.addFT();                        // incrementamos su frecuencia
    infoDoc.addPositionTerm(pos);           // añadimos dónde aparece
}

int InformacionTermino::getFT() const{
    return ftc;
}

int InformacionTermino::getNumDocs() const{
    return l_docs.size();
}

const unordered_map<int, InfTermDoc>& InformacionTermino::getLDocs() const{
    return l_docs;
}

/*
    IMPLEMENTACIÓN DE LA CLASE InfTermDoc
*/

ostream& operator<<(ostream& s, const InfTermDoc& itd) {
    s << "ft: " << itd.ft;
    // A continuaci�n se mostrar�an todos los elementos de p.posTerm 
    // ("posicion TAB posicion TAB ... posicion, es decir nunca finalizar� en un TAB"): 
    // s << "\t" << posicion;
    int i = 0;
    for(const auto& valor : itd.posTerm){
        if(i == itd.posTerm.size() - 1){    // último elemento de la lista
            s << valor;
        }else{
            s << valor << "\t"; i++;
        }
    }
    return s;
}

// evitar escribir codigo innecesario
InfTermDoc::InfTermDoc() : ft(0){}
InfTermDoc::InfTermDoc(const InfTermDoc &itd) = default;
InfTermDoc::~InfTermDoc() = default;
InfTermDoc &InfTermDoc::operator=(const InfTermDoc &itd) = default;

// metodos para controlar la parte privada

void InfTermDoc::addFT(){
    ft++;
}

void InfTermDoc::addPositionTerm(int pos){
    posTerm.push_back(pos);
}

int InfTermDoc::getFT() const{
    return ft;
}

const list<int>& InfTermDoc::getPositionTerm() const{
    return posTerm;
}

/*
    IMPLEMENTACIÓN DE LA CLASE InfDoc
*/

ostream& operator<<(ostream& s, const InfDoc& id) {
    s << "idDoc: " << id.idDoc << "\tnumPal: " << id.numPal << "\tnumPalSinParada: " << id.numPalSinParada << "\tnumPalDiferentes: " << id.numPalDiferentes << "\ttamBytes: " << id.tamBytes;
    return s;
}

// """"""""
InfDoc::InfDoc() : idDoc(0), numPal(0), numPalSinParada(0), numPalDiferentes(0), tamBytes(0), fechaModificacion(time(nullptr)) {}
InfDoc::InfDoc(const InfDoc& id) = default;
InfDoc::~InfDoc() = default;
InfDoc& InfDoc::operator=(const InfDoc& id) = default;

// controlar los atributos privados

int InfDoc::getIdDoc() const{return idDoc;}
int InfDoc::getNumPal() const{return numPal;}
int InfDoc::getNumPalSinParada() const{return numPalSinParada;}
int InfDoc::getNumPalDiferentes() const{return numPalDiferentes;}
int InfDoc::getTamBytes() const{return tamBytes;}
time_t InfDoc::getFechaModificacion() const{return fechaModificacion;}

void InfDoc::setIdDoc(int id){idDoc = id;}
void InfDoc::setTamBytes(int bytes){tamBytes = bytes;}
void InfDoc::setFechaModificacion(const time_t& fecha){fechaModificacion = fecha;}
void InfDoc::setNumPal(int pal){numPal = pal;}
void InfDoc::setNumPalSinParada(int pal){numPalSinParada = pal;}
void InfDoc::setNumPalDifernetes(int pal){numPalDiferentes = pal;}

void InfDoc::addNumPal(){numPal++;}
void InfDoc::addNumPalSinParada(){numPalSinParada++;}
void InfDoc::addNumPalDiferentes(){numPalDiferentes++;}

/*
    IMPLEMENTACIÓN DE LA CLASE InfColeccionDocs
*/

ostream& operator<<(ostream& s, const InfColeccionDocs& icd){
    s << "numDocs: " << icd.numDocs << "\tnumTotalPal: " << icd.numTotalPal << "\tnumTotalPalSinParada: " << icd.numTotalPalSinParada << "\tnumTotalPalDiferentes: " << icd.numTotalPalDiferentes << "\ttamBytes: " << icd.tamBytes;
    return s;
}

InfColeccionDocs::InfColeccionDocs() : numDocs(0), numTotalPal(0), numTotalPalSinParada(0), numTotalPalDiferentes(0), tamBytes(0){}
InfColeccionDocs::InfColeccionDocs(const InfColeccionDocs& icd) = default;
InfColeccionDocs::~InfColeccionDocs() = default;
InfColeccionDocs& InfColeccionDocs::operator=(const InfColeccionDocs& icd) = default;

// metodos para controlar la parte privada


void InfColeccionDocs::setNumDocs(int docs){
    numDocs = docs;
}

void InfColeccionDocs::setTotalPalabras(int pal){
    numTotalPal = pal;
}

void InfColeccionDocs::setPalabrasSinParada(int pal){
    numTotalPalSinParada = pal;
}

void InfColeccionDocs::setPalabrasDiferentes(int pal){
    numTotalPalDiferentes = pal;
}

void InfColeccionDocs::setBytes(int bytes){
    tamBytes = bytes;
}

int InfColeccionDocs::getNumDocs() const{return numDocs;}
int InfColeccionDocs::getNumTotalPal() const{return numTotalPal;}
int InfColeccionDocs::getNumTotalPalSinParada() const{return numTotalPalSinParada;}
int InfColeccionDocs::getNumTotalPalDiferentes() const{return numTotalPalDiferentes;}
int InfColeccionDocs::getTamBytes() const{return tamBytes;}

/*
    IMPLEMENTACIÓN DE LA CLASE InformacionTerminoPregunta
*/

ostream& operator<<(ostream& s, const InformacionTerminoPregunta& itp) {
    s << "ft: " << itp.ft;
    // A continuación se mostrarían todos los elementos de p.posTerm
    // ("posicion TAB posicion TAB ... posicion, es decir nunca finalizará en un TAB"): 
    // s << "\t" << posicion;
    int i = 0;
    for(const auto& entero : itp.posTerm){
        if(i == itp.posTerm.size() - 1){
            s << entero;
        }else{
            s << entero << "\t";
        }
    }
    return s;
}

InformacionTerminoPregunta::InformacionTerminoPregunta() : ft(0){}
InformacionTerminoPregunta::InformacionTerminoPregunta(const InformacionTerminoPregunta& itp) = default;
InformacionTerminoPregunta::~InformacionTerminoPregunta() = default;
InformacionTerminoPregunta& InformacionTerminoPregunta::operator=(const InformacionTerminoPregunta& itp) = default;

// metodos para controlar la parte privada

void InformacionTerminoPregunta::addFT() {ft++;}
void InformacionTerminoPregunta::setFT(int n) {ft = n;}

void InformacionTerminoPregunta::addItemToPos(int pos){
    this->posTerm.push_back(pos);
}

int InformacionTerminoPregunta::getFT() const {return ft;}
list<int> InformacionTerminoPregunta::getSortedPositions() const{
    list<int> ordenadas = posTerm;
    ordenadas.sort();
    return ordenadas;
}

/*
    IMPLEMENTACIÓN DE LA CLASE InformacionPregunta
*/

ostream& operator<<(ostream& s, const InformacionPregunta& ip){
    s << "numTotalPal: " << ip.numTotalPal << "\tnumTotalPalSinParada: "<< ip.numTotalPalSinParada << "\tnumTotalPalDiferentes: " << ip.numTotalPalDiferentes;
    return s;
}

InformacionPregunta::InformacionPregunta() : numTotalPal(0), numTotalPalSinParada(0), numTotalPalDiferentes(0){}
InformacionPregunta::InformacionPregunta(const InformacionPregunta& ip) = default;
InformacionPregunta::~InformacionPregunta() = default;
InformacionPregunta& InformacionPregunta::operator=(const InformacionPregunta& ip) = default;

// metodos para controlar la parte privada

int InformacionPregunta::getNumTotalPal() const{return numTotalPal;}
int InformacionPregunta::getNumTotalPalSinParada() const{return numTotalPalSinParada;}
int InformacionPregunta::getNumTotalPalDiferentes() const{return numTotalPalDiferentes;}
const unordered_map<string, InformacionTerminoPregunta>& InformacionPregunta::getTerminos() const{return terminos;}

void InformacionPregunta::addTermino(const string& termino, int pos, bool esStopword) {
    numTotalPal++;   // aumentamos el numero de palabras de la pregunta

    if(!esStopword){                // si no es stopword la tratamos
        numTotalPalSinParada++;
        auto it = terminos.find(termino);   
        if(it == terminos.end()){               // si no esta el termino
            InformacionTerminoPregunta info;    // creamos el termino con frecuencia 1 y la pos del termino
            info.addFT();                       // ft++
            info.addItemToPos(pos);
            terminos[termino] = info;
            numTotalPalDiferentes++;
        }else{                              // si ya existe en la pregunta aumentamos frecuencia y pos
            it->second.addFT();             // first -> termino, second -> ITP (ft, pos)
            it->second.addItemToPos(pos);
        }
    }
}

void InformacionPregunta::incrementarNPalabras(const int n){
    numTotalPal += n;
}

void InformacionPregunta::incrementarNPalabrasSinParada(const int n){
    numTotalPalSinParada += n;
}

void InformacionPregunta::incrementarNPalabrasDiferentes(const int n){
    numTotalPalDiferentes += n;
}