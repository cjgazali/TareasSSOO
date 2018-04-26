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
	task -> argv = malloc(sizeof(char*)*(count-1));
	task -> num_args = count-1;
	//printf("%s", linea);

	// asignar al array
	count = 0;
	token = strtok(linea, " ");
	while (token != NULL){
		//printf("%s \n", token);
		task -> argv[count] = token;
		token = strtok(NULL, " ");
		//printf("%s", token);
		count ++;
	}

    task -> intentos = 0;
    //printf("Comando: %s\n", task -> cmd);
    //printf("Argumentos: \n");
    //int i;
    //int length;
    //length = sizeof(task->argv) / sizeof(task->argv[0]);
    //printf("Largo %d\n", count);
    //printf("Comandos: \n");
    //for (i=0; i <= count; i++){
    //	printf("%s \n", task -> argv[i]);
    //}
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
    	//lista = calloc(contador, sizeof(tarea));
    	printf("READ %s %d\n", line, contador);
        lista[contador] = crear_tarea(line);
        //execvp(lista[contador]->cmd, lista[contador]->argv);
        //printf("%s uf %s\n", crear_tarea(line)->cmd, lista[contador]->cmd);
        
        // OJO DE ACÁ EN ADELANTE
        printf("Comando: %s\n", lista[0] -> cmd);
        printf("%s \n", lista[0] -> argv[1]);
        printf("%s \n", lista[contador] -> argv[1]);
        printf("%d\n", contador);
        //printf("\n");
        contador++;
        // después de cada vuelta se cambian los atributos de todas las listas...
    } 
    fclose(fp);
    if (line){
        free(line);
    }

    int i = 0;
    int j = 0;
    printf("Todo bien??\n");
    for (i=0; i<contador; i++){
    	printf("Palabras: ");
    	for (j=0; j<(lista[i]->num_args); j++){
    		printf("%s ", lista[i]->argv[j]);
    	}
    	printf("\n");
    }

    //char *cmd = "ls";
    //char *arg[3];
    //arg[0] = "ls";
    //arg[1] = "-a";
    //arg[2] = NULL;
    //execvp(cmd, arg);


    //endfree();

    return 0;
}