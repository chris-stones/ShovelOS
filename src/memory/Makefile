# DONT delete the .o files ( THIN archive requires them! )
#.SECONDARY:

SRC_C = kmalloc.c \
		mem_cache.c \
		page.c \
		boot_pages.c

OBJ = $(SRC_C:.c=.c.o)

#%.a : $(OBJ)
#	$(AR) csrT $@ $(OBJ) $(LIBS)

memory.o : $(OBJ)
	$(LD) -r -o $@ $(OBJ) 

%.c.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
        
%.s.o : %.s
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o
	rm -f *.a
	rm -f $(OBJ)
