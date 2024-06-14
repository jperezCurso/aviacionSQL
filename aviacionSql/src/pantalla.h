/*
 * pantalla.h
 *
 *  Created on: 18 may 2024
 *      Author: juan
 */

#ifndef PANTALLA_H_
#define PANTALLA_H_
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
//#include <gio/gioenums.h>

#define BOX_SPACING 5
#define WIN_ANCHO   1280
#define WIN_ALTO    1080
#define LOGO_ANCHO  150
#define LOGO_ALTO   100
#define MAPA_ANCHO  1260
#define MAPA_ALTO   740
#define BOTON_ANCHO 60
#define BOTON_ALTO  40
#define BOTON_MARG_START 30
#define INPUT_ANCHO 150
#define INPUT_ALTO  20
#define INPUT_UMBRAL 4	// Nº mínimo de caracteres para poder buscar un aeropuerto


// Funciones que se usan en otros fuentes ajenos a pantalla.c -------------------
int pantalla_inicial(int argc, char *argv[]);

#endif /* PANTALLA_H_ */
