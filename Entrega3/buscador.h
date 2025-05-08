#ifndef _BUSCADOR_RESULTADORI_H_
#define _BUSCADOR_RESULTADORI_H_

#include "indexadorHash.h"
#include <queue>
#include <string>
#include <iostream>
using namespace std;

// ==================================================
// Clase ResultadoRI: almacena un resultado individual
// de búsqueda (documento, puntuación y pregunta)
// ==================================================

class ResultadoRI {
    friend ostream& operator<<(ostream& os, const ResultadoRI& res);

public:
    // Constructor: recibe la similitud, ID de documento y número de pregunta
    ResultadoRI(const double& kvSimilitud, const long int& kidDoc, const int& np);

    // Devuelve el valor de similitud del documento con la pregunta
    double VSimilitud() const;

    // Devuelve el identificador del documento
    long int IdDoc() const;

    // Devuelve el número de la pregunta asociada al resultado
    int NumPregunta() const;

    // Operador de comparación para ordenación en priority_queue
    // Ordena de forma decreciente según la similitud (si son de la misma pregunta)
    // o por número de pregunta creciente (para mantener agrupación)
    bool operator<(const ResultadoRI& rhs) const;

private:
    double vSimilitud;  // Valor de similitud
    long int idDoc;     // Identificador del documento
    int numPregunta;    // Número de la pregunta asociada
};

// ==================================================
// Clase Buscador: hereda de IndexadorHash y permite
// realizar búsquedas usando modelos DFR o BM25
// ==================================================

class Buscador : public IndexadorHash {
    friend ostream& operator<<(ostream& os, const Buscador& b);
    friend class IndexadorHash;
    public:
        // Constructor para inicializar Buscador a partir de la indexación generada previamente
        // Se inicializa con el valor de similitud f y los valores por defecto: c = 2; k1 = 1.2; b = 0.75
        Buscador(const string& directorioIndexacion, const int& f);

        // Constructor de copia
        Buscador(const Buscador&);

        // Destructor
        ~Buscador();

        // Operador de asignación
        Buscador& operator=(const Buscador&);

        // Realiza la búsqueda sobre la pregunta actual indexada
        // Guarda los resultados más relevantes (hasta numDocumentos) en docsOrdenados
        bool Buscar(const int& numDocumentos = 99999);

        vector<ResultadoRI> calculoDFR(const int& numPregunta);

        vector<ResultadoRI> calculoBM25(const int& numPregunta);

        bool Buscar(const int& numDocumentos, const int& numPregunta);

        // Realiza la búsqueda sobre un conjunto de preguntas de un directorio
        // Se guarda el top numDocumentos por cada pregunta, entre numPregInicio y numPregFin
        bool Buscar(const string& dirPreguntas, const int& numDocumentos, const int& numPregInicio, const int& numPregFin);

        string RecuperarNombreDocumento(const int& idDoc) const;

        // Imprime por pantalla los resultados de la última búsqueda
        // Mostrará como máximo numDocumentos documentos por pregunta
        void ImprimirResultadoBusqueda(const int& numDocumentos = 99999) const;

        // Igual que la anterior, pero guarda la salida en un fichero
        // Devuelve false si no consigue crear el archivo
        bool ImprimirResultadoBusqueda(const int& numDocumentos, const string& nombreFichero) const;

        // Devuelve la fórmula de similitud actual (0 = DFR, 1 = BM25)
        int DevolverFormulaSimilitud() const;

        // Cambia la fórmula de similitud si el valor es válido (0 o 1)
        // Devuelve false si el valor no es válido
        bool CambiarFormulaSimilitud(const int& f);

        // Cambia el valor de la constante c del modelo DFR
        void CambiarParametrosDFR(const double& kc);

        // Devuelve el valor de la constante c del modelo DFR
        double DevolverParametrosDFR() const;

        // Cambia los valores de las constantes k1 y b del modelo BM25
        void CambiarParametrosBM25(const double& kk1, const double& kb);

        // Devuelve los valores de las constantes k1 y b del modelo BM25
        void DevolverParametrosBM25(double& kk1, double& kb) const;

    private:
        // Constructor privado prohibido: no se permite crear un Buscador sin indexación previa
        Buscador();

        // Cola de prioridad con los documentos más relevantes por pregunta
        priority_queue<ResultadoRI> docsOrdenados;

        // Fórmula de similitud actual (0 = DFR, 1 = BM25)
        int formSimilitud;

        // Constantes de los modelos de similitud
        double c;   // Constante DFR
        double k1;  // Constante BM25
        double b;   // Constante BM25
};

#endif // _BUSCADOR_RESULTADORI_H_
