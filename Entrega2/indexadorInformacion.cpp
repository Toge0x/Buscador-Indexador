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
    // A continuaci�n se mostrar�an todos los elementos de p.posTerm ("posicion TAB posicion TAB ... posicion, es decir nunca finalizar� en un TAB"): s << "\t" << posicion;
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
    fechaModificacion = id.fechaModificacion;   // se acepta
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

    }
    return *this;
}