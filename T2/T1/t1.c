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
	task -> cmd = token;

	while (token != NULL){
		//printf("%s %d \n", token, count);
		//task -> argv = calloc(count+1, sizeof(char));
		//task -> argv[count] = token;
		//printf("%s", task -> argv[count]);
		token = strtok(NULL, " ");
		//printf("%s", task -> argv[count]);
		count ++;
	}
	//printf("%d \n", count);
	task -> argv = malloc(sizeof(char)*(count-1));
	//printf("%s", linea);

	// asignar al array
	count = 0;
	token = strtok(linea, " ");
	while (token != NULL){
		//printf("%s \n", token);
		task -> argv[count] = token;
		token = strtok(NULL, " ");
		//task -> argv[count] = token;
		//printf("%s", token);
		count ++;
	}

    task -> intentos = 0;
    //printf("Comando: %s\n", task -> cmd);
    //printf("Argumentos: \n");
    int i;
    int length;
    //length = sizeof(task->argv) / sizeof(task->argv[0]);
    printf("Largo %d\n", count);
    for (i=0; i <= count; i++){
    	printf("%s \n", task -> argv[i]);
    }
    return task;
}

void endfree() {
    // rellenar
}

void main(int argc, char const *argv[])
{
    int n = atoi(argv[2]);
    const char* filename = argv[1];

    // leer archivo
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    fp = fopen(filename, "r");
    while ((read = getline(&line, &len, fp)) != -1) {
    	printf("READ %s", line);
        crear_tarea(line);
        printf("\n");
    } 
    printf("\n");
    fclose(fp);
    if (line){
        free(line);
    }

    //endfree();
}