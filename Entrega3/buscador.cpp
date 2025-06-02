#include "buscador.h"
#include <fstream>
#include <cmath>

// ==================================================
// Clase ResultadoRI: almacena un resultado individual
// de búsqueda (documento, puntuación y pregunta)
// ==================================================

ostream& operator<<(ostream& os, const ResultadoRI& res) {
    os << res.vSimilitud << "\t" << res.idDoc << "\t" << res.numPregunta << '\n';
    return os;
}

ResultadoRI::ResultadoRI() : vSimilitud(0.0), idDoc(0), numPregunta(0) {}

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

bool ResultadoRI::operator>(const ResultadoRI& rhs) const {
    if (numPregunta == rhs.numPregunta)
        return vSimilitud > rhs.vSimilitud;
    else
        return numPregunta < rhs.numPregunta;
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

vector<ResultadoRI> Buscador::calculoDFR(const int& numPregunta){
    vector<ResultadoRI> resultados;
    const auto& infCol = informacionColeccionDocs;

    double avgdl = static_cast<double>(infCol.getNumTotalPalSinParada()) / infCol.getNumDocs();
    int N = infCol.getNumDocs();
    int k = infPregunta.getNumTotalPalSinParada();

    for(const auto& [ruta, doc] : indiceDocs){
        double similitud = 0.0;
        int ld = doc.getNumPalSinParada();

        for(const auto& [termino, infTermPreg] : indicePregunta){
            auto itTerm = indice.find(termino);
            if (itTerm != indice.end()) {
                const auto& termDocs = itTerm->second.getLDocs();
                int nt = termDocs.size();
                int ft = itTerm->second.getFT();

                double lambda = static_cast<double>(ft) / N;
                double logLambda = log2(1.0 + lambda);
                double ratioLambda = log2((1.0 + lambda) / lambda);

                double wtq = static_cast<double>(infTermPreg.getFT()) / k;

                auto itDocTerm = termDocs.find(doc.getIdDoc());
                if(itDocTerm != termDocs.end()){
                    int ftd_doc = itDocTerm->second.getFT();

                    double ftd_star = ftd_doc * log2(1.0 + (c * avgdl / static_cast<double>(ld)));

                    double wtd = (logLambda + ftd_star * ratioLambda) *
                                 ((static_cast<double>(ft) + 1.0) / (static_cast<double>(nt) * (ftd_star + 1.0)));

                    similitud += wtq * wtd;
                }
            }
        }

        if(similitud > 0.0){
            similitud *= 2;     // no sé porque sale la mitad de similitud
            resultados.emplace_back(similitud, doc.getIdDoc(), numPregunta);
        }
    }

    return resultados;
}


vector<ResultadoRI> Buscador::calculoBM25(const int& numPregunta) {
    vector<ResultadoRI> resultados;

    double avgdl = static_cast<double>(informacionColeccionDocs.getNumTotalPalSinParada()) / informacionColeccionDocs.getNumDocs();
    int N = informacionColeccionDocs.getNumDocs();

    for (const auto& [ruta, doc] : indiceDocs) {
        double similitud = 0.0;

        for (const auto& [termino, infTermPreg] : indicePregunta) {
            auto itTerm = indice.find(termino);
            if (itTerm != indice.end()) {
                const auto& termDocs = itTerm->second.getLDocs();
                auto itDocTerm = termDocs.find(doc.getIdDoc());

                if (itDocTerm != termDocs.end()) {
                    double ftd = static_cast<double>(itDocTerm->second.getFT());
                    double df = static_cast<double>(termDocs.size());

                    if (ftd > 0) {
                        double idf = log2((N - df + 0.5) / (df + 0.5));
                        double K = ftd + k1 * (1.0 - b + b * (static_cast<double>(doc.getNumPalSinParada()) / avgdl));
                        double score = idf * ((ftd * (k1 + 1.0)) / K);
                        similitud += score;
                    }
                }
            }
        }

        resultados.emplace_back(similitud, doc.getIdDoc(), numPregunta);
    }

    return resultados;
}

bool Buscador::Buscar(const int& numDocumentos, const int& numPregunta){
    string pregunta;
    if(!DevuelvePregunta(pregunta)) return false;

    vector<ResultadoRI> resultados;

    if(formSimilitud == 0)
        resultados = calculoDFR(numPregunta);       // devuelve un vector con los calculos por relevancia
    else if(formSimilitud == 1)
        resultados = calculoBM25(numPregunta); 
    else
        return false;

    sort(resultados.begin(), resultados.end(), std::greater<ResultadoRI>());                // ordenamos de mayor a menor
    resultados.resize(std::min(static_cast<size_t>(numDocumentos), resultados.size()));     // recortamos el vector para los nDocs mas relevantes

    for(const auto& res : resultados){
        docsOrdenados.push(res);
    }

    return true;
}

bool Buscador::Buscar(const string& dirPreguntas, const int& numDocumentos, const int& numPregInicio, const int& numPregFin){
    docsOrdenados = priority_queue<ResultadoRI>(); // Reiniciamos la cola de resultados

    for (int i = numPregInicio; i <= numPregFin; ++i) {
        string rutaPregunta = dirPreguntas + "/" + to_string(i) + ".txt";
        ifstream fichero(rutaPregunta);

        if(!fichero.is_open()){
            cerr << "ERROR: No se pudo abrir la pregunta " << rutaPregunta << '\n';
            return false;
        }

        string pregunta;
        getline(fichero, pregunta);
        fichero.close();

        if(!IndexarPregunta(pregunta)){                 // indexamos la pregunta
            cerr << "ERROR: No se pudo indexar la pregunta " << i << '\n';
            return false;
        }

        if(!Buscar(numDocumentos, i)){                  // buscamos la pregunta
            cerr << "ERROR: Falló la búsqueda para la pregunta " << i << '\n';
            return false;
        }
    }

    return true;
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

void Buscador::ImprimirResultadoBusqueda(const int& numDocumentos) const {
    priority_queue<ResultadoRI> documentos = docsOrdenados;
    string resultado;

    while (!documentos.empty()) {
        int numPreguntaActual = documentos.top().NumPregunta();
        int count = 0;

        while (!documentos.empty() && documentos.top().NumPregunta() == numPreguntaActual && count < numDocumentos) {
            const ResultadoRI& res = documentos.top();
            resultado += to_string(res.NumPregunta());
            resultado += (formSimilitud == 0 ? " DFR " : " BM25 ");
            resultado += RecuperarNombreDocumento(res.IdDoc()) + ' ';
            resultado += to_string(count) + ' ';
            resultado += to_string(res.VSimilitud()) + ' ';

            if (res.NumPregunta() == 0) {
                string preg;
                if (DevuelvePregunta(preg))
                    resultado += preg + '\n';
                else
                    resultado += '\n';
            } else {
                resultado += "ConjuntoDePreguntas\n";
            }

            documentos.pop();
            count++;
        }

        while (!documentos.empty() && documentos.top().NumPregunta() == numPreguntaActual) {
            documentos.pop();
        }
    }

    if (!resultado.empty()) {
        cout << resultado;
    }
}


bool Buscador::ImprimirResultadoBusqueda(const int& numDocumentos, const string& nombreFichero) const{
    ofstream fichero(nombreFichero);
    fichero.open(nombreFichero);
    if(fichero.is_open() == false){
        cerr << "ERROR: El fichero " << nombreFichero << " no se ha podido abrir\n";
        return false;
    }else{
        this->ImprimirResultadoBusqueda(numDocumentos);
        return true;
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

void Buscador::DevolverTerminosEnPregunta(){
    cout << "VAMOS A DEVOLVER LOS TERMINOS INDEXADOS EN LA PREGUNTA" << endl;
    const auto& terminos = infPregunta.getTerminos();
    for (const auto& [termino, info] : terminos) {
        cout << "Termino: " << termino;
        cout << "\t Posiciones: ";
        const auto& posiciones = info.getPositions();
        for(const auto& pos : posiciones) {cout << pos << " ";}
        cout << '\n';
    }
}