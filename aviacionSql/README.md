# aviacionSql

Pequeño proyecto que muestra como usar lenguaje C con la base de datos MySQL 8 y la librería gráfica GTK4 en linux.

Para utilizarlo es necesario tener un servidor MySQL activado y tener instalada GTK4.

> Está diseñada para LC_CTYPE="es_ES.UTF-8" (español). Otros idiomas que también utilicen la coma como delimitador decimal también deberían funcionar.


** Instalación **
Crear la base de datos, situarse en directorio basedatos y ejecutar :

    `tar -xvf Dumpdb.tar.gz`
    `mysql < Dumpdb.sql`

Crear las variables siguientes en ~/.bashrc :
> La password debe ser la misma que la que va al final del fichero Dumpdb.sql. Si se quiere cambiar hay que hacerlo en los dos sitios.

   `export AVIACION_PASS=#abcABC_1234`
   `export AVIACION_USER=aviador`
   `export AVIACION_SERV=localhost`
   `export AVIACION_DBAS=bdaviacion`

El proyecto ha sido creado con Eclipse IDE for C/C++ Developeres 2024-03 (4.31.0)
Se puede utilizar el ejecutable del directorio aviacionSql y ejeccutarlo ' ./aviacionSql '


** Descripción del proyecto **
Pide mínimo 4 caracteres del nombre de un aeropuerto y muestra los encontrados.
Seleccionar un aeropuerto y muestra las rutas que tiene.
Seleccionar la ruta elegida y :
    - muestra en el mapa los aeropuertos origen (verde) y destino (rojo),
    - genera un fichero en binario con los datos de la ruta y lo lee mostrandolo en pantalla los datos.
    - graba un registro en la tabla elegidas con la ruta seleccionada.


** Imágenes de muestra **

[![Inicio](https://github/aviacionSql/tree/aviacionSql/imagenes/01_inicio.png)]

[![Inicio](https://github/aviacionSql/tree/aviacionSql/imagenes/02_aeropuerto.png)]

[![Inicio](https://github/aviacionSql/tree/aviacionSql/imagenes/03_port_seleccionado.png)]

[![Inicio](https://github/aviacionSql/tree/aviacionSql/imagenes/04_rutas.png)]

[![Inicio](https://github/aviacionSql/tree/aviacionSql/imagenes/05_ruta_seleccionada.png)]
