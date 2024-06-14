/*
 * utils.c
 *
 *  Created on: 19 may 2024
 *      Author: juan
 */

#include "misutils.h"


// --------------------------------------------------------------------------------
// Función para convertir grados a radianes
// --------------------------------------------------------------------------------
double deg2rad(double deg) {
    return deg * (M_PI / 180);
}

// --------------------------------------------------------------------------------
// Función Haversine para calcular la distancia entre 2 puntos GPS
// --------------------------------------------------------------------------------
double haversine(double lat1, double lon1, double lat2, double lon2) {
    double lat1_rad = deg2rad(lat1);
    double lat2_rad = deg2rad(lat2);
    double lon1_rad = deg2rad(lon1);
    double lon2_rad = deg2rad(lon2);

    double dlat = lat2_rad - lat1_rad;
    double dlon = lon2_rad - lon1_rad;

    double a = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1_rad) * cos(lat2_rad) *
               sin(dlon / 2) * sin(dlon / 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return R * c;
}

// --------------------------------------------------------------------------------
// Convertir coordenadas GPS a un plano 2D
// latitud, longitud: son el punto GPS
// ancho, alto: son las dimensiones del plano 2D
// x, y: son las posiciones en el plano 2D obtenidas
// --------------------------------------------------------------------------------
void util_gps_a_2d(double latitud, double longitud, int ancho, int alto, int *x, int *y) {
	// Normalizar las coordenadas
    double lat_norm = (latitud + 90) / 180.0;
    double lon_norm = (longitud + 180) / 360.0;

    // Escalar a las dimensiones del plano
	*x = (int)(lon_norm * ancho);
	*y = (int)((1 - lat_norm) * alto);
}
