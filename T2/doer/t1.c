#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
//time_t mi_tiempo = time(NULL);

typedef struct tarea {
    char *cmd; // la tarea misma, que sale del texto
    int intentos; // parte en 0
    int pid;
    int resultado; // final
    int num_args;
    int exit_code;
    char** argv; // los argumentos
} tarea;
// falta tiempo de cada tipo...
// --------------------------------------------

tarea* crear_tarea(char* linea) {

	// parseo y creación
	char *token;
	char *comando;
	char copia[100];
	strcpy(copia, linea);
	int count = 0;
	tarea* task = malloc(sizeof(tarea));
	token = strtok(copia, " ");
	task -> cmd = malloc(sizeof(char*));
	strcpy(task -> cmd, token);

	while (token != NULL){
		token = strtok(NULL, " ");
		count ++;
	}
	task -> argv = malloc(sizeof(char**)*(count));
	task -> num_args = count+1; 
	count = 0;
	token = strtok(linea, " ");
	while (token != NULL){
		task -> argv[count] = malloc(sizeof(char*));
		strcpy(task -> argv[count], token);
		token = strtok(NULL, " ");
		count ++;
	}

	//Hay problemas con cat, ls y sleep cuando no son el último comando
	// -------------------------------------------------------------

    task -> intentos = 0;

    return task;
}

void mostrar_tarea(tarea* task){
	int i=0;
	printf("Mostrando comandos tarea");
	for (i=0; i<task->num_args ;i++){
		printf("%s ", task->argv[i]);
	}
	printf("\n");
}

void endfree(tarea** list, int num_tasks) {
	int m=0;
	int l=0;
	for (m=0; m<num_tasks; m++){
		free(list[m]->cmd); //free cmd
		for (l=0; l<(list[m]->num_args); l++){
			free(list[m]->argv[l]); //free argv[i]
		free(list[m]->argv);
		free(list[m]);
		}
	}
	free(list);
    // muchos errores al hacer free...
    // ----------------------------------------
}

int main(int argc, char const *argv[])
{
    int n = atoi(argv[2]);
    const char* filename = argv[1];
    tarea** lista;
    printf("Máximo %d al mismo tiempo\n", n);

    // contar líneas
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int contador = 0; // número de tareas
    fp = fopen(filename, "r");
    while ((read = getline(&line, &len, fp)) != -1) {
    	contador++;
    } 
    printf("%d comandos en total\n", contador);
    lista = malloc((contador-1)*sizeof(tarea*));
    fclose(fp);
    if (line){
        free(line);
    }


    // leer archivo y hacer tareas
    int trampa=contador;
    contador=0;
    line = NULL;
    len = 0;
    ssize_t read2;
    fp = fopen(filename, "r");
    while ((read2 = getline(&line, &len, fp)) != -1) {
    	//printf("%d ", contador);
    	//printf("READ %s", line);
    	if ((contador+1) != trampa) {
			line[strlen(line)-1]=0;
		}
        lista[contador] = crear_tarea(line); 
        contador++;
    }
    fclose(fp);
    if (line){
        free(line);
    }

    //mostrar_tarea(lista[0]);
    //mostrar_tarea(lista[1]);
    //mostrar_tarea(lista[2]);
    //sleep(5);

    int i = 0;
    int j = 0;

    int ejecutando=0;
    for (i=0; i<contador; i++){
    	int status;
    	
    	ejecutando++;
    	lista[i]->intentos ++;
    	int identificador = fork();

    	if (identificador == 0){
    		execvp(lista[i]->cmd, lista[i]->argv);
    	}

    	printf("Empieza a ejecutar %d\n", identificador);
    	lista[i]->pid = identificador;
    	sleep(1);
		if (ejecutando >= n){ //estoy a tope
			printf("\nA tope\n");
			int aux_id=0;
			aux_id = wait(&status); //espero 
			printf("Esperamos...\n");

			//pasando el exit code...
			int j;
			for (j=0; j<contador; j++){
				if (lista[j]->pid == aux_id) {
					lista[j]->exit_code = status;
					printf("Terminó proceso %d con status %d\n", lista[j]->pid, lista[j]->exit_code);
					//chequear que no llevo dos intentos
    				// ------------------------------------------------------------
					ejecutando --;
				}
			}

		}
    }

    // ya eché a correr todos, hago waits
    sleep(1);
    printf("\nAhora hacemos los waits...\n");
    while (ejecutando>0){

    	int k=0;
		int aux_id=0;
		int status=0;
		aux_id = wait(&status); 
		sleep(1);
		for (k=0; k<ejecutando; k++){
			int j;
			for (j=0; j<contador; j++){
				if (lista[j]->pid == aux_id) {
					lista[j]->exit_code = status;
					printf("Terminó proceso %d con status %d\n", lista[j]->pid, lista[j]->exit_code);
					//chequear que no llevo dos intentos
    				// ------------------------------------------------------------
					ejecutando--;
					j=contador; //fuerzo la salida
				}
			}	
		}
	}

	//printf("%d", trampa);
    endfree(lista, trampa);
    // -----------------------------------------------------------

    return 0;
}