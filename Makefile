CC = gcc

CFLAGS = -Wall

.PHONY = main clean

SRCS = main.c standio.c video.c
OBJS  = main.o standio.o video.o
DEPS =  standio.h video.h


# tu file .c tao file .o

# tu file .o tao ra file main to execute
main: $(OBJS) 
	@echo "==========BUILD============"
	$(CC) -o $@ $^ $(CFLAGS) 

%.o: %c $(DEPS)
	@echo "======CREATING OBJECT======"
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	@echo "========REMOVE=========="
	rm -r *.o main
	