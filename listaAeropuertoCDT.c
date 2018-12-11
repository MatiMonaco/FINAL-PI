
/*
 * airportCDT.c
 *
 */
/* Version recursiva de listas implementadas dinamicamente */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "listaAeropuertoADT.h"
#include <string.h>

#define OACI 1
#define DENOMINACION 4
#define PROVINCIA 21
#define MAX_DENOM 70
#define MAX_TEXTO 250
#define DIAS_SEMANA 7
#define REGULAR 0
#define NO_REGULAR 1
#define VUELO_PRIVADO 2

typedef struct tMov{
	int cant_cabotaje;
	int cant_internacional;

}tMov;

typedef struct tDatosA{
	char * oaci; 
	char * denom;
	char * prov;
}tDatosA;

typedef struct tAeropuerto {
	tDatosA datos;	
	tMov cant_mov[DIAS_SEMANA][3]; //  0 = Regulares, 1 = Vuelos no regulares , 2= Vuelos privados;
	int mov_totales;
	struct tAeropuerto * cola;
	
}tAeropuerto;

typedef struct tAeropuerto * tAeropuertoP;

struct listaAeropuertoCDT {
	tAeropuertoP primero;
	
	tAeropuertoP proximo;
};



static comparaMov(tAeropuertoP a1,tAeropuertoP a2)
{
	return a1->mov_totales - a2->mov_totales;

}


static tAeropuertoP
insertarAPRec(tAeropuertoP primero,tDatosA datos, int * ok)
{
	int c;
	if(primero == NULL || (c = strcmp(primero->datos.oaci,datos.oaci)) > 0){
		tAeropuertoP aux = calloc(1,sizeof(tAeropuerto));
		if(aux == NULL){
			printf("Error:No se pudo utilizar malloc\n");
		}else{
		aux->cola = primero;
		aux->datos = datos;
		printf("Nuevo OICI: %s\nNueva Denom: %s\nNueva Prov: %s\n",aux->datos.oaci,aux->datos.denom,aux->datos.prov);
		*ok = 1;
		}
		return aux;
	}else if(c <0){
		primero->cola = insertarAPRec(primero->cola,datos,ok);
	}else{
		printf("Aeropuerto repetido \n");
	}
	return primero;
	

}

static int
insertarAP( listaAeropuertoADT lista, tDatosA datos)
{
	int ok =0 ;
	lista->primero = insertarAPRec(lista->primero, datos, &ok);

	return ok;
}



tAeropuertoP
agregarMovAPrec(tAeropuertoP primero,char * oaci,char * clase, char * clasif, int dia, int * agregado)
{
	
	int c;
	int claseIndex;
		if((c = strcmp(primero->datos.oaci,oaci)) == 0){
			if(strcmp(clase,"Regular") == 0){
				claseIndex = REGULAR;
			}else if(strcmp(clase,"No Regular") == 0){
				claseIndex = NO_REGULAR;	
			}else{
				claseIndex = VUELO_PRIVADO;
			}
			if(strcmp(clasif,"Cabotaje") == 0){
				(primero->cant_mov[dia][claseIndex].cant_cabotaje)++;
				
			}else{
				(primero->cant_mov[dia][claseIndex].cant_internacional)++;
			}
			(primero->mov_totales)++;
			 
			*agregado = 1;
			return primero;
		}else if(c > 0){
			
			printf("Error: no existe un aeropuerto con OACI: %s \n",oaci);
			
		}else{
			primero->cola = agregarMovAPrec(primero->cola,oaci,clase,clasif,dia,agregado);
			if(*agregado == 1){
				int comp;
				if( (comp =comparaMov(primero,primero->cola)) < 0||
				   ((comp == 0) && 
				    strcmp(primero->datos.oaci,primero->cola->datos.oaci) > 0)){
						tAeropuertoP aux = primero->cola->cola;
						primero->cola->cola = primero;
						primero = primero->cola;
						primero->cola->cola = aux;
				}
			}
			*agregado = 2; //Agregado y ordenado
			
		}
	
	return primero;
}
int
agregarMovAP(listaAeropuertoADT lista,char * oaci,char * clase, char * clasif, int dia)
{
	int agregado = 0;
	lista->primero = agregarMovAPrec(lista->primero,oaci,clase,clasif,dia,&agregado);
	return agregado;
}
int
cargarDatosAP(listaAeropuertoADT lista, char * pathA)
{

	FILE * archA = fopen(pathA,"rt"); //Abro archivo aeropuerto.csv
		
	if(archA == NULL){
		printf("Error al abrir los archivos. \n");
		return 1;
	
	}
		
	int cont;	
	int valido;
	char * token;
	tDatosA  datos; 
	char  s[MAX_TEXTO];
	fgets(s,MAX_TEXTO,archA);
	while(fgets(s,MAX_TEXTO,archA) != NULL){
		
		token = strtok(s,";");
		cont = 0;
		valido = 1;
		
		while((token != NULL) && valido){
			
			if(cont == OACI){
				if(strcmp(token," ") == 0){
					valido = 0;
							
				}else{		
					datos.oaci = token;
				}
				
			}else if(valido && cont == DENOMINACION){	
					datos.denom = token;
				
			}else if(valido && cont == PROVINCIA){	
					datos.prov = token;
					
			}	
			cont++;
		
			token =  strtok(NULL, ";");
			
		}		
		if(valido){ //Si es valido el aeropuerto tiene OACI
			printf("OACI: %s\nDENOM: %s\nPROVINCIA: %s\n",datos.oaci,datos.denom,datos.prov);
			if(!insertarAP(lista,datos)){
				printf("Error al cargar datos \n");
				return 1;
			}
		}
	}
	return 0;
}
listaAeropuertoADT
nuevaListaAP( void )
{
	return calloc(1, sizeof(struct listaAeropuertoCDT));
}

void
alPrincipioAP(listaAeropuertoADT lista) {
	lista->proximo = lista->primero;
}

int
tieneProxAP(const listaAeropuertoADT lista) {
	return lista->proximo != NULL;
}

void
proximoAP(listaAeropuertoADT lista)
{
	lista->proximo = lista->proximo->cola;
}

/*
typedef struct tAerolinea {
	char * nombre;
	tMov cant_mov[DIAS_SEMANA]; // Si estan en una aerolinea significa que son vuelos REGULARES.
				
	int mov_totales; // = cantidad de vuelos regulares de la aerolinea
	
	struct tAerolinea * tail;

}tAerolinea;

typedef struct tAerolinea * tAerolineaP;
*/
/*
typedef struct tDatos{
	char * origen;
	char * destino;
	char * nombre;
	int tipo_vuelo;
	int clase_vuelo;
	int clasificacion_vuelo;
	int dia;
}tDatos;

*/

/*
static tAeropuertoP insertRec(tAeropuertoP primero, tDatos datos,FILE * archA , int * added) {
	
	if( primero == NULL || (primero->mov_totales == 1) && strcmp(primero->OACI,datos->origen) > 0)
	{
		tAeropuertoP aux = malloc(sizeof( struct tAeropuerto ));
		if (aux == NULL){
			Error("No hay lugar para otro nodo\n");
		}
		aux->cola = primero;
		//Cargo datos de aeropuerto y aerolinea
		
			
		*added = 1;
		return aux;
	}else if((primero->mov_totales > 1) || strcmp(primero->OACI,datos->origen) < 0){
		
		primero->cola = insertRec( primero->cola, datos,archA, added);
	}
	return primero;
}
static int
diaDeLaSemana(int d, int m, int a)
{
	return   (d+=m<3?a--:a-2,23*m/9+d+4+a/4-a/100+a/400)%7  ; //Retorna el dia de la semana 0 es domingo, 1 es lunes, etc;

}

*/

/*
void
cargarDatos(listaAeropuertoADT lista,char * pathA)
{
	FILE * archA = fopen(pathA,"rt"); //Abro archivo aeropuerto.csv
	
	
	if(archA == NULL|| archM == NULL){
		printf("Error al abrir los archivos. \n");
	
	}
	
	int d,m,a;
	int cont = 0;	
	int index;
	char * token;
	tDatos * datos = malloc(sizeof(tDatos)); 
	char  s[MAX_TEXTO];
	fgets(s,MAX_TEXTO,archM);
	

	
	
	while(fgets(s,MAX_TEXTO,archM) != NULL){
		printf("s = %s\n",s);
		token = strtok(s,";");
		
		cont = 0;
		
		while(token != NULL){
			printf("token = %s \n",token);
			printf("cont = %d \n",cont);
			if(cont == 0){
				sscanf(token,"%02d/%02d/%04d",&d,&m,&a);
				printf("d = %d, m = %d, a = %d \n",d,m,a);
				datos->dia = diaDeLaSemana(d,m,a);
				printf("dia = %d \n",datos->dia);

				
			}else if(cont != 1 && cont != 8 && cont != 9){ //Si es un campo que me interesa extraigo la data;

			//	fscanf(archM,"%[^;]",s); //Extraigo la string hasta ;
			
				switch(cont){
					case CLASE:

						if(strcmp(token, "Regular") == 0){
							
								index = REGULAR;
						}else if(strcmp(s, "No Regular") == 0){
								index = NO_REGULAR;
						}else if(strcmp(s,  "Vuelo Privado con Matrícula Extranjera") == 0 || strcmp(s, "Vuelo Privado con Matrícula Nacional") == 0){

								index = PRIVADO;

						}
						datos->clase_vuelo = index;
					break;

					case CLASIFICACION:


						if(strcmp(token, "Cabotaje") == 0){

							index = CABOTAJE;
							
						}else if (strcmp(token,  "Internacional")  == 0){	

							index = INTERNACIONAL;
							

						}else if (strcmp(token, "N/A") == 0){

							index = NA;

						}
						datos->clasificacion_vuelo = index;
					break;

					case TIPO:
						


							if(strcmp(token,"Aterrizaje") == 0){
								index = ATERRIZAJE;			
							}else if(strcmp(token,"Despegue") == 0){	

								index = DESPEGUE;
							}	


						datos->tipo_vuelo = index;
					break;

					case ORIGEN:

						
						datos->origen = token;
						
					break;

					case DESTINO:
						
					
						datos->destino = token;
						
					break;

					case NOMBRE:
						
						
						datos->nombre = token;
						
					break;


				}
				//cont++;
			}
			cont++;
			token =  strtok(NULL, ";");

		}
		printf("nombre = %s\norigen = %s\ndestino = %s\n,clase=%d\nclasificacion=%d\ntipo=%d\n",datos->nombre,datos->origen,datos->destino,datos->clase_vuelo,datos->clasificacion_vuelo,datos->tipo_vuelo);
	}

}
*/





/*
int
listIsEmpty( listaAeropuertoADT list)
{
	return list->size == 0;
}

*/

/*
static int contains(nodeP first, listElementT elem) {
	int c;

	if(first == NULL || (c=compare(first->head, elem)) > 0)
		return 0;
	
	if ( c == 0 )
		return 1;

	return contains( first->tail, elem);
}*/
/*
int
elementBelongs( listADT list, listElementT element)
{
	return contains(list->first, element);
}
*/



/*

void
freeList( listADT list)
{
	nodeP curr=list->first, aux;

	while (curr != NULL) {
		aux = curr->tail;
		free(curr);
		curr = aux;
	}
	free(list);
}

int
listSize(const listADT list) {
	return list->size;
}
*/

