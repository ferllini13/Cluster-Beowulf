CC = mpic++
CFLAGS = -c
LFLAGS = -o
LIBS = `pkg-config --libs opencv`

OBJS = main.o

cluster: $(OBJS)
	mkdir -p build
	$(CC) $(LFLAGS) ./build/cluster $(OBJS) $(LIBS)
	rm *.o

main.o: main.c
	$(CC) $(CFLAGS) main.c 

make clean:
	rm ./build/cluster