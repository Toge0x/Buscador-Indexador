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

InformacionTermino::InformacionTermino(){		// Inicializa ftc = 0
    ftc = 0;
}

InformacionTermino::InformacionTermino(const InformacionTermino &it){
    ftc = it.ftc;
    l_docs = it.l_docs;
}

InformacionTermino::~InformacionTermino(){// Pone ftc = 0 y vacía l_docs
    ftc = 0;
    l_docs.clear();
}

InformacionTermino& InformacionTermino::operator=(const InformacionTermino &t){    // Operador asignación sobreescrito de la clase InformacionTermino
    if(this != &t){
        ftc = t.ftc;
        l_docs = t.l_docs;
    }
    return *this;
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
InfTermDoc::InfTermDoc(const InfTermDoc &itd){  // Constructor de copia
    ft = itd.ft;
    posTerm = itd.posTerm;
}

InfTermDoc::InfTermDoc(){   	// Inicializa ft = 0
    ft = 0;
}

InfTermDoc::~InfTermDoc(){		// Pone ft = 0 
    ft = 0;
}

InfTermDoc &InfTermDoc::operator=(const InfTermDoc &itd){ // Operador asignación sobreescrito de la clase InfTermDoc
    if(this != &itd){
        ft = itd.ft;
        posTerm = itd.posTerm;
    }
    return *this;
}

/*
    IMPLEMENTACIÓN DE LA CLASE InfTermDoc
*/

int AUTO_INCREMENTAL_ID_DOC = 0;

ostream& operator<<(ostream& s, const InfDoc& id) {
    s << "idDoc: " << id.idDoc << "\tnumPal: " << id.numPal << "\tnumPalSinParada: " << id.numPalSinParada << "\tnumPalDiferentes: " << id.numPalDiferentes << "\ttamBytes: " << id.tamBytes;
    return s;
}

InfDoc::InfDoc(const InfDoc &id){
    idDoc = id.idDoc;
    numPal = id.numPal;
    numPalDiferentes = id.numPalDiferentes;
    tamBytes = id.tamBytes;
    fechaModificacion = id.fechaModificacion;
}

InfDoc::InfDoc(){
    idDoc = ++AUTO_INCREMENTAL_ID_DOC;
    numPal = numPalSinParada = numPalDiferentes = tamBytes = 0;
}

InfDoc::~InfDoc(){
    AUTO_INCREMENTAL_ID_DOC--;
    idDoc = numPal = numPalSinParada = numPalDiferentes = tamBytes = 0;
}

InfDoc &InfDoc::operator=(const InfDoc &id){
    if(this != &id){
        idDoc = id.idDoc;
        numPal = id.numPal;
        numPalSinParada = id.numPalSinParada;
        numPalDiferentes = id.numPalDiferentes;
        tamBytes = id.tamBytes;
        fechaModificacion = id.fechaModificacion;
    }
    return *this;
}

/*
    IMPLEMENTACIÓN DE LA CLASE InfColeccionDocs
*/

ostream& operator<<(ostream& s, const InfColeccionDocs& icd){
    s << "numDocs: " << icd.numDocs << "\tnumTotalPal: " << icd.numTotalPal << "\tnumTotalPalSinParada: " << icd.numTotalPalSinParada << "\tnumTotalPalDiferentes: " << icd.numTotalPalDiferentes << "\ttamBytes: " << icd.tamBytes;
    return s;
}

InfColeccionDocs::InfColeccionDocs(const InfColeccionDocs &icd){
    numDocs = icd.numDocs;
    numTotalPal = icd.numTotalPal;
    numTotalPalSinParada = icd.numTotalPalSinParada;
    numTotalPalDiferentes = icd.numTotalPalDiferentes;
    tamBytes = icd.tamBytes;
}

InfColeccionDocs::InfColeccionDocs(){
    numDocs = numTotalPal = numTotalPalSinParada = numTotalPalDiferentes = tamBytes = 0;
}

InfColeccionDocs::~InfColeccionDocs(){  // sin memoria dinámica

}

InfColeccionDocs &InfColeccionDocs::operator=(const InfColeccionDocs &icd){
    if(this != &icd){
        numDocs = icd.numDocs;
        numTotalPal = icd.numTotalPal;
        numTotalPalSinParada = icd.numTotalPalSinParada;
        numTotalPalDiferentes = icd.numTotalPalDiferentes;
        tamBytes = icd.tamBytes;
    }
    return *this;
}

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

InformacionTerminoPregunta::InformacionTerminoPregunta(const InformacionTerminoPregunta &itp){
    ft = itp.ft;
    posTerm = itp.posTerm;
}

InformacionTerminoPregunta::InformacionTerminoPregunta(){
    ft = 0;
}

InformacionTerminoPregunta::~InformacionTerminoPregunta(){
    ft = 0;
    posTerm.clear();
}

InformacionTerminoPregunta &InformacionTerminoPregunta::operator=(const InformacionTerminoPregunta &itp){
    if(this != &itp){
        ft = itp.ft;
        posTerm = itp.posTerm;
    }
    return *this;
}

/*
    IMPLEMENTACIÓN DE LA CLASE InformacionPregunta
*/

ostream& operator<<(ostream& s, const InformacionPregunta& ip){
    s << "numTotalPal: " << ip.numTotalPal << "\tnumTotalPalSinParada: "<< ip.numTotalPalSinParada << "\tnumTotalPalDiferentes: " << ip.numTotalPalDiferentes;
    return s;
}

InformacionPregunta::InformacionPregunta(const InformacionPregunta &ip){
    numTotalPal = ip.numTotalPal;
    numTotalPalSinParada = ip.numTotalPalSinParada;
    numTotalPalDiferentes = ip.numTotalPalDiferentes;
}

InformacionPregunta::InformacionPregunta(){
    numTotalPal = numTotalPalSinParada = numTotalPalDiferentes = 0;
}

InformacionPregunta::~InformacionPregunta(){    // no hay memoria dinámica
}

InformacionPregunta &InformacionPregunta::operator=(const InformacionPregunta &ip){
    if(this != &ip){
        numTotalPal = ip.numTotalPal;
        numTotalPalSinParada = ip.numTotalPalSinParada;
        numTotalPalDiferentes = ip.numTotalPalDiferentes;
    }
    return *this;
}