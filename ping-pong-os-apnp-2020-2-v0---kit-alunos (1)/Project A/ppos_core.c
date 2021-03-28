#include "ppos.h"
#include "ppos-core-globals.h"

task_t taskMain;
task_t* taskExec;
task_t taskDisp;
task_t* readyQueue;
task_t* freeTask;
task_t* sleepQueue;
long nextid;
long countTasks;

void ppos_init (){
    
    before_ppos_init();
    
    taskExec = &taskMain;
    readyQueue = NULL;
    sleepQueue = NULL;
    countTasks= 0;
    taskMain.id = nextid;
    nextid++;
    
    setvbuf (stdout, 0, _IONBF, 0) ;
    
    after_ppos_init();
}

int task_create (task_t *task,void (*start_func)(void *), void *arg){
    before_task_create(task);
    if(task == NULL){
        printf("\nErro ao criar tarefa\n");
        return -1;
    }
    getcontext(&task->context);
    char *stack = malloc(STACKSIZE);
    if(stack){
        task->context.uc_stack.ss_sp = stack ;
        task->context.uc_stack.ss_size = STACKSIZE ;
        task->context.uc_stack.ss_flags = 0 ;
        task->context.uc_link = 0 ;
        task->id = nextid;
        task->prioridade=task->prioridadeEstatica = 0;
        task->state = PPOS_TASK_STATE_NEW;
    }else{
        printf("\nErro ao criar stack\n");
        return -1;
    }
    makecontext (&task->context, (void*)(*start_func), 1, arg) ;
    nextid++;   
    after_task_create(task);
    return (nextid - 1);
}

int task_id (){
   return taskExec->id;
}

void task_yield (){
    if(taskExec != &taskDisp && taskExec != &taskMain){
        task_t* auxiliar = taskExec;
        auxiliar->state= PPOS_TASK_STATE_READY;
        queue_append((queue_t **) &readyQueue, (queue_t *) taskExec);//coloca a tarefa em execução na fila de pronta
        countTasks++;
    }
    task_switch (&taskDisp); //retorna o processador para o dispath
}

void dispatcher(){
    // enquanto houverem tarefas de usuário   
    while(countTasks > 0){
      // escolhe a próxima tarefa a executar
      task_t *next_ready = scheduler();
      // escalonador escolheu uma tarefa?      
        if (next_ready != NULL){
          // transfere controle para a próxima tarefa
            task_switch (next_ready);   
        }
        countTasks = queue_size((queue_t *)readyQueue); // atualiza contagem de tarefas de usuario
    }
   // encerra a tarefa dispatcher
   task_exit(0);
}

void task_exit (int exitCode){
    before_task_exit ();
    freeTask = taskExec;
    taskExec->state = PPOS_TASK_STATE_TERMINATED;
    after_task_exit ();
}


int task_switch (task_t *task){
   before_task_switch(task);
    if(task){
       stack_t *auxiliar = taskExec ;
       taskExec = task;
       after_task_switch(auxiliar);
        return 0;
    }else{
        return -1;
    }
}


void task_suspend( task_t *task, task_t **queue ){
    
}
