#include "ppos_disk.h"
#include <sys/time.h>
#include <signal.h>

disk_t disk;
pedido* fila_disco; // fila de pedidos
extern task_t* taskDisk;

struct sigaction actionDisk ;

void tratadorDisk(int signum){
   
   disk.sinal = 1; // seta flag de signal como 1 (true)
   task_resume(taskDisk);

}

int disk_mgr_init (int *numBlocks, int *blockSize){


   int initDisk = disk_cmd(DISK_CMD_INIT,0,0);

   if(initDisk != 0){
      printf("Error ao iniciar disco");
      return -1;
   }

   *blockSize = disk_cmd (DISK_CMD_BLOCKSIZE, 0, 0);

   *numBlocks = disk_cmd (DISK_CMD_DISKSIZE, 0, 0);

   if( *numBlocks == 0 || *blockSize == 0 ){
      printf("Error ao pegar tamanho do bloco e numero de blocos");
      return -1;
   }

   sem_create(&disk, 1);
   disk.busy = 0;

   actionDisk.sa_handler = tratadorDisk ;
   sigemptyset (&actionDisk.sa_mask) ;
   actionDisk.sa_flags = 0 ;
   if (sigaction (SIGUSR1, &actionDisk, 0) < 0){
      perror ("Erro em sigaction: ") ;
      exit (1) ;
   }

   task_create(taskDisk,diskDriverBody,"Disk Manager");
   return 0;

}

// leitura de um bloco, do disco para o buffer
int disk_block_read (int block, void *buffer) {
   
   sem_down(&disk.sem_disk);

   // inclui o pedido na fila_disco
   pedido pedidoLeitura;
   pedidoLeitura.bloco = block;
   pedidoLeitura.buffer = buffer;
   pedidoLeitura.tipo_pedido = DISK_CMD_READ ;
   pedidoLeitura.next = pedidoLeitura.prev = NULL;  
   queue_append((queue_t **) &fila_disco, (queue_t *) &pedidoLeitura);
 
  if (taskDisk->state == PPOS_TASK_STATE_SUSPENDED)
   {
      task_resume(taskDisk);
   }
 
   // libera semáforo de acesso ao disco
   sem_up(&disk.sem_disk);
   
   // suspende a tarefa corrente (retorna ao dispatcher)
   task_join(&taskDisp);
}


// escrita de um bloco, do buffer para o disco
int disk_block_write (int block, void *buffer) {
   
   // obtém o semáforo de acesso ao disco
   sem_down(&disk.sem_disk);

 
   // inclui o pedido na fila_disco
   pedido pedidoLeitura;
   pedidoLeitura.emissor = taskExec;
   pedidoLeitura.bloco = block;
   pedidoLeitura.buffer = buffer;
   pedidoLeitura.tipo_pedido = DISK_CMD_WRITE ;
   pedidoLeitura.next = pedidoLeitura.prev = NULL;  

   queue_append((queue_t **) &fila_disco, (queue_t *) &pedidoLeitura);

   if (taskDisk->state == PPOS_TASK_STATE_SUSPENDED)
   {
      task_resume(taskDisk);
   }
 
   // libera semáforo de acesso ao disco
   sem_up(&disk.sem_disk);

   // suspende a tarefa corrente (retorna ao dispatcher)
   // task_join(taskExec);
   // task_switch(&taskDisp);
   task_join(&taskDisp);
}

// thread interface de gerencia do disco
void diskDriverBody (void * args)
{
   while (1) 
   {
      // obtém o semáforo de acesso ao disco
      sem_down(&disk.sem_disk);
      // se foi acordado devido a um sinal do disco
      if (disk.sinal == 1)
      {
         // acorda a tarefa cujo pedido foi atendido
         task_resume(fila_disco);
         queue_remove((queue_t **)&fila_disco, (queue_t *)fila_disco); // remove tarefa da fila 
         disk.busy = 0;
      }
 
      // se o disco estiver livre e houver pedidos de E/S na fila
      if (disk.busy != 1 && (queue_size(fila_disco) != 0))
      {
         // escolhe na fila o pedido a ser atendido, usando FCFS
         pedido* order = fila_disco ;

         // solicita ao disco a operação de E/S, usando disk_cmd()
         disk_cmd(order->tipo_pedido,order->bloco,order->buffer);
         disk.busy = 1;
      }
 
      // libera o semáforo de acesso ao disco
      sem_up(&disk.sem_disk);
      // suspende a tarefa corrente (retorna ao dispatcher)
      //  task_join(taskExec);
      task_join(&taskDisp);
      //  task_switch(&taskDisp);
   }
}


/*
   A gerência das operações de entrada/saída em disco consiste em implementar:
• Uma tarefa gerenciadora do disco;
• Uma função para tratar os sinais SIGUSR1 gerados pelo disco, que acorda a tarefa gerenciadora de
disco quando necessário;
• Uma fila de pedidos de acesso ao disco; cada pedido indica a tarefa solicitante, o tipo de pedido
(leitura ou escrita), o bloco desejado e o endereço do buffer de dados;
• As funções de acesso ao disco oferecidas às tarefas através do arquivo ppos_disk.h (disk_mgr_init,
disk_block_read e disk_block_write).*/