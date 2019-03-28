CC = gcc

CFLAGS = -Wall

.PHONY = main clean

SRCS = main.c standio.c video.c proc_video.c
OBJS  = main.o standio.o video.o proc_video.o
DEPS =  standio.h video.h proc_video.h


# tu file .c tao file .o

# tu file .o tao ra file main to execute
main: $(OBJS) 
	@echo "==========BUILD============"
	$(CC) -o $@ $^ $(CFLAGS) -lm

build/%.o: %c $(DEPS)
	@echo "======CREATING OBJECT======"
	$(CC) -c -o $@ $< $(CFLAGS) 

clean:
	@echo "========REMOVE=========="
	rm -r *.o main
	