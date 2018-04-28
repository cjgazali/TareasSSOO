comandos1.txt es un ejemplo en que todos los comandos funcionan, y se ve un poco cómo va avanzando el programa
Al final de leer archivo y hacer tareas hay un close del archivo fp comentado. Cuando lo ejecuto, me tira un error corrupted size vs. prev_size, creo que es porque en teoría edito líneas del archivo. Por eso mismo usé copias de las líneas (con strcpy), pero tampoco funcionó
Usé time(NULL) por problema con otras funciones como clock o gettimeofday :(
El parseo lo hice con los espacios, pero sin "" por problemas con el strtok al usar "
Cuando un proceso falla, se ejecuta nuevamente de forma inmediata, se "salta la fila"

Para proba el programa se usó un documento con los siguientes comandos:
touch a.txt
touch b.txt
touch c.txt
touch d.txt
cat comandos3.txt
cat comandos1.txt
ps T
echo haciendo_hora_something
ls
sleep 3
rm hola.txt
rm a.txt
rm b.txt
rm c.txt
rm d.txt
ls
./ejemplo1.exe

./ejemplo1.exe era un programa en c que hacía sleep
Se ejecutó con ./t1.exe comandos1.txt 5 

Hecho por Cristóbal Gazali y Juan Ignacio Silva