/*
 * basedatos.h
 *
 *  Created on: 18 may 2024
 *      Author: juan
 */

#ifndef BASEDATOS_H_
#define BASEDATOS_H_
#include <mysql.h>
#include <stdbool.h>

// Tamaño del nombre del aeropuerto de lista de aeropuertos
#define LONG_LISTAERO 100

// Tablas de MySQL -------------------------------------------------------------------------------------
// Tabla Mysql de aerolineas ---
struct aerolineas {
	unsigned int id;
	char nombre[85];
	char alias[35];
	char iata[3];
	char icao[4];
	char callsign[50];
	char pais[37];
	bool activa;
};

// Tabla Mysql de aeropuertos ---
struct aeropuertos {
	unsigned int id;
	char nombre[72];
	char ciudad[33];
	char pais[32];
	char iata[3];
	char icao[3];
	double latitud;
	double longitud;
	float altitud;
	float zonaHora;
};

// Tabla Mysql de aviones ---
struct aviones {
	char nombre[70];
	char iata[3];
	char icao[4];
};

// Tabla Mysql de rutas ---
struct rutas {
	char aerolinea[2];
	unsigned int aerolineaId;
	char puertoOri[3];
	unsigned int puertoOriId;
	char puertoDes[3];
	unsigned int puertoDesId;
	char codeshare[4];
	unsigned short int paradas;
	char equipment[35];
};

// Tabla Mysql de rutas elegidas ---
struct elegidas {
	char timestamp[20];
	char iataOri[3];
	char iataDes[3];
	char aerolin[60];
	char vehicul[35];
	char notas[9];
	double latOri;
	double lonOri;
	double latDes;
	double lonDes;
	double distancia;
	int portDesId;
};


// Estructuras devueltas por las funciones sql_ --------------------------------------------------------
// Estructura devuelta por la select listado de aeropuertos
typedef struct {
	char nombre[LONG_LISTAERO];
	unsigned int id;
} st_list_aeropu;

// Estructura devuelta por la select listado de rutas
typedef struct {				// -- rutas --  --aerolineas--   -- rutas --
	char iataOri[4];	// puertoOri
	char iataDes[4];	// puretoDes
	char aerolin[61];	// aerolineaId->aerolinea.nombre (aerolinea)
	char vehicul[36]; 	// equipment
	char notas[10];	  	// codeshare= 0-Direct, 1-Codeshare
	double latOri;
	double lonOri;
	double latDes;
	double lonDes;
	double distancia;
	int port_des_id;
} st_list_rutas;

// Estructura devuelta por la select aerpuerto para tooltip
typedef struct {
	char nombre[72];
	char iata[4];
	char ciudad[33];
	char pais[32];
} st_port_tooltip;


// Opciones en funcion sql_liberar_memoria -------------------------------------------------------------
#define FREE_TODO 0
#define FREE_LISPOR 1	// lista de aeropuertos
#define FREE_LISRUT 2	// lista de rutas
#define FREE_TTPORT 3	// datos de un aeropuerto para tooltip

// Empiezan con sql_ las funciones que se usan en otros fuentes ajenos a basedatos.c -------------------
int  sql_inicio_basedatos();
void sql_liberar_memoria();
int  sql_buscar_aeropuertos(char *aeropuerto);
int  sql_buscar_rutas(unsigned int id);
int  sql_buscar_aeropuerto_tooltip(unsigned int id_aeropuerto);
int  sql_insertar_ruta_elegida(st_list_rutas elegida);


#endif /* BASEDATOS_H_ */
