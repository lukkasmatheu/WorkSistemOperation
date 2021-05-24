CC = gcc

EXEC = disco 


# ppos_disk.c disk.c ppos_disk.o disk.o pingpong-disco1.o
SRC = pingpong-disco1.c ppos_disk.c disk.c ppos_disk.c disk.c ppos-core-task-aux.c 

OBJ =  pingpong-disco1.o ppos_disk.o disk.o ppos-core-task-aux.o ppos-all.o queue.o 

all: $(EXEC)
	@echo " "
	@echo "Copilação concluída!!"
	@echo " "

$(EXEC): $(OBJ)
	$(CC) -Wall -o $@ $^ -lrt

%.o: %.c
	$(CC) -Wall -o $@  -c $^ 
#$(CC) -Wall -o $@  -c $^ -lrt

clean: 
	rm -f pingpong-disco2.o ppos_disk.o disk.o  ppos-core-task-aux.o


cleanExec:
	rm -f $(EXEC)

