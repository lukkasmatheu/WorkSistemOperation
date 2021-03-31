CC = gcc

EXEC = saida

SRC = pingpong-preempcao-stress.c ppos-core-scheduler.c ppos-core-scheduler.c

OBJ = pingpong-preempcao-stress.o ppos-core-sched.o ppos-core-task-aux.o ppos-core-task.o ppos-core-scheduler.o queue.o 

all: $(EXEC)
	@echo " "
	@echo "Copilação concluída!!"
	@echo " "

$(EXEC): $(OBJ)
	$(CC) -Wall -o $@ -DDEBUG $^ 

%.o: %.c
	$(CC) -Wall -o $@  -c $^
# $(CC) -Wall -o $@  -c -DDEBUG $^ 



clean: 
	rm -f pingpong-preempcao-stress.o ppos-core-scheduler.o ppos-core-task-aux.o


cleanExec:
	rm -f $(EXEC)

