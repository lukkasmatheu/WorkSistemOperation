#include "ppos.h"
#include "ppos-core-globals.h"


// First Come, First Served (FCFS)

task_t * schedulerFCFS(){
    if ( readyQueue != NULL ) {
        task_t *auxiliar = readyQueue; 
        queue_remove((queue_t **)&readyQueue, (queue_t *) auxiliar);
        return auxiliar;
    }
    return NULL;
}


task_t * scheduler() {
    if ( readyQueue != NULL ) {
        task_t *minorPrio = readyQueue; // menor prioridade
        task_t *auxiliar = readyQueue; // fila auxiliar para percorrer fila de tarefas prontas
        do{ 
            //seleciona a menor prioridade , das tarefas prontas e escolhe a com menor prioridade para executar
            if(auxiliar->prioridade <= minorPrio->prioridade ) { 
                minorPrio = auxiliar;
            }

            auxiliar = auxiliar->next;
        }while( auxiliar != readyQueue);
            
        do{
            //verifica se prioridade não vai estrapolar o MINPRIO (valor minimo de prioridade)
            if((auxiliar->prioridade + ENVELHECIMENTO) > MINPRIO ) {
                auxiliar->prioridade += ENVELHECIMENTO;
            }
            auxiliar = auxiliar->next;
        }while( auxiliar != readyQueue);

        minorPrio->prioridade = minorPrio->prioridadeEstatica;
        minorPrio->state = PPOS_TASK_STATE_EXECUTING;
        
        queue_remove((queue_t **)&readyQueue, (queue_t *) minorPrio);
        
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