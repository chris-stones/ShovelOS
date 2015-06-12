#include <file/file.h>
#include <stdint.h>

char * fgets(char * s, size_t size, file_itf file) {

	char *const sstart = s;

	if(size < 1)
		return 0;

	while(size > 1) {

		char c = 0;
		int  e = (*file)->read(file, &c, 1);

		if(e == 1) {
			if(c != '\0') {
				*s++ = c;
				--size;
			}
			if(c=='\0' || c=='\n' || c=='\r')
				break;
		}
	}
	*s++ = '\0';
	return sstart;
}

