#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

typedef struct tarea { // sintaxis... t?
    char string[256]; // la tarea misma, que sale del texto
    int intentos; // parte en 0
    int id;
    struct tarea* next;
} tarea;

tarea* crear_tarea(int ID, char* linea) {
    tarea* pp = malloc(sizeof(tarea));
    strcpy(pp -> string, linea);
    pp -> intentos = 0; 
    pp -> id = ID;
    return pp;
}

void endfree() {
    // rellenar
}

void main(int argc, char const *argv[])
{
    int n = atoi(argv[2]);
    const char* filename = argv[];

    // leer archivo
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    fp = fopen(filename, "r");
    while ((read = getline(&line, &len, fp)) != -1) {
        //tarea* task = crear_tarea(ID, line);
        printf("%s",line);
    } 
    // printf("\n");
    fclose(fp);
    if (line){
        free(line);
    }

    //endfree();
}