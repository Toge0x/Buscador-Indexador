#include <iostream>  
#include <string> 
#include <list>  
#include <sys/resource.h> 
#include "tokenizador.h"
#include "indexadorHash.h"
#include "buscador.h"
 
using namespace std; 
 
double getcputime(void) {  
	struct timeval tim;         
	struct rusage ru;         
	getrusage(RUSAGE_SELF, &ru);         
	tim=ru.ru_utime;         
	double t=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;         
	tim=ru.ru_stime;         
	t+=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;         
	return t;  
} 

int
main() { 
	IndexadorHash b("./StopWordsEspanyol.txt", ". ,:", false, false, "./indicePruebaEspanyol", 1, false); 
	b.Indexar("ficherosTimes.txt"); 
	b.GuardarIndexacion(); 
 
	Buscador a("./indicePruebaEspanyol", 1); 
 
	a.IndexarPregunta("KENNEDY  ADMINISTRATION  PRESSURE  ON  NGO  DINH  DIEM  TO STOP SUPPRESSING THE BUDDHISTS . ");
	
	time_t inicioB, finB; 
	time(&inicioB); 
	double aaB = getcputime(); 
	a.Buscar("./CorpusTime/Preguntas/", 423, 1, 83);
	a.ImprimirResultadoBusqueda(423, "fich_salida_buscador_alumno.txt"); 
	
	double bbB = getcputime() - aaB; 
	cout << "\nHa tardado " << bbB << " segundos\n\n";
	return 0;
} 
