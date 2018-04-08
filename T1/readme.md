# T1 SSOO

Autores: Cristóbal Gazali, Juan Ignacio Silva

#### Decisiones de diseño
Una de las principales decisiones fue el uso de listas ligadas para  los procesos (cada uno tiene un puntero a next). Esto se hizo así porque:
* permite concatenar colas de manera más fácil
* facilita enormemente el uso y control de malloc y free, pues la estructura lista no cambia de tamaño, solo se alarga o acorta la cadena lógica de punteros a procesos

Creamos un struct `Queue`, que tiene como atributos su head y su tail, además de su largo. Por último, todo esto se puso dentro de una Queue de Queues (de procesos) llamada `colas`.

Por otra parte, decidimos hacer una simulación discreta, en que cada tick es una unidad de tiempo, y en cada uno de ellos cambiamos los datos de los procesos según sea necesario. 
Decidimos, además, usar variables globales para simplificar el tema de Ctrl+C y las estadísticas.

Otro punto de diseño importante fue que para iterar hasta llegar al final de una cola usamos una comparación con el último elemento de la cola, que es una lista ligada. Este patrón se puede ver en varios `while` en nuestro código.

Por último, tratamos de hacer funciones generales que sólo cambiaran cuando pasa algo distinto en una versión, tomando como base v1. Es decir, tomamos v1 e hicimos los cambios para que ejecute los cambios necesarios según la versión.

#### Supuestos adicionales
Tratamos de hacer el programa lo más general posible. Por ejemplo, tratamos de dar soluciones a problemas como el empate entre el tiempo S y el final de un burst corriendo.
En caso de empate de terminar mi burst con que me saquen de la CPU por terminar mi `q`, lo tomamos como que hay un bloqueo por `q`.

#### Información extra
Es primera vez que usamos C, por lo que fue muy difícil llegar al punto de un programa funcional en todos los casos. Nos tomó un tiempo entender cómo usar los punteros y los structs, tras lo cual todo fue más rápido. El ejemplo de las listas ligadas nos ayudó bastante. Aprendimos muchísimo en el proceso y los resultados fueron buenos.

#### Comparación
Para probar el programa, utilizamos los siguientes datos:
```
Proceso1 7 4 5 4 6 7
PROCESS2 0 6 100 2 2 1 2 1
P3 23 2 6 5
Proceso_4 15 4 3 3 3 12
Proceso5 50 6 3 3 3 4 4 4
```
En cuanto a los resultados, es difícil decir qué tan efectivos fueron los cambios dada la cantidad de procesos que usamos para probar. Sin embargo, podemos ver desde los resultados los siguientes datos: 
1) la segunda versión permite que hayan menos bloqueos que la primera. Esto tiene lógica, dado que los procesos más grandes quedan al final (mínima prioridad), y si el q es pequeño entonces van a tener que cambiar muy seguido.
2) la tercera versión impide que los procesos que tienen un burst largo se queden para siempre en las colas con menos prioridad, lo que es útil dado que sus otros burst se podrían terminar sin usar el quantum más grande que tienen las colas de menor prioridad.
3) 
