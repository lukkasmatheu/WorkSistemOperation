#include "ppos_disk.h"

#include <signal.h>

#include "ppos-core-globals.h"

disk_t disk;
pedido* fila_disco; // fila de pedidos

task_t taskDisk;

struct sigaction actionDisk ;

pedido* order;

void tratadorDisk(int signum){
   // seta flag de sinal gerado e acorda task de disco
   disk.sinal = 1; // seta flag de signal como 1 (true)
   task_resume(&taskDisk);

}

// modelo de escalonamento FCFS
pedido* pedidosFCFS(){
   return (pedido *) queue_remove((queue_t **)&fila_disco, (queue_t *)fila_disco); // remove o pedido da cabeça da fila
}

// modelo de escalonamento SSTF
pedido* pedidosSSTF(int head){
   
   if(fila_disco != NULL ){
      pedido *auxOrder = fila_disco;
      pedido *minorOrder = fila_disco;
      int auxValue = 0;         //como a cabeça percorreu então é incrementado/ (sendo a menor distancia 0 (head + 1) já estaria no bloco mais proximo)
      int minorDistance = abs(((head + 1 )- minorOrder->bloco)); 

      do{
         // pega a distancia entre a cabeça e o bloco
         auxValue = abs((( head + 1) - auxOrder->bloco));
         //verifica se esta distancia é menor
         if(auxValue < minorDistance){
            minorDistance = auxValue;
            minorOrder = auxOrder;
         }
         auxOrder = auxOrder->next;
      }while(auxOrder != fila_disco && auxOrder->next != auxOrder);

      return (pedido *) queue_remove((queue_t **) &fila_disco, (queue_t *) minorOrder);
   }
   return NULL;
}

// modelo de escalonamento CSCAN
pedido* pedidoCSCAN(int head){
   if(fila_disco != NULL ){
      
      pedido *auxOrder = fila_disco;
      pedido *nextOrder = fila_disco;
      pedido *minOrder = fila_disco;

      int auxFlag = 0;
      int minorBlock = 999;
      int minorNext = 999;

      do{
         // procura o bloco seguinte ao da cabeça em ordem crescente
         if(auxOrder->bloco > head && auxOrder->bloco < minorNext){
            minorNext = auxOrder->bloco;
            nextOrder = auxOrder;
            auxFlag = 1;
         }
         else if(auxOrder->bloco < minorBlock && auxFlag == 0){
            //busca o menor bloco em quanto não encontra um bloco sequente ao da cabeça em ordem crescente
            minorBlock = auxOrder->bloco;
            minOrder = auxOrder;
         }
         auxOrder = auxOrder->next;
      }while(auxOrder != fila_disco && auxOrder->next != auxOrder);

      // caso não encontre um bloco de maior posição que o da cabeça retorna ao menor bloco
      if(auxFlag == 0){
         nextOrder = minOrder;
      }
       
      return (pedido *) queue_remove((queue_t **)&fila_disco, (queue_t *) nextOrder);
   }
   return NULL;
}

// thread interface de gerencia do disco
void diskDriverBody (void * args){
   while (1) {
      // obtém o semáforo de acesso ao disco
      sem_down(&disk.sem_disk);

      // se foi acordado devido a um sinal do disco
      if (disk.sinal == 1){
         // acorda a tarefa cujo pedido foi atendido
         task_resume(disk.execDisc); //acorda a tarefa colocando a na fila de pronta
         disk.sinal = 0;
        
      }
      
      // se o disco estiver livre e houver pedidos de E/S na fila
      disk.state = disk_cmd(DISK_CMD_STATUS,0,0);
      
      if ((disk.state == DISK_STATUS_IDLE)){
        
         //basta alterar entre as formas de escalonamento abaixo
         
         pedido *order = pedidosFCFS();
         
         // pedido *order = pedidosSSTF(disk.head);
         
         // pedido *order = pedidoCSCAN(disk.head);

         if(order != NULL){            
            // solicita ao disco a operação de E/S, usando disk_cmd()
            int result = disk_cmd(order->tipo_pedido,order->bloco,order->buffer);

            if(result == 0){
               //contabiliza distancia percorrida pela cabeça do leitor
               disk.contador = disk.contador + abs(disk.head - order->bloco);
               disk.head = order->bloco;
               disk.execDisc = order->emissor; // seta o pedido que esta sendo atendido no momento

               printf("Agendamento concluido\n");  
               printf("Bloco percorridos até o momento - %d \n",disk.contador);
            }else{
               printf("Error ao realizar agendamento");
            }
         }
      }
      // libera o semáforo de acesso ao disco
      sem_up(&disk.sem_disk);
      // suspende a tarefa corrente (retorna ao dispatcher)
      task_yield();
   }
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

   fila_disco = NULL;

   // inicializa variaveis de disco
   disk.tarefas_disco = NULL;
   disk.execDisc= NULL;
   disk.head = 0; // cabeça do leitor inicia no bloco 0
   disk.contador = 0; // contador de blocos percorrido pela cabeça do leitor de disco
   disk.sinal = 0;
   sem_create(&disk.sem_disk, 1);

   taskDisk.activation_count = 0;
   taskDisk.activation_time = 0;
   taskDisk.next = taskDisk.prev = NULL;
   taskDisk.state = PPOS_TASK_STATE_NEW;
   
   // • Uma tarefa gerenciadora do disco;
   task_create(&taskDisk,diskDriverBody,"Disk Manager");

   actionDisk.sa_handler = tratadorDisk ;
   sigemptyset (&actionDisk.sa_mask) ;
   actionDisk.sa_flags = 0 ;
   if (sigaction (SIGUSR1, &actionDisk, 0) < 0){
      perror ("Erro em sigaction: ") ;
      exit (1) ;
   }

   return 0;
}

// leitura de um bloco, do disco para o buffer
int disk_block_read (int block, void *buffer) {
  
   sem_down(&disk.sem_disk);

   // inclui o pedido na fila_disco
   pedido pedidoLeitura;
   pedidoLeitura.bloco = block;
   pedidoLeitura.buffer = buffer;
   pedidoLeitura.emissor = taskExec;
   pedidoLeitura.tipo_pedido = DISK_CMD_READ ;
   pedidoLeitura.prev = NULL;
   pedidoLeitura.next = NULL;
   
   queue_append((queue_t **) &fila_disco, (queue_t *) &pedidoLeitura);

  if (taskDisk.state == PPOS_TASK_STATE_SUSPENDED || taskDisk.state == 's'){
      // acorda o gerente de disco (põe ele na fila de prontas)
      task_resume(&taskDisk);
   }
 
   // libera semáforo de acesso ao disco
   sem_up(&disk.sem_disk);
   

   // suspende a tarefa corrente (retorna ao dispatcher)
   task_suspend(taskExec,&disk.tarefas_disco);
   task_yield();
   return 0;
}


// escrita de um bloco, do buffer para o disco
int disk_block_write (int block, void *buffer) {
   
   // obtém o semáforo de acesso ao disco
   sem_down(&disk.sem_disk);

 
   // inclui o pedido na fila_disco
   pedido pedidoEscrita;
   pedidoEscrita.bloco = block;
   pedidoEscrita.buffer = buffer;
   pedidoEscrita.emissor = taskExec;
   pedidoEscrita.tipo_pedido = DISK_CMD_WRITE;
   pedidoEscrita.prev = NULL; 
   pedidoEscrita.next = NULL;

   queue_append((queue_t **) &fila_disco, (queue_t *) &pedidoEscrita);

   if (taskDisk.state == PPOS_TASK_STATE_SUSPENDED || taskDisk.state == 's'){
      // acorda o gerente de disco (põe ele na fila de prontas)
      
      task_resume(&taskDisk);
   }
 
   // libera semáforo de acesso ao disco
   sem_up(&disk.sem_disk);

   // suspende a tarefa corrente (retorna ao dispatcher)
 
   task_suspend(taskExec,&disk.tarefas_disco);
   task_yield();
   return 0;
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