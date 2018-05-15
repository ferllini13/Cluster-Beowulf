CC = mpic++
CFLAGS = -c
LFLAGS = -o
LIBS = `pkg-config --libs opencv`

OBJS = main.o

cluster: $(OBJS)
	mkdir -p build
	$(CC) $(LFLAGS) ./build/cluster $(OBJS) $(LIBS)
	rm *.o

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

make clean:
	rm ./build/cluster