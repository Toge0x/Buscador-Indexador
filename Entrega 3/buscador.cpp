#include "buscador.h"
#include <cmath>

// ==================================================
// Clase ResultadoRI: almacena un resultado individual
// de búsqueda (documento, puntuación y pregunta)
// ==================================================

ostream& operator<<(ostream& os, const ResultadoRI& res) {
    os << res.vSimilitud << "\t" << res.idDoc << "\t" << res.numPregunta << '\n';
    return os;
}

ResultadoRI::ResultadoRI(const double& kvSimilitud, const long int& kidDoc, const int& np)
    : vSimilitud(kvSimilitud), idDoc(kidDoc), numPregunta(np) {}

double ResultadoRI::VSimilitud() const {
    return vSimilitud;
}

long int ResultadoRI::IdDoc() const {
    return idDoc;
}

int ResultadoRI::NumPregunta() const {
    return numPregunta;
}

bool ResultadoRI::operator<(const ResultadoRI& rhs) const {
    if (numPregunta == rhs.numPregunta)
        return vSimilitud < rhs.vSimilitud;
    else
        return numPregunta > rhs.numPregunta;
}

// ==================================================
// Clase Buscador: hereda de IndexadorHash y permite
// realizar búsquedas usando modelos DFR o BM25
// ==================================================

ostream& operator<<(ostream& os, const Buscador& b){
    string preg;
    os << "Buscador: " << '\n';
    if(b.DevuelvePregunta(preg)){
        os << "\tPregunta indexada: " << preg << '\n';
    }else{
        os << "\tNo hay ninguna pregunta indexada" << '\n';
    }
    os << "\tDatos del indexador: " << '\n' << (IndexadorHash) b;
    return os;
}

Buscador::Buscador(const string& directorioIndexacion, const int& f) : IndexadorHash(directorioIndexacion), formSimilitud(f){
    c = 2;
    k1 = 1.2;
    b = 0.75;
}

Buscador::Buscador(const Buscador& b) : IndexadorHash(b), 
    formSimilitud(b.formSimilitud), c(b.c), k1(b.k1), b(b.b) {}

Buscador::~Buscador(){
    priority_queue<ResultadoRI> vacia;
    docsOrdenados = vacia;
}

Buscador& Buscador::operator=(const Buscador& busc) {
    if (this != &busc) {
        IndexadorHash::operator=(busc);
        formSimilitud = busc.formSimilitud;
        c = busc.c;
        k1 = busc.k1;
        b = busc.b;
    }
    return *this;
}

bool Buscador::Buscar(const int& numDocumentos){
    if(indicePregunta.empty()){
        cout << "No hay pregunta indexada" << '\n';
        return false;
    }

    docsOrdenados = priority_queue<ResultadoRI>();      // limpiar resultados anteriores
    if(formSimilitud == 0){     // DFR
        double avgdl = informacionColeccionDocs.getNumTotalPalSinParada() * 1.0 / informacionColeccionDocs.getNumDocs();    // media de palabras en los documentos
        int N = informacionColeccionDocs.getNumDocs();              // número total de documentos   
        int totalPalabrasPregunta = infPregunta.getNumTotalPalSinParada();  // total de palabras en la pregunta

        for (const auto& [termino, infoPregunta] : indicePregunta) {
            if (indice.find(termino) == indice.end()) continue; // término no en colección

            int ftc = indice[termino].getFT();              // frecuencia total en colección
            int nt = indice[termino].getNumDocs();          // documentos que contienen t
            if (nt == 0) continue;

            double wiq = static_cast<double>(infoPregunta.getFT()) / totalPalabrasPregunta;

            for (const auto& [nombreDoc, infoDoc] : indiceDocs) {
                int idDoc = infoDoc.getIdDoc();  // identificador interno
                int ftd = indice[termino].getFT(); // frecuencia de término en doc actual
                if (ftd == 0) continue;

                int longitudDoc = infoDoc.getNumPal();
                double normalizador = c * avgdl / longitudDoc;
                double log1 = log2(1 + (static_cast<double>(ftd) / normalizador));
                double log2parte = log2(1 + static_cast<double>(N) / nt);
                double wid = log1 * (ftd * log2parte);

                similitud[idDoc] += wiq * wid;
            }
        }

        // Insertar resultados en la cola ordenada
        for (const auto& [idDoc, score] : similitud) {
            docsOrdenados.push(ResultadoRI(score, idDoc, 0));
        }

        return true;
    }else if(formSimilitud == 1){   // BM25

    }else{
        cout << "Error: formula de similitud no válida" << '\n';
        return false;
    }

}

int Buscador::DevolverFormulaSimilitud() const {
    return formSimilitud;
}

bool Buscador::CambiarFormulaSimilitud(const int& f) {
    if (f == 0 || f == 1) {
        formSimilitud = f;
        return true;
    }
    return false;
}

void Buscador::CambiarParametrosDFR(const double& kc) {
    c = kc;
}

double Buscador::DevolverParametrosDFR() const {
    return c;
}

void Buscador::CambiarParametrosBM25(const double& kk1, const double& kb) {
    k1 = kk1;
    b = kb;
}

void Buscador::DevolverParametrosBM25(double& kk1, double& kb) const {
    kk1 = k1;
    kb = b;
}

