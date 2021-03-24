#include "ppos.h"
#include "ppos-core-globals.h"

task_t * scheduler() {
    // FCFS scheduler
    if ( readyQueue != NULL ) {
        return readyQueue;
    }
    return NULL;
}


void task_setprio (task_t *task, int prio) {
    if(prio > MAXPRIO || prio < MINPRIO)
        return;    
    if(!task){
        task->prioridade = prio ;
    }
    else{
        while(task->next != NULL && task->state != PPOS_TASK_STATE_EXECUTING){
            taskExec->prioridade = prio;
        }
    }
}

int task_getprio (task_t *task) {
    if(task!=NULL){
        return task->prioridade;
    }
    return 0;
}