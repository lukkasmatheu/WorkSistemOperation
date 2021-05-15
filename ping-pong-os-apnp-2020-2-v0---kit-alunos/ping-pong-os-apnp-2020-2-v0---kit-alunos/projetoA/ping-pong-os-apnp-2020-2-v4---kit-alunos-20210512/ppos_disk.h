// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DINF UFPR
// Versão 1.2 -- Julho de 2017

// interface do gerente de disco rígido (block device driver)

#ifndef __DISK_MGR__
#define __DISK_MGR__

#include "ppos_data.h" 
#include "disk.h"
#include "ppos.h"
// estruturas de dados e rotinas de inicializacao e acesso
// a um dispositivo de entrada/saida orientado a blocos,
// tipicamente um disco rigido.

// estrutura que representa um disco no sistema operacional
typedef struct
{
  int busy;         //indica se disco esta ocupado
  int sinal;        //flag sinal de disco
  semaphore_t sem_disk;
  // completar com os campos necessarios
} disk_t ;


//Uma fila de pedidos de acesso ao disco 
//cada pedido indica a tarefa solicitante, 
//o tipo de pedido (leitura ou escrita),
// o bloco desejado e o endereço do buffer de dados;
typedef struct{
  task_t* emissor; // tarefa emissora do pedido
  int tipo_pedido; // tipo de pedido E/S
  int bloco; // bloco da operação 
  char *buffer ; // buffer onde sera armazenado a leitura ou a escrita
  struct pedido* next; // encademento da fila de pedidos
  struct pedido* prev;
}pedido;


void diskDriverBody (void * args);

// inicializacao do gerente de disco
// retorna -1 em erro ou 0 em sucesso
// numBlocks: tamanho do disco, em blocos
// blockSize: tamanho de cada bloco do disco, em bytes
int disk_mgr_init (int *numBlocks, int *blockSize) ;

// leitura de um bloco, do disco para o buffer
int disk_block_read (int block, void *buffer) ;

// escrita de um bloco, do buffer para o disco
int disk_block_write (int block, void *buffer) ;

#endif