
# T1 SSOO

  

Autores: Cristóbal Gazali, Juan Ignacio Silva

  

#### Decisiones de diseño

Una de las principales decisiones fue el uso de listas ligadas para los procesos (cada uno tiene un puntero a next). Esto se hizo así porque:

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

El programa corre bien los ejemplos para test.txt que fueron subidos a la página del curso.
  

#### Comparación

Para probar el programa, utilizamos los siguientes datos:

```

Proceso1 7 4 5 4 6 7

PROCESS2 0 6 100 2 2 1 2 1

P3 23 2 6 5

Proceso_4 15 4 3 3 3 12

Proceso5 50 6 3 3 3 4 4 4

```

Además, utilizamos como parámetros un q=5 y 5 colas, junto con S=30.

Los resultados de cada versión en el PROCESS2 fueron los siguientes:

v1

PROCESS2:

Turnos de CPU: 25

Bloqueos: 20

Turnaround time: 183

Response time: 0

Waiting time: 65

  

v2

PROCESS2:

Turnos de CPU: 25

Bloqueos: 20

Turnaround time: 183

Response time: 0

Waiting time: 50

  

v3

PROCESS2:

Turnos de CPU: 16

Bloqueos: 10

Turnaround time: 183

Response time: 0

Waiting time: 52

  

Los resultados son bastante claros, gracias a la elección del primer burst de este proceso, que es mucho más largo que los burst del resto. El proceso tiende a quedarse sólo al final, sin otros que procesos que pidan la CPU, lo que permite identificar de forma más directa los cambios entre versiones.

Todas las versiones dan el mismo turnaround time y response time, ya que es el primer proceso que llega (en t=0), y el último que sale.

Comparando v1 con v2, vemos que la diferencia está en el waiting time, que es mucho menor en v2. Esto se debe a que, en v2, se implementa el sistema del S, que le permite al proceso volver a la cola de mayor prioridad, a pesar de no terminar su primer burst. Por lo tanto, alcanza a usar la CPU un par de veces antes de bajar a la lista de menor prioridad, en la que tiene que quedarse esperando más tiempo (mayor waiting time).

Comparando v2 y v3, vemos que la diferencia más grande está en el número de turnos de CPU y bloqueos, ambos menores en v3. Esto es consecuencia directa de la implementación del quantum diferenciado entre las distintas colas, y permite al proceso tener menos bloqueos porque tiene un quantum más alto al estar en la última cola. El que haya menos bloqueos implica que tiene menos turnos de CPU, lo que, en un sistema real, haría que fuera mucho más eficiente el scheduling (recordemos que en esta simulación no hay un tiempo de context switch, por lo que el tener más turnos de CPU y bloqueos no cambia el tiempo total de ejecución de todos los procesos, siendo que debiera aumentarlo).

VIendo los resultados de los otros procesos podemos llegar a ciertas conclusiones sobre el turnaround time y el response time. El turnaround time y el response time van aumentando con las versiones.

El turnaround time se puede explicar por el proceso mencionado anteriormente, que hace que todo vaya más lento, ya que vuelve a subir gracias al S (usando todo el quantum). Esto se acentúa aún más cuando el quantum es variable, ya que empieza a usar aún más tiempo la CPU, aumentando el turnaround time del resto.

Algo similar se puede decir respecto al response time: como el proceso 2 acapara la CPU, y cada vez puede usarla por más tiempo (primero por el S y después por el q variable), los otros procesos tienden a llegar en momentos en que la CPU está siendo usada por él.

Esto muestra lo difícil que es hacer un scheduler que funcione mejor todas las veces, y cómo depende de ciertos procesos que podríamos denominar críticos, como el proceso 2 en este caso.

Sin embargo, podemos decir que la eficiencia real iría mejorando, dada la menor cantidad de context switches, que permiten disminuir el tiempo total en que se ejecutan todos los procesos.