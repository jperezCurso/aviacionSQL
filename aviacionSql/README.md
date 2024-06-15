# aviacionSql

Pequeño proyecto que muestra como usar lenguaje C con la base de datos MySQL 8 y la librería gráfica GTK4 en linux. Concretamente OpenSuse Leap 15.5 .

Para utilizarlo es necesario tener un servidor MySQL activado y tener instalada la librería GTK4.

La base de datos se obtuvo de openflights.org y la imagen del mapamundi de freepik.com

> Está diseñada para LC_CTYPE="es_ES.UTF-8" (español). Otros idiomas que también utilicen la coma como delimitador decimal también deberían funcionar.


**Instalación**

Crear las variables siguientes en ~/.bashrc :
> La password (AVIACION_PASS) debe ser la misma que la que va al final del fichero Dumpdb.sql. Si se quiere cambiar hay que hacerlo en los dos sitios, en la variable y en el fichero.

  ```
  export AVIACION_PASS=#abcABC_1234
  export AVIACION_USER=aviador
  export AVIACION_SERV=localhost
  export AVIACION_DBAS=bdaviacion
  ```

Crear la base de datos, hay que situarse en el directorio basedatos y ejecutar :

    tar -xvf Dumpdb.tar.gz
    mysql < Dumpdb.sql
    

El proyecto ha sido creado con Eclipse IDE for C/C++ Developers 2024-03 (4.31.0).
Se puede utilizar el ejecutable del directorio aviacionSql y ejecutarlo con

	
	./aviacionSql
	


**Descripción del proyecto**

Pide mínimo 4 caracteres del nombre de un aeropuerto para buscarlo y muestra los encontrados.
Se selecciona un aeropuerto y muestra las rutas que tiene.

Seleccionar la ruta elegida y :

    - Muestra en el mapa los aeropuertos origen (verde) y destino (rojo).
    - Genera un fichero binario con los datos de la ruta y muestra en pantalla (stdout) los
     		datos que se han utilizado.
    - Graba un registro en la tabla "elegidas" con la ruta seleccionada.


**Imágenes de muestra**

![1](https://github.com/jperezCurso/aviacionSql/blob/main/aviacionSql/imagenes/01_inicio.png)

![2](https://github.com/jperezCurso/aviacionSql/blob/main/aviacionSql/imagenes/02_aeropuerto.png)

![3](https://github.com/jperezCurso/aviacionSql/blob/main/aviacionSql/imagenes/03_port_seleccionado.png)

![4](https://github.com/jperezCurso/aviacionSql/blob/main/aviacionSql/imagenes/04_rutas.png)

![5](https://github.com/jperezCurso/aviacionSql/blob/main/aviacionSql/imagenes/05_ruta_seleccionada.png)
