#include "queue.h"
#include <stdio.h>


void queue_append (queue_t **queue, queue_t *elem){
    // Condicoes a verificar, gerando msgs de erro:
    // - o elemento nao deve estar em outra fila    CHECK!!!!
    // - a fila deve existir                        CHECK!!!!
    // - o elemento deve existir                    CHECK!!!!
    // Insere um elemento no final da fila.         

    if(queue == NULL && elem == NULL){
        if(queue == NULL){
            printf("\nFila inexistente\n");
        }else{
            printf("\nElemento inexistente\n");
        }
    }else if(elem->prev == NULL && elem->next == NULL){
        if(*queue == NULL){
            *queue = elem ;
            (*queue)->prev = (*queue)->next = elem;
        }
        else{
            elem->prev = (*queue)->prev;
            (*queue)->prev = elem ;
            elem->next = *queue;
            elem->prev->next = elem;
        }
    }else{
        printf("\nElemento ja esta em outra fila\n");
    }
}

// Remove o elemento indicado da fila, sem o destruir.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - a fila nao deve estar vazia
// - o elemento deve existir
// - o elemento deve pertencer a fila indicada
// Retorno: apontador para o elemento removido, ou NULL se erro

queue_t *queue_remove (queue_t **queue, queue_t *elem) {
    return NULL;
}


void queue_print (char *name, queue_t *queue, void print_elem (void*) ) {

}

int queue_size (queue_t *queue) {
    if(!queue) return 0;

    queue_t *next;
    int i=0;
    next = queue;
    while(!next){
        next = next->next;
        i++;
    }
    
    return i;
}