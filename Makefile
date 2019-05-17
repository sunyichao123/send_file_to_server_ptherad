SOURCES = main.c ./mongoose/mongoose.c tcp_client_optional_mode.c
CFLAGS = -g -W -Wall -pthread 
PROG = tcp_client_optional_mode

$(PROG):$(SOURCES)
	gcc $(SOURCES) -o $@ $(CFLAGS)

clean:
	rm -rf *.o a.out $(PROG)
