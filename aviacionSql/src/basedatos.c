/*
 * basedatos.c
 *
 *  Created on: 18 may 2024
 *      Author: juan
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "basedatos.h"
#include "misutils.h"

// Nombre de la variable del sistema que tiene la password del usuario de la base de datos
const char *VAR_PASS = "AVIACION_PASS";
const char *VAR_USER = "AVIACION_USER";
const char *VAR_SERV = "AVIACION_SERV";
const char *VAR_DBAS = "AVIACION_DBAS";

// Número máximo de filas a devolver en la búsqueda por aeropuerto
const char *LIM_AEROPUERTOS = "10";

// Variables utilizadas en la conexión con la BD. Se obtienen en inicio_basedatos()
char *server   = "xxxxxxxxxxxxxxxxxxxx";
char *user     = "xxxxxxxxxxxxxxxxxxxx";
char *password = "xxxxxxxxxxxxxxxxxxxx";
char *database = "xxxxxxxxxxxxxxxxxxxx";

// Variable global con los aeropuertos devueltos en la búsqueda
st_list_aeropu *lista_aeropuertos = NULL;
unsigned long_st_laeropu;

// Variable global con las rutas devueltas en la búsqueda
st_list_rutas *lista_rutas = NULL;
unsigned long_st_lrutas;

// Variable global con los datos devueltos ne la búsqueda
st_port_tooltip *tt_port = NULL;

// Cabeceras del widget de listado de aeropuertos de pantalla
char *laerop_nombres[] = {"Aeropuerto", "id"};

// Cabeceras del widget de listado de rutas de pantalla
char *lrutas_nombres[] = {"Origen", "Destino", "Aerolinea", "Distancia", "Notas", "Vehículo"};

// Definición de funciones internas -----------------------------------------
MYSQL* conectar_basedatos();
void finish_with_error(MYSQL *conn);
void mostrar_tablas(MYSQL *conn);
void liberar_mem_ttport();
void liberar_mem_lispor();
void liberar_mem_lisrut();

// ----------------------------------------------------------------------------
// Inicializa los valores para conectarse a la base de datos.
// Devuelve 1 si hubo algún error.
// ----------------------------------------------------------------------------
int sql_inicio_basedatos() {
	password = getenv(VAR_PASS);
	if (password == NULL)
		return 1;

	user = getenv(VAR_USER);
	if (user == NULL)
		return 1;

	server = getenv(VAR_SERV);
	if (server == NULL)
		return 1;

	database = getenv(VAR_DBAS);
	if (database == NULL)
		return 1;

	return 0;
}

// ----------------------------------------------------------------------------
// Liberar la memoria asignada para no dejarla asignada a nuestra aplicación
// Opciones:
// FREE_TODO, FREE_LISPOR, FREE_LISRUT
// ----------------------------------------------------------------------------
void liberar_mem_ttport() {
	if (tt_port) {
//		printf("liberada tt_port\n");
		free(tt_port);
		tt_port = NULL;
	}
}

void liberar_mem_lispor() {
	if (lista_aeropuertos) {
//		printf("liberada lista_aeropuertos\n");
		free(lista_aeropuertos);
		lista_aeropuertos = NULL;
		long_st_laeropu = 0;
	}
}

void liberar_mem_lisrut() {
	if (lista_rutas) {
//		printf("liberada lista_rutas\n");
		free(lista_rutas);
		lista_rutas = NULL;
		long_st_lrutas = 0;
	}
}

void sql_liberar_memoria(int libre) {
	switch(libre) {
	case FREE_TODO:
		liberar_mem_lispor();
		liberar_mem_lisrut();
		liberar_mem_ttport();
		break;
	case FREE_LISPOR:
		liberar_mem_lispor();
		break;
	case FREE_LISRUT:
		liberar_mem_lisrut();
		break;
	case FREE_TTPORT:
		liberar_mem_ttport();
		break;
	default:
		printf("Error al liberar memoria, código = %d desconocido\n", libre);
	}
}

// ----------------------------------------------------------------------------
// Busca hasta 10 aeropuertos que contengan el texto recibido como parámetro.
// ----------------------------------------------------------------------------
int sql_buscar_aeropuertos(char *aeropuerto) {
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;

	conn = conectar_basedatos();

	// Construir la sentencia select para recuperar los aeropuertos
	char busca1[100] = "SELECT * FROM aeropuertos WHERE LOWER(nombre) LIKE '%";
    strcat( busca1, aeropuerto );
    strcat( busca1, "%' LIMIT " );
    strcat( busca1, LIM_AEROPUERTOS );

	if (mysql_query( conn, busca1 )) {
		finish_with_error( conn );
	}

	// La función mysql_store_result devuelve el número de filas leídas, pero consume más recursos.
//	res = mysql_use_result(conn);
	res = mysql_store_result( conn );
	unsigned long int num_filas = mysql_num_rows( res );

	// reservamos espacio para guardar la lista de aeropuertos devuelta por la base de datos
	sql_liberar_memoria( FREE_LISPOR );
//	printf("Reservando memoria: %ld\n", num_filas * sizeof(st_list_aeropu));
	lista_aeropuertos = calloc( num_filas, sizeof(st_list_aeropu) + 1 );
	long_st_laeropu = num_filas;

	// Formamos la lista de aeropuertos con la ciudad, aeropuerto y país.
	// Se comprueba que la unión de textos no exceda el límite del campo destino.
	// strcat y strncat añaden el campo nulo al final del texto añadido.
	int ind = 0;
	int libre = 0;
	while( (row = mysql_fetch_row(res)) != NULL ) {
		lista_aeropuertos[ind].id = atoi(row[0]);
		strcpy( lista_aeropuertos[ind].nombre, row[2] );
		strcat( lista_aeropuertos[ind].nombre, " - " );
		libre = LONG_LISTAERO - strlen( lista_aeropuertos[ind].nombre ) - 3;

		if (libre > 1) {
			strncat( lista_aeropuertos[ind].nombre,
					row[1],
					strlen(row[1]) +1 );
			libre -= strlen( lista_aeropuertos[ind].nombre );
			if (libre > 2) {
				strncat( lista_aeropuertos[ind].nombre,
						", ",
						3 );
				libre -= 3;
				if (libre > 1) {
					strncat( lista_aeropuertos[ind].nombre,
							row[3],
							strlen(row[3]) +1 );
				}
			}
		}
		ind++;
	}

	mysql_free_result( res );
	mysql_close( conn );

	return EXIT_SUCCESS;
}

// ----------------------------------------------------------------------------
// Busca las rutas de un aeropuerto determinado
// ----------------------------------------------------------------------------
int sql_buscar_rutas(unsigned int id_aeropuerto) {
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;

	conn = conectar_basedatos();

	// Construir la sentencia select para recuperar las rutas.
	// Devuelve los campos decimales con ',' como separador ya que la función
	// atof() que se utiliza más adelante se ve afectada por las categorías
	// LC_CTYPE y LC_NUMERIC del entorno local.
	char busca1[670] = "SELECT puertoOri, puertoDes, "
					   "CONCAT(ae.nombre, ' (', aerolinea,')'), equipment, "
					   "CASE codeshare WHEN 0 THEN 'Direct' WHEN 1 THEN 'Codeshare' END, "
					   "REPLACE(CONVERT(apo.latitud, NCHAR), '.', ','), "
					   "REPLACE(CONVERT(apo.longitud, NCHAR), '.', ','), "
					   "REPLACE(CONVERT(apd.latitud, NCHAR), '.', ','), "
					   "REPLACE(CONVERT(apd.longitud, NCHAR), '.', ','), "
					   "puertoDesId "
					   "FROM rutas "
					   "JOIN aerolineas ae "
					   "ON aerolineaId = ae.id "
					   "JOIN aeropuertos apo "
			           "ON puertoOriId = apo.id "
			   	   	   "JOIN aeropuertos apd "
	           	   	   "ON puertoDesId = apd.id "
					   "WHERE puertoOriId = ";
	char numero[10];
	sprintf( numero, "%d", id_aeropuerto );
    strcat( busca1, numero );
    strcat( busca1, " ORDER BY puertoDes" );

	if (mysql_query( conn, busca1 )) {
		finish_with_error( conn );
	}

	// Se utiliza store en vez de use para poder saber el número de filas leídas, aunque
	// este método consume más recursos.
//	res = mysql_use_result(conn);
	res = mysql_store_result( conn );
	int num_filas  = mysql_num_rows( res );

	// reservamos espacio para guardar la lista de rutas devuelta por la base de datos
	sql_liberar_memoria( FREE_LISRUT );
//	printf("Reservando memoria: %ld\n", num_filas * sizeof( st_list_rutas ));
	lista_rutas = calloc( num_filas, sizeof( st_list_rutas ) );
	long_st_lrutas = num_filas;

	// Formamos la lista de aeropuertos .
	// Se comprueba que la unión de textos no exceda el límite del campo destino.
	// strcat y strncat añaden el campo nulo al final del texto añadido.
	int ind = 0;

	while( (row = mysql_fetch_row(res)) != NULL ) {
		strcpy( lista_rutas[ind].iataOri, row[0] );
		strcpy( lista_rutas[ind].iataDes, row[1] );
		strcpy( lista_rutas[ind].aerolin, row[2] );
		strcpy( lista_rutas[ind].vehicul, row[3] );
		strcpy( lista_rutas[ind].notas,   row[4] );
		lista_rutas[ind].latOri = atof( row[5] );
		lista_rutas[ind].lonOri = atof( row[6] );
		lista_rutas[ind].latDes = atof( row[7] );
		lista_rutas[ind].lonDes = atof( row[8] );
		lista_rutas[ind].distancia = haversine( lista_rutas[ind].latOri, lista_rutas[ind].lonOri,
											    lista_rutas[ind].latDes, lista_rutas[ind].lonDes );
		lista_rutas[ind].port_des_id = atoi( row[9] );

		ind++;
	}

	mysql_free_result( res );
	mysql_close( conn );

	return EXIT_SUCCESS;
}

// ----------------------------------------------------------------------------
// Busca los datos de un aeropuerto para mostrarlos en un tooltip
// ----------------------------------------------------------------------------
int sql_buscar_aeropuerto_tooltip(unsigned int id_aeropuerto) {
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;

	conn = conectar_basedatos();

	// Construir la sentencia select para recuperar el aeropuerto
	char busca1[88] = "SELECT nombre, iata, ciudad, pais "
					  "FROM aeropuertos "
					  "WHERE id = ";
	char numero[10];
	sprintf( numero, "%d", id_aeropuerto );
    strcat( busca1, numero );

	if (mysql_query( conn, busca1 )) {
		finish_with_error( conn );
	}

	res = mysql_use_result( conn );
	int num_filas = 1;

	// reservamos espacio para guardar los datos del aeropuerto
	sql_liberar_memoria( FREE_TTPORT );
//	printf("Reservando memoria: %ld\n", num_filas * sizeof( st_port_tooltip ));
	tt_port = calloc( num_filas, sizeof( st_port_tooltip ) );

	// Rellenar la estructura de los datos. Solo debería haber 1 fila
	while((row = mysql_fetch_row(res)) != NULL) {
		strcpy( tt_port->nombre, row[0] );
		strcpy( tt_port->iata,   row[1] );
		strcpy( tt_port->ciudad, row[2] );
		strcpy( tt_port->pais,   row[3] );
	}

	mysql_free_result( res );
	mysql_close( conn );

	return EXIT_SUCCESS;
}

// ----------------------------------------------------------------------------
// Inserta en tabla elegidas la ruta que ha seleccionado
// Los datos con decimales al utilizar sprintf utilizan la coma. La base de datos
// utiliza el punto como separador por lo que hay que cambiarlo en los campos
// antes de enviarlos a grabar. Esto ocurre porque la función sprintf() se ve
// afectada por LC_CTYPE y LC_NUMERIC del entorno local
// ----------------------------------------------------------------------------
int sql_insertar_ruta_elegida(st_list_rutas elegida) {
	MYSQL *conn;

	conn = conectar_basedatos();

	// Construir la sentencia select para recuperar el aeropuerto
	char ins1[1000] = "INSERT INTO elegidas (iataOri, iataDes, aerolin, vehicul, notas, latOri, "
			          "lonOri, latDes, lonDes, distancia, portDesId) VALUES(";
	char numero[66];
	char *ptr;
	int busca = ',';

	sprintf( numero, "'%s', ", elegida.iataOri );
    strcat( ins1, numero );
	sprintf( numero, "'%s', ", elegida.iataDes );
    strcat( ins1, numero );
	sprintf( numero, "'%s', ", elegida.aerolin );
    strcat( ins1, numero );
	sprintf( numero, "'%s', ", elegida.vehicul );
    strcat( ins1, numero );
	sprintf( numero, "'%s', ", elegida.notas );
    strcat( ins1, numero );
	sprintf( numero, "%lf, ", elegida.latOri );
	ptr = strchr( numero, busca);
	*ptr = '.';
    strcat( ins1, numero );

    sprintf( numero, "%lf, ", elegida.lonOri );
	ptr = strchr( numero, busca);
	*ptr = '.';
	strcat( ins1, numero );

	sprintf( numero, "%lf, ", elegida.latDes );
	ptr = strchr( numero, busca);
	*ptr = '.';
	strcat( ins1, numero );

	sprintf( numero, "%lf, ", elegida.lonDes );
	ptr = strchr( numero, busca);
	*ptr = '.';
	strcat( ins1, numero );

	sprintf( numero, "%lf, ", elegida.distancia );
	ptr = strchr( numero, busca);
	*ptr = '.';
	strcat( ins1, numero );

	sprintf( numero, "%d", elegida.port_des_id );
    strcat( ins1, numero );
    strcat( ins1, ")");

	if (mysql_query( conn, ins1 )) {
		finish_with_error( conn );
	}

	mysql_close( conn );

	return EXIT_SUCCESS;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void finish_with_error(MYSQL *conn) {
	fprintf( stderr, "err: %s\n", mysql_error( conn ) );
	mysql_close( conn );
	exit( EXIT_FAILURE );
}


// ----------------------------------------------------------------------------
// Conecta con la base de datos y la configura para su acceso.
// ----------------------------------------------------------------------------
MYSQL* conectar_basedatos() {
	MYSQL *conn;

	conn = mysql_init( NULL );
	if (conn == NULL) {
		fprintf(stderr, "err: %s\n", mysql_error( conn ));
		exit( EXIT_FAILURE );
	}

	if (!mysql_real_connect( conn, server, user, password, database, 0, NULL, 0 )) {
		finish_with_error( conn );
	}

	// indicar el juego de caracteres a utilizar, en este caso es UTF-8
	mysql_set_character_set( conn, "utf8" );

	return conn;
}

// ----------------------------------------------------------------------------
// Muestra las tablas que hay en la base de datos actual.
// ----------------------------------------------------------------------------
void mostrar_tablas(MYSQL *conn) {
	MYSQL_RES *res;
	MYSQL_ROW row;

	if (mysql_query( conn, "show tables" ) ) {
		finish_with_error( conn );
	}

	res = mysql_use_result( conn );

	printf("MySQL Tables in mysql database:\n");
	while((row = mysql_fetch_row( res )) != NULL)
		printf("\t%s \n", row[0]);

	mysql_free_result( res );

}
