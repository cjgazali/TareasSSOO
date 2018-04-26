#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

typedef struct tarea {
    char *cmd; // la tarea misma, que sale del texto
    int intentos; // parte en 0
    int pid;
    int resultado; // final
    int num_args;
    int exit_code;
    char** argv; // los argumentos
} tarea;

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

    task -> intentos = 0;
    return task;
}

void endfree() {
    // rellenar
}

int main(int argc, char const *argv[])
{
    int n = atoi(argv[2]);
    const char* filename = argv[1];
    tarea** lista;

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
    printf("%d comandos\n", contador);
    lista = malloc((contador-1)*sizeof(tarea*));
    fclose(fp);
    if (line){
        free(line);
    }

    // leer archivo y hacer tareas
    contador=0;
    line = NULL;
    len = 0;
    ssize_t read2;
    fp = fopen(filename, "r");
    while ((read2 = getline(&line, &len, fp)) != -1) {
    	printf("%d ", contador);
    	printf("READ %s", line);
        lista[contador] = crear_tarea(line); 
        contador++;
    } 
    fclose(fp);
    if (line){
        free(line);
    }

    int i = 0;
    int j = 0;
    printf("\n");
    for (i=0; i<contador; i++){
    	printf("Comando: %s\n", lista[i]->cmd);
    	printf("Palabras: ");
    	for (j=0; j<(lista[i]->num_args); j++){
    		printf("%d: %s, ", j, lista[i]->argv[j]);
    	}
    	printf("\n");
    }

    int ejecutando=0;
    for (i=0; i<contador; i++){
    	int status;
    	//chequear que no llevo dos intentos
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
			printf("A tope\n");
			int aux_id=0;
			aux_id = wait(&status); //espero 
			printf("Esperamos...\n");

			//pasando el exit code...
			int j;
			for (j=0; j<contador; j++){
				if (lista[j]->pid == aux_id) {
					lista[j]->exit_code = status;
					printf("Terminó proceso %d con status %d\n", lista[j]->pid, lista[j]->exit_code);
					ejecutando --;
				}
			}

		}
    }

    // ya eché a correr todos, hago waits
    sleep(1);
    printf("\nAhora hacemos los waits...\n");

    int k=0;
	int aux_id=0;
	int status=0;
	aux_id = wait(&status); //espero
	for (k=0; k<ejecutando; k++){
		int j;
		//printf("Entramos al for\n");
		for (j=0; j<contador; j++){
			//printf("Buscando %d, revisando %d", aux_id, lista[j]->pid);
			if (lista[j]->pid == aux_id) {
				lista[j]->exit_code = status;
				printf("Terminó proceso %d con status %d\n", lista[j]->pid, lista[j]->exit_code);
			}
		}	
	}

    //endfree();

    return 0;
}