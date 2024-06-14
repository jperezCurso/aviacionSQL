/*
 * misutils.h
 *
 *  Created on: 19 may 2024
 *      Author: juan
 */

#ifndef MISUTILS_H_
#define MISUTILS_H_

#include <stdio.h>
#include <math.h>
#ifndef  M_PI
#define  M_PI  3.1415926535897932384626433
#endif

// Define el radio de la Tierra en kilómetros
#define R 6371
#define R_METROS (R * 1000)

//double deg2rad(double deg);
double haversine(double lat1, double lon1, double lat2, double lon2);
void util_gps_a_2d(double lat, double lon, int ancho, int alto, int *x, int *y);

#endif /* MISUTILS_H_ */
