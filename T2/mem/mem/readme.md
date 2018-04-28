
# T2 SSOO

Autores: Cristóbal Gazali, Juan Ignacio Silva

## Parte II: memoria virtual

Autor parte II: Cristóbal Gazali.
  

#### Decisiones de diseño

Se hizo todo escalable y parametrizado a la cantidad de niveles, de manera tal que se usan en general las mismas funciones y códigos para cualquier caso.

Memoria física, TLB y tabla de páginas se construyeron con estructuras de lógica 'fila', con todos los atributos necesarios, y luego se construyen arreglos de la estructura respectiva.
En las tres se agregó validez y se usó para mantener limpieza de información y mostrarla de manera eficiente.

En línea 397 puede cambiar la ubicación de data.bin.



#### Supuestos adicionales

En general se cumplen supuestos dados por enunciado.
