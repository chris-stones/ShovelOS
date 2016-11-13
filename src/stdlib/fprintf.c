
#include <stdint.h>
#include <stdlib.h>
#include <file/file.h>
#include <stdarg.h>

static int32_t _putc(file_itf file, char c) {

  return INVOKE(file,write,&c,1);
}

static int32_t _puts(file_itf file, const char * str) {

  int l = strlen(str);
  return INVOKE(file,write,str,l);
}

static sint32_t _nstrlen(sint64_t n, sint64_t base) {

	sint32_t ret;
	if(n<0) {
		ret  =  2;
		n   *= -1;
	}
	else
		ret = 1;

	while((n /= base)>0)
		ret++;

	return (ret);
}

static int32_t _padn(file_itf file, sint64_t n, uint32_t base, int32_t pad, int zlead, int leftalign) {

	int32_t l=0;
	if(!leftalign) {
		pad -= _nstrlen(n,base);
		const char padchar = zlead ? '0' : ' ';
		while(pad-- >0)
			l += _putc(file, padchar);
	}
	return l;
}

static int32_t  _padc(file_itf file, int32_t pad, int leftalign) {

	int32_t l=0;
	if(!leftalign) {
		pad -= 1;
		while(pad-- >0)
			l += _putc(file, ' ');
	}
	return l;
}

static int32_t  _pads(file_itf file, const char * str, int32_t pad, int leftalign) {

	int32_t l=0;
	if(!leftalign) {
		pad -= strlen(str);
		while(pad-- >0)
			l += _putc(file, ' ');
	}
	return l;
}

static int32_t __putxX(file_itf file, uint32_t n, uint8_t a) {

    int l=0;
    int16_t s;   // shift
    uint8_t  x;
    for(s=28; s>=0; s-=4)
        if((x = (n>>s)&15) || l || !s)
            l += _putc( file, x + ((x<10) ? '0' : (a-10)));

    return l;
}

static int32_t _putx(file_itf file, uint32_t n) {

	return __putxX(file, n,'a');
}

static int32_t _putX(file_itf file, uint32_t n) {

	return __putxX(file, n,'A');
}

static int32_t __putlxX(file_itf file, uint64_t n, uint8_t a) {

    int      l=0;
    int16_t s;   // shift
    uint8_t  x;
    for(s=60; s>=0; s-=4)
        if((x = (n>>s)&15) || l || !s)
            l += _putc(file, x + ((x<10) ? '0' : (a-10)));

    return l;
}

static int32_t _putlx(file_itf file, uint64_t n) {

	return __putlxX(file, n, 'a');
}

static int32_t _putlX(file_itf file, uint64_t n) {

	return __putlxX(file, n, 'A');
}

static int32_t _putu(file_itf file, uint32_t n) {

    uint32_t   d = 1000000000; // divisor
    int16_t   h;              // digit
    int16_t   l = 0;          // wrote length
    for(; d>=1; d/=10)
        if((h = ((n/d)%10)) || l || (d<=1)) {
            ++l;
            _putc(file, '0' + h);
            }
    return l;
}

static int32_t _putd(file_itf file, int32_t n) {

    if(n>=0)
        return _putu(file, (int32_t)n);

    return _putc(file, '-') + _putd(file, (int32_t)(-n));
}

static int32_t _putlu(file_itf file, uint64_t n) {

    uint64_t d  = 1000000000;
             d *= 1000000000;

    int16_t   h;              // digit
    int16_t   l = 0;          // wrote length
    for(; d>=1; d/=10)
        if((h = ((n/d)%10)) || l || (d<=1)) {
            ++l;
            _putc(file, '0' + h);
            }
    return l;
}

static int32_t _putll(file_itf file, int64_t n) {

    if(n>=0)
        return _putlu(file, (uint64_t)n);

    return _putc(file, '-') + _putlu(file, (uint64_t)(-n));
}

int32_t vfprintf(file_itf file, const char * format, va_list va) {

	size_t l = 0;
	int err = 0;

	while(!err && *format) {

		if(*format == '%') {

			int leftalign=0;
			int zlead=0;
			int32_t pad=0;

			format++;

			// Determine formatting.. alignment / padding.
			for(;;) {
				int _breakFlag=0;
				switch(*format) {
				case '0':
					zlead=1;
					format++;
					break;
				case '-':
					leftalign=1;
					format++;
					break;
				case '*':
					pad = va_arg(va,int32_t);
					if(pad<0) {
						pad=-pad;
						leftalign=1;
					}
					format++;
					break;
				default:
					if(isdigit(*format)) {
						pad = atoi(format);
						while(isdigit(*format))
							++format;
					}
					else
						_breakFlag=1;
				}
				if(_breakFlag)
					break;
			}
			if(leftalign)
				zlead=0; // DONT right pad numbers with zero, for obvious reasons.

			switch(*format++) {
			default:
				err = 1;
				break;
			case '%':
				l += _putc(file, '%');
				break;
			case 'c':
			{
				l += _padc(file, pad,   leftalign);
				l += _putc(file, (uint8_t)va_arg(va, int32_t));
				l += _padc(file, pad,  !leftalign);
				break;
			}
			case 'u':
			{
				uint32_t arg = va_arg(va,uint32_t);
				l += _padn(file, arg, 10, pad, zlead,  leftalign);
				l += _putu(file, arg);
				l += _padn(file, arg, 10, pad, zlead, !leftalign);
				break;
			}
			case 'i':
			case 'd':
			{
				int32_t arg = va_arg(va,int32_t);
				l += _padn(file, arg, 10, pad, zlead,  leftalign);
				l += _putd(file, arg);
				l += _padn(file, arg, 10, pad, zlead, !leftalign);
				break;
			}
			case 'p':
			{
				if(sizeof(void*) == 8) {
					uint64_t arg = va_arg(va, uint64_t);
					l += _padn(file, arg, 16, pad, zlead,  leftalign);
					l += _putlx(file,arg);
					l += _padn(file, arg, 16, pad, zlead, !leftalign);
					break;
				}
			}
			// no break - 32bit pointers can be handled by %x.
			case 'x':
			{
				uint32_t arg = va_arg(va,uint32_t);
				l += _padn(file, arg, 16, pad, zlead,  leftalign);
				l += _putx(file, arg);
				l += _padn(file, arg, 16, pad, zlead, !leftalign);
				break;
			}
			case 'X':
			{
				uint32_t arg = va_arg(va,uint32_t);
				l += _padn(file, arg, 16, pad, zlead,  leftalign);
				l += _putX(file, arg);
				l += _padn(file, arg, 16, pad, zlead, !leftalign);
				break;
			}
			case 's':
			{
				const char * str = va_arg(va,const char *);
				l += _pads(file, str, pad,  leftalign);
				l += _puts(file, str);
				l += _pads(file, str, pad, !leftalign);
				break;
			}
			case 'l':
				{
					switch(*format++) {
						default:
							err=1;
							break;
						case 'l':
						case 'd':
						{
							int64_t arg = va_arg(va, int64_t);
							l += _padn(file, arg, 10, pad, zlead,  leftalign);
							l += _putll(file,arg);
							l += _padn(file, arg, 10, pad, zlead, !leftalign);
							break;
						}
						case 'u':
						{
							uint64_t arg = va_arg(va, uint64_t);
							l += _padn(file, arg, 10, pad, zlead,  leftalign);
							l += _putlu(file,arg);
							l += _padn(file, arg, 10, pad, zlead, !leftalign);
							break;
						}
						case 'x':
						{
							uint64_t arg = va_arg(va, uint64_t);
							l += _padn(file, arg, 16, pad, zlead,  leftalign);
							l += _putlx(file,arg);
							l += _padn(file, arg, 16, pad, zlead, !leftalign);
							break;
						}
						case 'X':
						{
							uint64_t arg = va_arg(va, uint64_t);
							l += _padn(file, arg, 16, pad, zlead,  leftalign);
							l += _putlX(file,arg);
							l += _padn(file, arg, 16, pad, zlead, !leftalign);
							break;
						}
					}
					break;
				}
			}
		}
		else {
			l += _putc(file, *format++);
		}
	}

	return err ? -1 : l;
}

int32_t fprintf(file_itf file, const char * format, ...) {

	int32_t r;

	va_list va;
	va_start(va, format);

	r = vfprintf(file, format, va);

	va_end(va);

	return r;
}
