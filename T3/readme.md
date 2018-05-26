

# T3 SSOO

  

Autores: Cristóbal Gazali, Juan Ignacio Silva

  

#### Decisiones de diseño
Los números fueron tratados con Big Endian.

La lectura del archivo .bin se hace cargando sus datos con la función `init` . En adelante, los cambios se guardan en el archivo cada vez que aparece la función `save_changes`. La función `liberar` hace todos los free necesarios para recuperar la memoria usada en leer archivo .bin usando nuestras estructuras.



#### Supuestos adicionales

Se tomaron los supuestos dados por enunciado.
