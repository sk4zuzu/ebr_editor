    CC = gcc
   OUT = ebr_editor
  OBJS = $(patsubst %.c,%.o, $(wildcard *.c))
CFLAGS = -fpack-struct -g

all: $(OUT)

$(OUT): $(OBJS)
	$(CC) -static -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	-rm $(OBJS) $(OUT)

