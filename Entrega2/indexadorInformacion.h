#ifndef INDEXADOR_INFORMACION_H
#define INDEXADOR_INFORMACION_H

#include<iostream>
#include<unordered_map>
#include<list>
#include<time.h>
#include <algorithm> // para sort
using namespace std;

class InformacionTermino;
class InfTermDoc;
class InfDoc;
class InfColeccionDocs;
class InformacionTerminoPregunta;
class InformacionPregunta;

class InformacionTermino { 
    friend ostream& operator<<(ostream& s, const InformacionTermino& p);
public:
    InformacionTermino (const InformacionTermino &);
    InformacionTermino ();		// Inicializa ftc = 0
    ~InformacionTermino ();		// Pone ftc = 0 y vacia l_docs
    InformacionTermino & operator= (const InformacionTermino &);

    // A�adir cuantos metodos se consideren necesarios para manejar la parte privada de la clase
    void addFT();
    void addTerminoEnDocumento(int idDoc, int pos);
    int getFT() const;
    int getNumDocs() const;
    const unordered_map<int, InfTermDoc>& getLDocs() const;
private:
    int ftc;	// Frecuencia total del t?rmino en la coleccion
    unordered_map<int, InfTermDoc> l_docs; 
    // Tabla Hash que se accedera por el id del documento, devolviendo un objeto de la clase InfTermDoc que contiene 
    // toda la informacion de aparicion del termino en el documento
};

class InfTermDoc { 
    friend ostream& operator<<(ostream& s, const InfTermDoc& p);
public:
    InfTermDoc (const InfTermDoc &);
    InfTermDoc ();		// Inicializa ft = 0
    ~InfTermDoc ();		// Pone ft = 0 
    InfTermDoc & operator= (const InfTermDoc &);

    // A?adir cuantos m?todos se consideren necesarios para manejar la parte privada de la clase
    void addFT();
    void addPositionTerm(int pos);
    int getFT() const;
    const list<int>& getPositionTerm() const;
private:
    int ft;	// Frecuencia del t?rmino en el documento
    list<int> posTerm;	
    // Solo se almacenar? esta informaci?n si el campo privado del indexador almacenarPosTerm == true
    // Lista de n?meros de palabra en los que aparece el t?rmino en el documento. Los n?meros de palabra comenzar?n desde cero 
    //(la primera palabra del documento). Se numerar?n las palabras de parada. Estar? ordenada de menor a mayor posici?n. 
};

class InfDoc { 
    friend ostream& operator<<(ostream& s, const InfDoc& p);
public:
    InfDoc (const InfDoc &);
    InfDoc ();	
    ~InfDoc ();
    InfDoc & operator= (const InfDoc &);

    // A�adir cuantos metodos se consideren necesarios para manejar la parte privada de la clase 
    int getIdDoc() const;
    int getNumPal() const;
    int getNumPalSinParada() const;
    int getNumPalDiferentes() const;
    int getTamBytes() const;
    time_t getFechaModificacion() const;

    void setIdDoc(int id);
    void setTamBytes(int bytes);
    void setFechaModificacion(const time_t& fecha);
    void addNumPal();
    void addNumPalSinParada();
    void addNumPalDiferentes();

private:
    int idDoc;	
    // Identificador del documento. El primer documento indexado en la colecci?n ser? el identificador 1
    int numPal;	// N? total de palabras del documento
    int numPalSinParada;	// N? total de palabras sin stop-words del documento
    int numPalDiferentes;	
    // N? total de palabras diferentes que no sean stop-words (sin acumular la frecuencia de cada una de ellas)
    int tamBytes;	// Tama?o en bytes del documento
    // Atributo correspondiente a la fecha y hora (completa) de modificaci?n del documento. El tipo "Fecha/hora" lo elegir?/implementar? el alumno
    time_t fechaModificacion;
};

class InfColeccionDocs { 
    friend ostream& operator<<(ostream& s, const InfColeccionDocs& p);
public:
    InfColeccionDocs (const InfColeccionDocs &);
    InfColeccionDocs ();
    ~InfColeccionDocs ();
    InfColeccionDocs & operator= (const InfColeccionDocs &);

    // A�adir cuantos metodos se consideren necesarios para manejar la parte privada de la clase
    void addNumDocs();
    void addPalabras(int total, int sinParada, int diferentes);
    void addTamBytes(int bytes);

    int getNumDocs() const;
    int getNumTotalPal() const;
    int getNumTotalPalSinParada() const;
    int getNumTotalPalDiferentes() const;
    int getTamBytes() const;
private:
    int numDocs;	// N� total de documentos en la colecci?n
    int numTotalPal;	
    // N� total de palabras en la colecci?n 
    int numTotalPalSinParada;
    // N� total de palabras sin stop-words en la colecci?n 
    int numTotalPalDiferentes;	
    // N� total de palabras diferentes en la colecci?n que no sean stop-words (sin acumular la frecuencia de cada una de ellas)
    int tamBytes;	// Tama�o total en bytes de la coleccion
};

class InformacionTerminoPregunta { 
    friend ostream& operator<<(ostream& s, const InformacionTerminoPregunta& p);
public:
    InformacionTerminoPregunta (const InformacionTerminoPregunta &);
    InformacionTerminoPregunta ();
    ~InformacionTerminoPregunta ();
    InformacionTerminoPregunta & operator= (const InformacionTerminoPregunta &);

    // A?adir cuantos m?todos se consideren necesarios para manejar la parte privada de la clase
    void addFT();
    void addPosition(int pos);
    int getFT() const;
    list<int> getSortedPositions() const;
private:
    int ft;	// Frecuencia total del termino en la pregunta
    list<int> posTerm;	
    // Solo se almacenara esta informaci?on si el campo privado del indexador almacenarPosTerm == true
    // Lista de numeros de palabra en los que aparece el termino en la pregunta. 
    //Los n?meros de palabra comenzar?n desde cero (la primera palabra de la pregunta).
    //Se numerar?n las palabras de parada. Estar? ordenada de menor a mayor posici?n.
};

class InformacionPregunta { 
    friend ostream& operator<<(ostream& s, const InformacionPregunta& p);
public:
    InformacionPregunta (const InformacionPregunta &);
    InformacionPregunta ();	
    ~InformacionPregunta ();
    InformacionPregunta & operator= (const InformacionPregunta &);

    // A�adir cuantos metodos se consideren necesarios para manejar la parte privada de la clase
    void addTermino(const string& termino, int pos, bool esStopword);
    int getNumTotalPal() const;
    int getNumTotalPalSinParada() const;
    int getNumTotalPalDiferentes() const;
    const unordered_map<string, InformacionTerminoPregunta>& getTerminos() const;
    void InformacionPregunta::incrementarNPalabras(const int n);
    void InformacionPregunta::incrementarNPalabrasSinParada(const int n);
    void InformacionPregunta::incrementarNPalabrasDiferentes(const int n);
private:
    int numTotalPal;	
    // N� total de palabras en la pregunta
    int numTotalPalSinParada;
    // N� total de palabras sin stop-words en la pregunta
    int numTotalPalDiferentes;	
    // N� total de palabras diferentes en la pregunta que no sean stop-words
    // (sin acumular la frecuencia de cada una de ellas)
    unordered_map<string, InformacionTerminoPregunta> terminos; // hash map string -> infoterminopregunta (ft, pos)
};

#endif