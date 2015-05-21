# DONT delete the .o files ( THIN archive requires them! )
#.SECONDARY:

SRC_C = uart/uart.c

OBJ = $(SRC_C:.c=.c.o)

omap36xx.o : $(OBJ)
	$(LD) -r -o $@ $(OBJ) 
	
%.c.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
        
%.s.o : %.s
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o
	rm -f *.a
	rm -f $(OBJ)
