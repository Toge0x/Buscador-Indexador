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
    docsOrdenados = priority_queue<ResultadoRI>();          // vaciamos la cola
    return Buscar(numDocumentos, 0);
}

vector<ResultadoRI> Buscador::calculoDFR(const int& numPregunta) {
    vector<ResultadoRI> resultados;

    return resultados;
}

vector<ResultadoRI> Buscador::calculoBM25(const int& numPregunta) {
    vector<ResultadoRI> resultados;

    double avgdl = static_cast<double>(informacionColeccionDocs.getNumTotalPalSinParada()) / informacionColeccionDocs.getNumDocs();
    int N = informacionColeccionDocs.getNumDocs();

    for(const auto& [ruta, doc] : indiceDocs){
        double similitud = 0.0;
        for(const auto& [termino, infTermPreg] : indicePregunta){
            auto itTerm = indice.find(termino);
            if(itTerm != indice.end()){
                const auto& termDocs = itTerm->second.getLDocs();
                auto itDocTerm = termDocs.find(doc.getIdDoc());
                if (itDocTerm != termDocs.end()) {
                    int ftd = itDocTerm->second.getFT();
                    int df = termDocs.size();
                    double idf = log2((N - df + 0.5) / (df + 0.5));
                    double score = idf * (ftd * (k1 + 1)) / (ftd + k1 * (1 - b + b * (doc.getNumPalSinParada() / avgdl)));

                    similitud += score;
                }
            }
        }
        if(similitud > 0.0){
            resultados.emplace_back(similitud, doc.getIdDoc(), numPregunta);
        }
    }
    return resultados;
}

bool Buscador::Buscar(const int& numDocumentos, const int& numPregunta){
    string pregunta;
    if(!DevuelvePregunta(pregunta)) return false;

    vector<ResultadoRI> resultados;

    if(formSimilitud == 0)
        resultados = calculoDFR(numPregunta);
    else if(formSimilitud == 1)
        resultados = calculoBM25(numPregunta);
    else
        return false;

    sort(resultados.begin(), resultados.end());
    resultados.resize(std::min(static_cast<size_t>(numDocumentos), resultados.size()));

    for(const auto& res : resultados){
        docsOrdenados.push(res);
    }

    return true;
}

bool Buscador::Buscar(const string& dirPreguntas, const int& numDocumentos, const int& numPregInicio, const int& numPregFin){

}

string Buscador::RecuperarNombreDocumento(const int& idDoc) const{
    for(const auto& [ruta, info] : indiceDocs){
        if(info.getIdDoc() == idDoc){
            string::size_type lastPos = ruta.find_last_of("/\\");   // hasta la barra antes del archivo
            if (lastPos == string::npos) lastPos = 0;
            else lastPos++;                                         // para quitar la barra de antes del nombre

            string::size_type pos = ruta.find_first_of(".", lastPos);   // desde lastPos hasta el . de la extension
            return ruta.substr(lastPos, pos - lastPos);
        }
    }
    return "";
}

void Buscador::ImprimirResultadoBusqueda(const int& numDocumentos) const{
    priority_queue<ResultadoRI> documentos;
    documentos = docsOrdenados;
    string resultado;
    int numPreg;

    while(!documentos.empty()){
        numPreg = documentos.top().NumPregunta();

        for(int i = 0; i < numDocumentos; i++){
            if(documentos.empty() || documentos.top().NumPregunta() != numPreg)
                break;

            int numPregunta = documentos.top().NumPregunta();
            long int id = documentos.top().IdDoc();
            double sim = documentos.top().VSimilitud();

            resultado += to_string(numPregunta);                        // obtenemos la pregunta
            resultado += (formSimilitud == 0 ? " DFR " : " BM25 ");     // obtenemos el tipo de modelo
            resultado += RecuperarNombreDocumento(id) + ' ';            // obtenemos el nombre del documento
            resultado += to_string(i) + ' ';                            // obtenemos el num documento
            resultado += to_string(sim) + ' ';                          // obtenemos el valor de similitud

            if(numPregunta == 0){
                string preg;
                if(DevuelvePregunta(preg))
                    resultado += preg + '\n';
                else
                    resultado += '\n';
            }else{
                resultado += "ConjuntoDePreguntas\n";
            }
            documentos.pop();
        }
    }
    cout << resultado;
}

bool Buscador::ImprimirResultadoBusqueda(const int& numDocumentos, const string& nombreFichero) const{

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

