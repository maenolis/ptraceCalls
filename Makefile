OBJS = picodb.o components.o
EXEC = picodb
GCCC = gcc -g -Wall -c
GCCO = gcc -g -Wall -o

$(EXEC): $(OBJS)
	$(GCCO) $(EXEC) $(OBJS)

picodb.o: picodb.c
	$(GCCC) picodb.c
	
components.o: components.c
	$(GCCC) components.c

clean:
	rm -rf $(OBJS) $(EXEC)
