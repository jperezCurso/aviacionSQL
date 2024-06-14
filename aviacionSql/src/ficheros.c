/*
 * ficheros.c
 *
 *  Created on: 31 may 2024
 *      Author: juan
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "basedatos.h"
#include "ficheros.h"

// variables globales en otros fuentes ---------------------------------------
extern st_list_rutas *lista_rutas;
// fin variables globales ----------------------------------------------------

//--------------------------------------------------------------------------
// Escribe un fichero conteniendo los datos de la ruta seleccionada.
// Parametro de entrada el índice de la ruta a usar.
// Devuelve 0 si todo correcto, -1 si hubo algún error.
//--------------------------------------------------------------------------
int files_generar_fichero(int posi) {
	FILE *fp;
	char fname[28] = "./rutas/ruta_";
	st_list_rutas myruta;

	// Formar el nombre del fichero
	strcat(fname, lista_rutas[posi].iataOri);
	strcat(fname, "_a_");
	strcat(fname, lista_rutas[posi].iataDes);
	strcat(fname, ".dat");

	// Abrir el fichero en modo escritura en binario.
	if ((fp = fopen( fname, "wb" )) == NULL) {
		printf("Error abriendo fichero para escritura %s\n", fname);
		return -1;
	}

	// Escribir la ruta seleccionada
	if (fwrite( &lista_rutas[posi], sizeof( st_list_rutas ), 1, fp) != 1) {
		printf("Error escribiendo fichero %s\n", fname);
		return -1;
	}

	fclose(fp);

	// Abrir el fichero en modo lectura en binario.
	if ((fp = fopen( fname, "rb" )) == NULL) {
		printf("Error abriendo fichero para lectura %s\n", fname);
		return -1;
	}

	// Leer la ruta guardada
	if (fread( &myruta, sizeof( st_list_rutas ), 1, fp ) != 1) {
		printf("Error leyendo fichero %s\n", fname);
		return -1;
	}

	printf("\nLeido del fichero generado con la ruta seleccionada:\n");
	printf("Fichero = %s\n", fname);
	printf(" %s\n", myruta.iataOri);
	printf(" %s\n", myruta.iataDes);
	printf(" %s\n", myruta.aerolin);
	printf(" %s\n", myruta.vehicul);
	printf(" %s\n", myruta.notas);
	printf(" %f\n", myruta.latOri);
	printf(" %f\n", myruta.lonOri);
	printf(" %f\n", myruta.latDes);
	printf(" %f\n", myruta.lonDes);
	printf(" %f\n", myruta.distancia);
	printf(" %d\n", myruta.port_des_id);

	fclose(fp);

	return 0;
}
