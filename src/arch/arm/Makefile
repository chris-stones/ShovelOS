
# DONT delete the .o files ( THIN archive requires them! )
.SECONDARY:

SRC_S = entry.S

OBJ = $(SRC_S:.S=.S.o)

%.a : $(OBJ)
	$(AR) csrT $@ $(OBJ) $(LIBS) 
	
%.c.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
        
%.S.o : %.S
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o
	rm -f *.a
	rm -f $(OBJ)

