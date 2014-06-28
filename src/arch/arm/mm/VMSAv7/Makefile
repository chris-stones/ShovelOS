
# DONT delete the .o files ( THIN archive requires them! )
#.SECONDARY:

SRC_C = init_page_tables.c \
		page_tables.c \
		cache.c

OBJ = $(SRC_C:.c=.c.o)

#%.a : $(OBJ)
#	$(AR) csrT $@ $(OBJ) $(LIBS) 
	
vmsa.o : $(OBJ)
	$(LD) -r -o $@ $(OBJ)

%.c.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.S.o : %.S
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o
	rm -f *.a
	rm -f $(OBJ)

