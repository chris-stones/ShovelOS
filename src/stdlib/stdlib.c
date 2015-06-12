
#include "stdlib.h"
#include <stdint.h>

void memset(void * _p, int _c, size_t size) {

	uint8_t * p = (uint8_t *)_p;
	uint8_t   c = (uint8_t  )_c;
	while(size--)
		*p++ = c;
}

size_t strlen(const char * const string) {

	size_t s = 0;
	while(string[s] != '\0')
		++s;
	return s;
}

static uint8_t isnum(char c) {

        return c >= '0' && c <= '9';
}

uint8_t isdigit(char c) {

	return isnum(c);
}

uint8_t isspace(char c) {
	switch(c) {
	default:
		return 0;
	case ' ':
	case '\f':
	case '\n':
	case '\r':
	case '\t':
	case '\v':
		return 1;
	}
}

static uint64_t declen(const char *str) {

        uint64_t ret=0;

        if(*str == '-') {
                if(isnum(*(str+1))) {
                        ret+=2;
                        str+=2;
                }
                else
                        return 0;
        }

        while(isnum(*str++))
                ret++;

        return ret;
}


sint64_t atoq(const char * str) {

        sint32_t ret    = 0;
        uint8_t negflag = 0;
        uint16_t power  = 1;

        for(const char *c = str + declen(str)-1; c>=str; c--, power*=10)
                switch(*c) {
                        default:
                                return negflag ? -ret : ret;
                        case '-':
                                if(negflag)
                                        return 0;
                                negflag = 0;
                                break;

                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                                ret += power * ((uint64_t)(*c - '0'));
                                break;
                }

        return negflag ? -ret : ret;
}

sint64_t atoll(const char * str) {
        return atoq(str);
}

sint32_t atoi(const char * str) {

        sint64_t r64 = atoq(str);
        sint32_t r32 = r64 & 0x7fffffff;

        return r64<0 ? -r32 : r32;
}

