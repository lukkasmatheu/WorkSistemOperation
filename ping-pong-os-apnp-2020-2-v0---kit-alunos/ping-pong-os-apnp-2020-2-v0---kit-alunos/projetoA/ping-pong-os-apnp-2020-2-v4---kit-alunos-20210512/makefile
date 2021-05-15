CC = gcc

EXEC = disco 

SRC = pingpong-disco1.c ppos_disk.c disk.c  ppos-core-task-aux.c 

OBJ = ppos_disk.c disk.c pingpong-disco1.o ppos-core-task-aux.o ppos-all.o queue.o 

all: $(EXEC)
	@echo " "
	@echo "Copilação concluída!!"
	@echo " "

$(EXEC): $(OBJ)
	$(CC) -Wall -o $@ -DDEBUG $^ -lrt

%.o: %.c
	$(CC) -Wall -o $@  -c $^ -lrt
# $(CC) -Wall -o $@  -c -DDEBUG $^ -lrt



clean: 
	rm -f pingpong-scheduler.o  ppos-core-scheduler.o ppos-core-task-aux.o


cleanExec:
	rm -f $(EXEC)

