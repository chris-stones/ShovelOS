
# DONT delete the .o files ( THIN archive requires them! )
#.SECONDARY:

SRC_S = entry.S
SRC_C = arm.c

LIBS =  mm/$(VMSA)/vmsa.o exceptions/exceptions.o

OBJ = $(SRC_S:.S=.S.o) $(SRC_C:.c=.c.o)

arch.o : $(OBJ) $(LIBS)
	$(LD) -r -o $@ $(OBJ) $(LIBS)

mm/$(VMSA)/vmsa.o :
	make -C mm/$(VMSA) vmsa.o  
	
%.c.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
        
%.S.o : %.S
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	make -C mm/$(VMSA) clean
	make -C exceptions clean
	rm -f *.o
	rm -f *.a
