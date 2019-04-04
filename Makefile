CC = gcc
BUILD_DIR = ./build

CFLAGS = -Wall

.PHONY = main clean

#SRCS = main.c standio.c video.c proc_video.c
#OBJS  = main.o standio.o video.o proc_video.o
SRCS = $(wildcard *.c)
OBJS = $(addprefix $(BUILD_DIR)/, $(patsubst %.c, %.o, $(SRCS)))

# tu file .c tao file .o

# tu file .o tao ra file main to execute
main: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) -lm

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS) 

clean:
	test -d $(BUILD_DIR)/
	rm -r $(BUILD_DIR) main
	
