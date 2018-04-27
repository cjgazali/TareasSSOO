#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>


typedef struct tarea {
    char *cmd; // la tarea misma, que sale del texto
    int intentos; // parte en 0
    int pid;
    int resultado; // final
    int num_args;
    int exit_code;
    clock_t t_inicio;
    float t_total;
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
	for (i=0; i<task->num_args ;i++){
		printf("%s ", task->argv[i]);
	}
}

void termino_tarea(tarea* task){
	mostrar_tarea(task);
	printf(" terminó con status %d ", task->exit_code);
	task->t_total = (float)(clock() - task->t_inicio);
	printf("demorando %f\n", task->t_total);
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
    clock_t t1_sim = clock();
    int n = atoi(argv[2]);
    const char* filename = argv[1];
    tarea** lista;
    printf("\nMáximo %d al mismo tiempo\n", n);

    //clock_t start = clock();
    //clock_t end = clock();
    //float seconds = (float)(end-start) / CLOCKS_PER_SEC;
    //printf("%f", seconds);

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
    printf("%d comandos en total\n\n", contador);
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
    	char copia[100];
		strcpy(copia, line);
		if ((contador+1) != trampa) {
			copia[strlen(copia)-1]=0;
		}
        lista[contador] = crear_tarea(copia);
    	//if ((contador+1) != trampa) {
		//	line[strlen(line)-1]=0;
		//}
        //lista[contador] = crear_tarea(line); 
        contador++;
    }
    sleep(1);
    //fclose(fp); 
    //queda el sapo... reclama que cambié el tamaño, aunque haga strcpy
    if (line){
        free(line);
    }
    //sleep(3);

    mostrar_tarea(lista[9]);
    int aux = fork();
    if (aux==0){
    	execvp(lista[9]->cmd, lista[9]->argv);
    }
    sleep(1);
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
    		//int fd = open("output.txt", O_CREAT | O_TRUNC | O_RDWR, 0644);
    		//if (fd < 0) {
        	//	perror("open()");
        	//	exit(EXIT_FAILURE);
    		//}
    		//close(STDOUT_FILENO);
    		//dup2(fd, STDOUT_FILENO);
    		// Esto es para pasar todo a un archivo... 
    		// no hace append, rellena entero
    		execvp(lista[i]->cmd, lista[i]->argv);
    	}

    	//printf("Empieza a ejecutar ");
    	//mostrar_tarea(lista[i]);
    	//printf(" con id %d", identificador);
    	//char* t_string;
    	//t_string = ctime(&(lista[i]->t_inicio));
    	//printf(" en %s\n", t_string);
    	//printf("\n");

    	lista[i]->pid = identificador;
    	lista[i]->t_inicio = clock();
    	sleep(0.2);
		if (ejecutando >= n){ //estoy a tope
			//printf("\nA tope\n");
			int aux_id=0;
			aux_id = wait(&status); //espero 
			//printf("Esperamos...\n");

			//pasando el exit code...
			int j;
			for (j=0; j<contador; j++){
				if (lista[j]->pid == aux_id) {
					lista[j]->exit_code = status;
					//printf("Terminó proceso %d con status %d, \n", lista[j]->pid, lista[j]->exit_code);
					
					termino_tarea(lista[j]);
					//mostrar_tarea(lista[j]);
					//printf(" terminó con status %d ", lista[j]->exit_code);
					//lista[j]->t_total = (float)(clock() - lista[j]->t_inicio);
					//printf("demorando %f\n", lista[j]->t_total);

					// if terminó bien asigno el t_total, else no
					//chequear que no llevo dos intentos
    				// ------------------------------------------------------------
					ejecutando --;
				}
			}

		}
    }

    // ya eché a correr todos, hago waits
    sleep(0.2);
    while (ejecutando>0){

    	int k=0;
		int aux_id=0;
		int status=0;
		aux_id = wait(&status); 
		sleep(0.2);
		int j;
		for (j=0; j<contador; j++){
			if (lista[j]->pid == aux_id) {
				lista[j]->exit_code = status;
				
				termino_tarea(lista[j]);
				//mostrar_tarea(lista[j]);
				//printf(" terminó con status %d ", lista[j]->exit_code);
				//lista[j]->t_total = (float)(clock() - lista[j]->t_inicio);
				//printf("demorando %f\n", lista[j]->t_total);
				
				// if terminó bien asigno el t_total, else no
				//chequear que no llevo dos intentos
    			// ------------------------------------------------------------
				ejecutando--;
				j=contador; //fuerzo la salida
			}
		}	
	}

    //endfree(lista, trampa);
    // genera problema por double free
    // -----------------------------------------------------------
    float t_paralelo = (float)(clock() - t1_sim);
    float t_secuencial;
    printf("\n\n");
    for (j=0; j<contador; j++){
    	t_secuencial += lista[j]->t_total;
    	mostrar_tarea(lista[j]);
    	printf("demoró %f ", lista[j]->t_total);
    	printf("y terminó con código %d ", lista[j]->exit_code);
    	printf("en %d intento(s)\n", lista[j]->intentos);
    }
	printf("\n\nTerminado en %f paralelo y %f secuencial\n", t_paralelo, t_secuencial);
    return 0;
}