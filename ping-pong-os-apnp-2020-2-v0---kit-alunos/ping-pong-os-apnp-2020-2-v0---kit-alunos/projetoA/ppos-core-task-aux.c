#include "ppos.h"
#include "ppos-core-globals.h"


// ****************************************************************************
// Coloque aqui as suas modificações, p.ex. includes, defines variáveis, 
// estruturas e funções

#include <sys/time.h>
#include <signal.h>

//#define DEBUG
#define PREP 1

struct sigaction action ;
struct itimerval timer ;

unsigned int systemTime = 0;

int timeExecution = 0; 

extern unsigned char preemption;

void tratador (int signum);
unsigned int timePassed = 20 ; // Quantun das tarefas em execução
// ****************************************************************************



void tratador (int signum){
    systemTime++;
    if(taskExec->isUserTask != 1){
        timePassed--;
        timeExecution++;
        if(timePassed < 1){
            taskExec->timeExec += 20;
            timeExecution = 0;
            queue_append((queue_t **) &readyQueue, (queue_t *) taskExec);
            task_switch(&taskDisp);
        }
   }
}

void before_ppos_init () {
#ifdef DEBUG
    printf("\ninit - BEFORE");
#endif
}

void after_ppos_init () {
    
    if(PREP){
        action.sa_handler = tratador ;
        sigemptyset (&action.sa_mask) ;
        action.sa_flags = 0 ;
        if (sigaction (SIGALRM, &action, 0) < 0){
            perror ("Erro em sigaction: ") ;
            exit (1) ;
        }

        // ajusta valores do temporizador
        timer.it_value.tv_sec  = 0 ; 
        timer.it_interval.tv_sec  =  0; 
        timer.it_value.tv_usec = 2;    // primeiro disparo, em micro-segundos    
        timer.it_interval.tv_usec = 1000;   // disparos subsequentes, em micro-segundos 

        // arma o temporizador ITIMER_REAL
        if (setitimer (ITIMER_REAL, &timer, 0) < 0){
            perror ("Erro em setitimer: ") ;
            exit (1) ;
        }
    }
#ifdef DEBUG
    printf("\ninit - AFTER");
#endif
}

void before_task_create (task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_create - BEFORE - [%d]", task->id);
#endif
}

void after_task_create (task_t *task ) {
    
    task->totalTimeExec = systime();
    task->ativacoes = 0;
    if(task == &taskDisp){
        task->isUserTask = 1;
    }else{
        task->timeExec = 0 ;
        task->isUserTask = 0;
    }
    
    // put your customization here
#ifdef DEBUG
    printf("\ntask_create - AFTER - [%d]", task->id);
#endif
}

void before_task_exit () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_exit - BEFORE - [%d]\n ", taskExec->id);
#endif
}

void after_task_exit () {
    // put your customization here
    if(taskExec->isUserTask != 1 )taskExec->timeExec += timeExecution;
    int inicio = taskExec->totalTimeExec;
    int fim = systime();
    taskExec->totalTimeExec = fim - inicio;
    
   printf("\ntask_exit code:[%d] tempo total : %d | ativacoes: %d | tempo de processamento: %d  ", taskExec->id,taskExec->totalTimeExec, taskExec->ativacoes,taskExec->timeExec);
#ifdef DEBUG
    printf("\ntask_exit - AFTER- [%d]", taskExec->id);
#endif
}

void before_task_switch ( task_t *task ) {
    task->ativacoes++;
    timePassed = 20;
#ifdef DEBUG
    printf("\ntask_switch - BEFORE - [%d -> %d]", taskExec->id, task->id);
#endif
}

void after_task_switch ( task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_switch - AFTER - [%d -> %d]", taskExec->id, task->id);
#endif
}

void before_task_suspend( task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_suspend - BEFORE - [%d]", task->id);
#endif
}

void after_task_suspend( task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_suspend - AFTER - [%d]", task->id);
#endif
}

void before_task_resume(task_t *task) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_resume - BEFORE - [%d]", task->id);
#endif
}

void after_task_resume(task_t *task) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_resume - AFTER - [%d]", task->id);
#endif
}

void before_task_sleep () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_sleep - BEFORE - [%d]", taskExec->id);
#endif
}

void after_task_sleep () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_sleep - AFTER - [%d]", taskExec->id);
#endif
}

// retorna o relógio atual (em milisegundos)
unsigned int systime () {
    return systemTime;
}
