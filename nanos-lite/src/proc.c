#include <proc.h>
#include <am.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
int free_pcb = 0;

PCB *find_free_pcb() {
    for (int i = 0; i < MAX_NR_PROC; i++) {
        if (!pcb[i].in_use) {
            pcb[i].in_use = 1;
            return &pcb[i];
        }
    }
    return NULL;
}

void switch_boot_pcb() {
    current = &pcb_boot;
}

void hello_fun(void *arg) {
    int j = 1;
    while (1) {
        printf("Hello World from Nanos-lite with arg '%d' for the ", *(int *)arg);
        printf("%dth time!\n", j);
        j++;
        yield();
    }
}

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void *entry, void *arg);

void init_proc() {
    // int *arg1 = malloc(sizeof(int));
    // *arg1 = 1;
    // int *arg2 = malloc(sizeof(int));
    // *arg2 = 2;
    // context_kload(&pcb[0], hello_fun, arg1);
    // context_kload(&pcb[1], hello_fun, arg2);
    switch_boot_pcb();

    Log("Initializing processes...");

    // load program here
    // naive_uload(NULL, "/bin/hello");
    // naive_uload(NULL, "/bin/dummy");
    // naive_uload(NULL, "/bin/file-test");
    // naive_uload(NULL, "/bin/timer-test");
    // naive_uload(NULL, "/bin/event-test");
    // naive_uload(NULL, "/bin/bmp-test");
    // naive_uload(NULL, "/bin/nslider");
    // naive_uload(NULL, "/bin/menu");
    // naive_uload(NULL, "/bin/nterm");
    // naive_uload(NULL, "/bin/bird");
    // naive_uload(NULL, "/bin/pal");
}

Context *schedule(Context *prev) {
    return NULL;
    current->cp = prev;
    current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
    return current->cp;
}
