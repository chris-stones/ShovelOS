
SRC_C = devices.c

OBJ = $(SRC_C:.c=.c.o)

%.o : $(OBJ)
	$(LD) -r -o $@ $(OBJ)
	
%.c.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
        
%.s.o : %.s
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o
