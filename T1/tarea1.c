#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


// ESTRUCTURAS
typedef enum { false, true } bool;

typedef struct process {
	char nombre[256];
	int estado;
	int num_bursts;
	int i_burst;
	int* bursts_list;
	int pid;
	int turnos_cpu;
	int bloqueos;
	int t_inicio;
	int waiting_time;
	int response_time;
	int turnaround_time;
	struct process* next;
} proceso;


typedef struct ListaLigada {
	proceso* head;
	proceso* tail;
	int length;
} Queue;

// VARIABLES
// variables usadas por todo el programa
int i;
int n_queues;
Queue** colas;
proceso* current;
const char* v1 = "v1";
const char* v3 = "v3";
const char* version;
// variables globales simulación
int t = 0;
int q;
int q_left;  // contador para descontar q cada vez
bool running = false;
int S;

// FUNCIONES
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
    return pp;
}

void add_burst(proceso* p, int i, int b) {
    p -> bursts_list[i] = b;
}


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
                if (current == cola -> tail) {
                    cola -> tail = p;
                    break;
                }
                current = current -> next;
            }
            p -> next = prev -> next;
            prev -> next = p;
        }
    }
    cola -> length ++;
}

void insert_head(int icola, proceso* pp) {
    Queue* cola = colas[icola];
    if (cola -> length == 0) {
        append_to_queue(icola, pp);
        return;
    }
    proceso* current_head = cola -> head;
    cola -> head = pp;
    pp -> next = current_head;
    cola -> length++;
}

void append_cola(int ifirst, int ilast) {
    Queue* colafirst = colas[ifirst];
    Queue* colalast = colas[ilast];
    append_to_queue(ifirst, colalast -> head);
    colafirst -> tail = colalast -> tail;
    colafirst -> length += colalast -> length - 1;
    colalast -> length = 0;
    proceso* nothing;
    colalast -> head = nothing;
    colalast -> tail = nothing;
}

void send_to_tail(proceso* pid) {
    if (i < n_queues + 2 - 1) {
        append_to_queue(i + 1, pid);
        printf("at queue %d\n", i);
    }
    else {
        append_to_queue(i, pid);
        printf("at queue %d\n", i - 1);
    }
}

void p_arrive() {
    proceso* pid_in;
    pid_in = pop(1);
    append_to_queue(2, pid_in);
    printf("t=%d: ARRIVE: ready %s at queue 1\n", t, pid_in -> nombre);
}

void scheduler() {
    proceso* new_runner;
    for (i = 2; i < n_queues + 2; i++) {
        if (colas[i] -> length == 0) {
            continue;
        }
        if (colas[i] -> length > 0) {
            new_runner = colas[i] -> head;
            new_runner -> estado = 2;
            printf("t=%d: SCHEDULER: running %s from queue %d\n", t, new_runner -> nombre, i - 1);
            new_runner -> turnos_cpu++;
            if (new_runner -> response_time == -1) {
                new_runner -> response_time = new_runner -> waiting_time;
            }
            if (!strcmp(version, v3)) {
                q_left = (i - 1) * q;  // por enunciado
            }
            else {
                q_left = q;
            }
            running = true;
            break;
        }
    }
}

void check_S() {
    if (t % S != 0 || t == 0 || n_queues == 1) {
        return;
    }
    printf("t=%d: AGING: all ready at queue 1\n", t);
    proceso* runpid;
    for (i = 3; i < n_queues + 2; i++) {
        if (colas[i] -> length == 0) {
            continue;
        }
        if (colas[i] -> head -> estado == 2) {
            proceso* runpid = pop(i);
            insert_head(2, runpid);
        }
        append_cola(2, i);
    }
}

void print_queue(int icola) {
    if (colas[icola] -> length == 0) {
        return;
    }
    Queue* cola = colas[icola];
    current = cola -> head;
    while (current != cola -> tail) {
        if (current -> response_time == -1) {
            printf("%s:\nTurnos de CPU: %d\nBloqueos: %d\nTurnaround time: %d\nResponse time: -\nWaiting time: %d\n", 
            current -> nombre, current -> turnos_cpu, current -> bloqueos, current -> turnaround_time, 
            current -> waiting_time);
        }
        else {
            printf("%s:\nTurnos de CPU: %d\nBloqueos: %d\nTurnaround time: %d\nResponse time: %d\nWaiting time: %d\n", 
            current -> nombre, current -> turnos_cpu, current -> bloqueos, current -> turnaround_time, 
            current -> response_time, current -> waiting_time);
        }
        printf("\n");
        current = current -> next;
    }
    current = cola -> tail;
    printf("%s:\nTurnos de CPU: %d\nBloqueos: %d\nTurnaround time: %d\nResponse time: %d\nWaiting time: %d\n", 
            current -> nombre, current -> turnos_cpu, current -> bloqueos, current -> turnaround_time, 
            current -> response_time, current -> waiting_time);
}

void print_sim() {
    printf("\nProcesos terminados: %d\n", colas[0] -> length);
    printf("Tiempo total: %d\n\n", t - 1);

    for (i = 0; i < n_queues + 2; i++) {
         print_queue(i);
    }
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
        int count = 0;
        while (current != cola -> tail) {
            count++;
            free(current -> bursts_list);
            to_free = current;
            if (current == cola -> tail) {
                free(to_free);
                break;
            }
            current = current -> next;
            free(to_free);
        }
        current = cola -> tail;
        free(current -> bursts_list);
        free(current);
        free(cola);
    }
    free(colas);
}

void handler() {
    sleep(1);
    print_sim();
    exit(0);
}

int main(int argc, char const *argv[]) {

    signal(SIGINT, handler);

	if(argc < 5)
    {
      printf("Mal uso de parámetros.\n");
      exit(2);
    }

    version = argv[1];
    const char* filename = argv[2];
    q = atoi(argv[3]);
    q_left = q; // para simulación
    n_queues = atoi(argv[4]);

    if (strcmp(version, v1))
    {
        if (argc < 6) {
            printf("Mal uso de parámetros.\n");
            exit(2);
        }
        else {
            S = atoi(argv[5]);
        }
    }

    colas = malloc(sizeof(Queue*)*(n_queues + 2));

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
        pcs -> estado = 1;
    }
    fclose(fp);

    ////////////////////////////////////////////////
    // SIMULACIÓN    ///////////////////////////////
    ////////////////////////////////////////////////
    int np = colas[1] -> length;
    int this_burst;
    proceso* runpid;
    
    while (colas[0] -> length < np) {
        
        // printf("%d\n", t);

        if (colas[1] -> length > 0 &&  // ve si llegó algún proceso
            colas[1] -> head -> t_inicio == t) {
            p_arrive();
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
                        runpid -> estado = 1;
                        pop(i);
                        if (runpid -> bursts_list[this_burst] == 0) {
                            if (this_burst == runpid -> num_bursts - 1) {
                                runpid -> estado = 0;
                                printf("t=%d: OUT OF q & BURSTS: finished %s\n", t, runpid -> nombre);
                                runpid -> turnaround_time = t - runpid -> t_inicio;
                                append_to_queue(0, runpid);
                            }
                            else {
                                printf("t=%d: OUT OF q & BURST: ready %s ", t, runpid -> nombre);
                                runpid -> i_burst++;
                                send_to_tail(runpid);
                            }
                        }
                        else {
                            printf("t=%d: OUT OF q: ready %s ", t, runpid -> nombre);
                            send_to_tail(runpid);
                        }
                        running = false;
                    }
                    else if (runpid -> bursts_list[this_burst] == 0) {
                        runpid -> estado = 1;
                        pop(i);
                        if (this_burst == runpid -> num_bursts - 1) {
                            runpid -> estado = 0;
                            printf("t=%d: OUT OF BURSTS: finished %s\n", t, runpid -> nombre);
                            runpid -> turnaround_time = t - runpid -> t_inicio;
                            append_to_queue(0, runpid);
                        }
                        else {
                            printf("t=%d: OUT OF BURST: ready %s at queue %d\n", t, runpid -> nombre, i - 1);
                            runpid -> i_burst++;
                            append_to_queue(i, runpid);
                        }
                        running = false;
                    }
                    break;
                }
            }
        }

        if (strcmp(version, v1)) {  // aplica S antes de Scheduling
            check_S();
        }     

        if (!running) {  // scheduling
            scheduler();
        }

        if (!running) {
            if (colas[0] -> length < np) {
                printf("t=%d: idle\n", t);
            }
        }

        // subir waiting times a procesos ready
        for (i = 2; i < n_queues + 2; i++) {
            Queue* cola = colas[i];
            if (cola -> length == 0) {
                continue;
            }
            current = cola -> head;
            while (current != cola -> tail) {
                if (current -> estado == 1) {
                    current -> waiting_time++;
                }
                current = current -> next;
            }
            current = cola -> tail;
            if (current -> estado == 1) {
                current -> waiting_time++;
            }
            current = current -> next;
        }

        t++;
        // sleep(1);

    }

    print_sim();

    endfree();

	return 0;
}