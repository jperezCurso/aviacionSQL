/*
 ============================================================================
 Name        : aviacionSql.c
 Author      : juan
 Version     : 1.0
 Description : Programa en lenguaje C utilizando MySQl como base de datos
             : y GTK4 como libreria gráfica. Los datos de la bd se han
             : obtenido de OpenFlights.org. La tabla de rutas lleva desde
             : 2014 sin actualizarse así que no mostrará las rutas más
             : recientes.
 Librerias   : incluir librerias obteniendo la información al ejecutar
 	 	 	   mariadb_config

 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "basedatos.h"
#include "pantalla.h"

// --------------------------------------------------------------------------
// 						main - entrada al programa
// --------------------------------------------------------------------------

int main(int argc, char *argv[]) {
	int status = 0;

	status = sql_inicio_basedatos();
	if (status > 0) {
		printf("Error al recuperar la password del usuario de la base de datos.\n");
	} else {
		status = pantalla_inicial(argc, argv);
	}

	return status;
}


// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
