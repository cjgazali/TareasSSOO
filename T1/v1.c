#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef enum { false, true } bool;

typedef struct process {  // v3 tendrá quantum
	char nombre[256];
	int estado; // o con números?
	int num_bursts;
	int i_burst;  // -1 init. En primera atención comienza a atender desde 0
	int* bursts_list;
	int pid;
	int turnos_cpu;
	int bloqueos;
	int t_inicio;
	int waiting_time;
	int response_time;
	int turnaround_time;
	// int t_last_attention;
	struct process* next;
} proceso;


typedef struct ListaLigada {
	proceso* head;
	proceso* tail;
	int length;
} Queue;


typedef struct {
	proceso** procesos;
	int length;
} ArregloProcesos;


proceso* create_process(int id, char* nombre, int ti, int n) {
    proceso* pp = malloc(sizeof(proceso));
    strcpy(pp -> nombre, nombre);
    pp -> estado = 1;  // 1: ready - 2: running - 0: finished
    pp -> num_bursts = n;
    pp -> i_burst = 0;
    pp -> bursts_list = malloc(sizeof(int)*n);
    pp -> pid = id;
    pp -> turnos_cpu = 0;
    pp -> bloqueos = 0;
    pp -> t_inicio = ti;
    pp -> waiting_time = 0;
    pp -> response_time = -1;  // primera atención wait=resp / para stats if -1
    pp -> turnaround_time = 0;
    // pp -> t_last_attention = -1;  // para stats if -1
    // pp -> process* next;
    return pp;
}

void add_burst(proceso* p, int i, int b) {
    p -> bursts_list[i] = b;
}

int i;
int n_queues;
Queue** colas;
proceso* current;


void append_to_queue(int i_queue, proceso* p) {
    Queue* cola = colas[i_queue];
    if (cola -> length == 0) {
        cola -> head = p;
    }
    else {
        cola -> tail -> next = p;
    }
    cola -> tail = p;
    cola -> length ++;
}

proceso* pop(int i_queue) {
    Queue* cola = colas[i_queue];
    proceso* pp = cola -> head;
    cola -> head = pp -> next;
    proceso* nothing;
    pp -> next = nothing;
    cola -> length--;
    return pp;
}

void ordered_insert_to_queue(int i_queue, proceso* p) {
    Queue* cola = colas[i_queue];
    if (cola -> length == 0) {
        cola -> head = p;
        cola -> tail = p;
    }
    else {
        current = colas[i_queue] -> head;
        if (p -> t_inicio < current -> t_inicio) {
            p -> next = cola -> head;
            cola -> head = p;
        }
        else {
            proceso* prev;
            while (p -> t_inicio > current -> t_inicio) {
                prev = current;
                current = current -> next;
                if (!current) {
                    cola -> tail = p;
                    break;
                }
            }
            p -> next = prev -> next;
            prev -> next = p;
        }
    }
    cola -> length ++;
}


void endfree() {

    proceso* to_free;

    for (i = 0; i < n_queues + 2; i++) {
        Queue* cola = colas[i];
        if (cola -> length == 0) {
            free(cola);
            continue;
        }
        current = cola -> head;
        while (current) {
            free(current -> bursts_list);
            to_free = current;
            current = current -> next;
            free(to_free);
        }
        free(cola);
    }

    free(colas);
}


int main(int argc, char const *argv[]) {
	if(argc != 5)  // depende de versión
    {
      printf("Mal uso de parámetros.\n");
      exit(2);
    }

    // versión se asume 1...
    const char* filename = argv[2];
    int q = atoi(argv[3]);
    n_queues = atoi(argv[4]);

    colas = malloc(sizeof(Queue*)*(n_queues + 2));

    // Queue* colaFI = malloc(sizeof(Queue));
    // colaFI -> length = 0;
    // colas[0] = colaFI;

    // Queue* StartQueue = malloc(sizeof(Queue));
    // StartQueue -> length = 0;
    // colas[1] = StartQueue;

    for (i = 0; i < n_queues + 2; i++) {  // incluye 0:finished - 1:iniciales
    	Queue* NuevaCola = malloc(sizeof(Queue));
        NuevaCola -> length = 0;
    	colas[i] = NuevaCola;
    }

    // leer archivo
    FILE* fp;
    char p_name[256];
    int init_time;
    int n_bursts;
    int id;

    fp = fopen(filename, "r");
    while ((fscanf(fp, "%s %d %d", p_name, &init_time, &n_bursts)) == 3) {
     	proceso* pcs = create_process(id, p_name, init_time, n_bursts);
        id++;

        for (i = 0; i < n_bursts; i++) {
            int burst;
            fscanf(fp, "%d", &burst);
            add_burst(pcs, i, burst);
        }

        ordered_insert_to_queue(1, pcs);
        pcs -> estado = 2;
    }
    fclose(fp);


    // SIMULACIÓN
    int t = 0;
    int np = colas[1] -> length;
    int q_left = q;
    int this_burst;
    proceso* pid_in;
    proceso* runpid;
    proceso* new_runner;
    bool running = false;

    while (colas[0] -> length < np) {
        if (colas[1] -> head -> t_inicio == t) {
            pid_in = pop(1);
            append_to_queue(2, pid_in);
        }

        if (running) {  // cuenta run tick
            for (i = 2; i < n_queues + 2; i++) {
                if (colas[i] -> length == 0) {
                    continue;
                }
                if (colas[i] -> head -> estado == 2) {


                    runpid = colas[i] -> head;
                    this_burst = runpid -> i_burst;

                    q_left--;
                    runpid -> bursts_list[this_burst]--;

                    if (q_left == 0) {
                        runpid -> bloqueos++;
                        pop(i);
                        if (i < n_queues + 2 - 1) {
                            append_to_queue(i + 1, runpid);
                        }
                        else {
                            append_to_queue(i, runpid);
                        }
                        if (runpid -> bursts_list[this_burst] == 0) {
                            if (this_burst == runpid -> num_bursts - 1) {
                                runpid -> estado = 0;
                                runpid -> turnaround_time = t - runpid -> t_inicio;
                                append_to_queue(0, runpid);
                            }
                            else {
                                runpid -> i_burst++;
                            }
                        }
                        running = false; // ??
                    }
                    else if (runpid -> bursts_list[this_burst] == 0) {
                        pop(i);
                        if (this_burst == runpid -> num_bursts - 1) {
                            runpid -> estado = 0;
                            runpid -> turnaround_time = t - runpid -> t_inicio;
                            append_to_queue(0, runpid);
                        }
                        else {
                            runpid -> i_burst++;
                            if (i < n_queues + 2 - 1) {
                                append_to_queue(i + 1, runpid);
                            }
                            else {
                                append_to_queue(i, runpid);
                            }
                        }
                        running = false; // ??
                    }
                    break;
                }
            }
        }        

        if (!running) {  // scheduling
            for (i = 2; i < n_queues + 2; i++) {
                if (colas[i] -> length == 0) {
                    continue;
                }
                if (colas[i] -> length > 0) {
                    new_runner = colas[i] -> head;
                    new_runner -> estado = 2;
                    new_runner -> turnos_cpu++;
                    // new_runner -> t_last_attention = t;
                    if (new_runner -> response_time == -1) {
                        new_runner -> response_time = new_runner -> waiting_time;
                    }
                    q_left = q;
                    running = true;
                    break;
                }
            }
        }

        if (!running) {
            // if (colas[0] -> length == np) {
            //     break;
            // }
            printf("idle\n");
        }

        // subir waiting times a procesos ready
        for (i = 2; i < n_queues + 2; i++) {
            Queue* cola = colas[i];
            if (cola -> length == 0) {
                continue;
            }
            current = cola -> head;
            while (current != cola -> tail -> next) {  // cuidado, pero si funciona revisar que todos sean así
                if (current -> estado == 1) {
                    current -> waiting_time++;
                }
                current = current -> next;
            }
        }

        printf("%d\n", t);
        t++;
        // sleep(1);

    }

    // imprime procesos ordenados y bursts
    current = colas[0] -> head;
    while (current) {
        printf("%s %d %d ", current -> nombre, current -> t_inicio, current -> num_bursts);
        for (i = 0; i < current -> num_bursts; i++) {
            printf("%d ", current -> bursts_list[i]);
        }
        printf("\n");
        current = current -> next;
    }

    endfree();


	return 0;
}