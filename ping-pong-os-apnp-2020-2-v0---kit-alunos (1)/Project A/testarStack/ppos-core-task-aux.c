#include "ppos.h"
#include "ppos-core-globals.h"


// ****************************************************************************
// Coloque aqui as suas modificações, p.ex. includes, defines variáveis, 
// estruturas e funções

#include <sys/time.h>
#include <signal.h>

//#define DEBUG


struct sigaction action ;
struct itimerval timer ;

unsigned int systemTime = 0;

void tratador (int signum);
unsigned int timePassed = 20 ; // Quantun das tarefas em execução
// ****************************************************************************



void tratador (int signum){
    systemTime++;
    if(taskExec != &taskDisp ){
        timePassed--;
        if(timePassed < 1){
            queue_append((queue_t **) &readyQueue, (queue_t *) taskExec);
            countTasks++;
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
    action.sa_handler = tratador ;
    sigemptyset (&action.sa_mask) ;
    action.sa_flags = 0 ;
    if (sigaction (SIGALRM, &action, 0) < 0)
    {
    perror ("Erro em sigaction: ") ;
    exit (1) ;
    }

    // ajusta valores do temporizador
    timer.it_value.tv_sec  = 0 ; 
    timer.it_interval.tv_sec  =  0; 
    timer.it_value.tv_usec = 1;    // primeiro disparo, em micro-segundos    
    timer.it_interval.tv_usec = 1000;   // disparos subsequentes, em micro-segundos 

    // arma o temporizador ITIMER_REAL (vide man setitimer)
    if (setitimer (ITIMER_REAL, &timer, 0) < 0)
    {
    perror ("Erro em setitimer: ") ;
    exit (1) ;
    }
    // put your customization here
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
    // put your customization here
#ifdef DEBUG
    printf("\ntask_create - AFTER - [%d]", task->id);
#endif
}

void before_task_exit () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_exit - BEFORE - [%d]", taskExec->id);
#endif
}

void after_task_exit () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_exit - AFTER- [%d]", taskExec->id);
#endif
}

void before_task_switch ( task_t *task ) {
    // put your customization here
    timePassed = 20;
#ifdef DEBUG
    printf("\ntask_switch - BEFORE - [%d -> %d]", taskExec->id, task->id);
#endif
}

void after_task_switch ( task_t *task ) {
     // setando novo quantun ao alterar tarefa
     
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
