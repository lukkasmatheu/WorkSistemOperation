CC = gcc

EXEC = contabprio

SRC = pingpong-disco1.c  ppos-core-scheduler.c ppos-core-task-aux.c

OBJ =  pingpong-contab-prio.o ppos-core-sched.o ppos-core-task-aux.o ppos-core-task.o ppos-core-scheduler.o queue.o 

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
	rm -f pingpong-contab-prio.o  ppos-core-scheduler.o ppos-core-task-aux.o


cleanExec:
	rm -f $(EXEC)

