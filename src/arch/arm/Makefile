
# DONT delete the .o files ( THIN archive requires them! )
#.SECONDARY:

SRC_S = entry.S

LIBS =  mm/$(VMSA)/vmsa.o

OBJ = $(SRC_S:.S=.S.o)

arch.o : $(OBJ) $(LIBS)
	$(LD) -r -o $@ $(OBJ) $(LIBS)

mm/$(VMSA)/vmsa.o :
	make -C mm/$(VMSA) vmsa.o  
	
%.c.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
        
%.S.o : %.S
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o
	rm -f *.a
	rm -f $(OBJ)

