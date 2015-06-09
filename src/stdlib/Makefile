# DONT delete the .o files ( THIN archive requires them! )
#.SECONDARY:

SRC_C = stdlib.c fprintf.c

OBJ = $(SRC_C:.c=.c.o)

#%.a : $(OBJ)
#	$(AR) csrT $@ $(OBJ) $(LIBS)

stdlib.o : $(OBJ)
	$(LD) -r -o $@ $(OBJ) 
	
%.c.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
        
%.s.o : %.s
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o
	rm -f *.a
	rm -f $(OBJ)

