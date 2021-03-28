#include "ppos.h"
#include "ppos-core-globals.h"
#define ENVELHECIMENTO -1

task_t * scheduler() {
    if ( readyQueue != NULL ) {
      task_t *minorPrio = readyQueue;
      task_t *auxiliar = readyQueue;
      do{
            if(auxiliar->prioridade < minorPrio->prioridade ) {
                minorPrio = auxiliar;
            }
            if((auxiliar->prioridade + ENVELHECIMENTO) > -20){
                auxiliar->prioridade += ENVELHECIMENTO;
            }
            auxiliar = auxiliar->next;
        }while( auxiliar != readyQueue);
        minorPrio->prioridade = minorPrio->prioridadeEstatica;
        queue_remove((queue_t **)&readyQueue, (queue_t *) minorPrio);
        countTasks--;
        minorPrio->joinQueue = minorPrio->queue = NULL;
        return minorPrio;
    }
    return NULL;
}


void task_setprio (task_t *task, int prio) {
    if(prio > MAXPRIO || prio < MINPRIO)
        return;    
    if(task){
        task->prioridadeEstatica = prio;
        task->prioridade = prio;
    }else{
       task->prioridadeEstatica = prio;
    }
    
}

int task_getprio (task_t *task) {
    if(task){
        return task->prioridade;
    }else{
        return taskExec->prioridade;
    }
    return 0;
}