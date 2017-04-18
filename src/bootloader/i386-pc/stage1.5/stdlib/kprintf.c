
#include <_config.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#define __WITH_64BIT_MATH 0

static int32_t _puts(const char * str) {

  int32_t l = 0;
  char c;
  while((c = *str++))
    l += _putc(c);
  return l;
}

static sint32_t _nstrlen32(sint32_t n, sint32_t base) { \
  sint32_t ret;
  if(n<0) { 
    ret  =  2; 
    n   *= -1; 
  } else 
    ret = 1; 
  while((n /= base)>0) 
    ret++; 
  return (ret); 
}

static int32_t _padn32(sint32_t n, uint32_t base, int32_t pad, int zlead, int leftalign) {

  int32_t l=0;
  if(!leftalign) {
    pad -= _nstrlen32(n,base);
    const char padchar = zlead ? '0' : ' ';
    while(pad-- >0)
      l += _putc(padchar);
  }
  return l;
}

#if(__WITH_64BIT_MATH)
static sint32_t _nstrlen64(sint64_t n, sint64_t base) { \
  sint32_t ret;
  if(n<0) { 
    ret  =  2; 
    n   *= -1; 
  } else 
    ret = 1; 
  while((n /= base)>0) 
    ret++; 
  return (ret); 
}
static int32_t _padn64(sint64_t n, uint32_t base, int32_t pad, int zlead, int leftalign) {

  int32_t l=0;
  if(!leftalign) {
    pad -= _nstrlen64(n,base);
    const char padchar = zlead ? '0' : ' ';
    while(pad-- >0)
      l += _putc(padchar);
  }
  return l;
}
#endif

static int32_t  _padc(int32_t pad, int leftalign) {

  int32_t l=0;
  if(!leftalign) {
    pad -= 1;
    while(pad-- >0)
      l += _putc(' ');
  }
  return l;
}

static int32_t  _pads(const char * str, int32_t pad, int leftalign) {

  int32_t l=0;
  if(!leftalign) {
    pad -= strlen(str);
    while(pad-- >0)
      l += _putc(' ');
  }
  return l;
}

static int32_t __putxX(uint32_t n, uint8_t a) {

  int l=0;
  int16_t s;   // shift
  uint8_t  x;
  for(s=28; s>=0; s-=4)
    if((x = (n>>s)&15) || l || !s)
      l += _putc(x + ((x<10) ? '0' : (a-10)));
  
  return l;
}

static int32_t _putx(uint32_t n) {

  return __putxX(n,'a');
}

static int32_t _putX(uint32_t n) {

  return __putxX(n,'A');
}

#if(__WITH_64BIT_MATH)
static int32_t __putlxX(uint64_t n, uint8_t a) {

  int      l=0;
  int16_t s;   // shift
  uint8_t  x;
  for(s=60; s>=0; s-=4)
    if((x = (n>>s)&15) || l || !s)
      l += _putc(x + ((x<10) ? '0' : (a-10)));
  
  return l;
}

static int32_t _putlx(uint64_t n) {

  return __putlxX(n, 'a');
}

static int32_t _putlX(uint64_t n) {

  return __putlxX(n, 'A');
}
#endif

static int32_t _putu(uint32_t n) {

  uint32_t   d = 1000000000; // divisor
  int16_t   h;              // digit
  int16_t   l = 0;          // wrote length
  for(; d>=1; d/=10)
    if((h = ((n/d)%10)) || l || (d<=1)) {
      ++l;
      _putc('0' + h);
    }
  return l;
}

static int32_t _putd(int32_t n) {

  if(n>=0)
    return _putu((int32_t)n);
  
  return _putc('-') + _putd((int32_t)(-n));
}

#if(__WITH_64BIT_MATH)
static int32_t _putlu(uint64_t n) {

  uint64_t d  = 1000000000;
  d *= 1000000000;
  
  int16_t   h;              // digit
  int16_t   l = 0;          // wrote length
  for(; d>=1; d/=10)
    if((h = ((n/d)%10)) || l || (d<=1)) {
      ++l;
      _putc('0' + h);
    }
  return l;
}

static int32_t _putll(int64_t n) {

  if(n>=0)
    return _putlu((uint64_t)n);

  return _putc('-') + _putlu((uint64_t)(-n));
}
#endif

static int32_t vkprintf(const char * format, va_list va) {

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
	l += _putc('%');
	break;
      case 'c':
	{
	  l += _padc(pad,   leftalign);
	  l += _putc((uint8_t)va_arg(va, int32_t));
	  l += _padc(pad,  !leftalign);
	  break;
	}
      case 'u':
	{
	  uint32_t arg = va_arg(va,uint32_t);
	  l += _padn32(arg, 10, pad, zlead,  leftalign);
	  l += _putu(arg);
	  l += _padn32(arg, 10, pad, zlead, !leftalign);
	  break;
	}
      case 'i':
      case 'd':
	{
	  int32_t arg = va_arg(va,int32_t);
	  l += _padn32(arg, 10, pad, zlead,  leftalign);
	  l += _putd(arg);
	  l += _padn32(arg, 10, pad, zlead, !leftalign);
	  break;
	}
      case 'p':
	{
	  if(sizeof(void*) == 8) {
#if(__WITH_64BIT_MATH)
	    uint64_t arg = va_arg(va, uint64_t);
	    l += _padn64(arg, 16, pad, zlead,  leftalign);
	    l += _putlx(arg);
	    l += _padn64(arg, 16, pad, zlead, !leftalign);
#endif
	    break;
	  }
	}
	// no break - 32bit pointers can be handled by %x.
      case 'x':
	{
	  uint32_t arg = va_arg(va,uint32_t);
	  l += _padn32(arg, 16, pad, zlead,  leftalign);
	  l += _putx(arg);
	  l += _padn32(arg, 16, pad, zlead, !leftalign);
	  break;
	}
      case 'X':
	{
	  uint32_t arg = va_arg(va,uint32_t);
	  l += _padn32(arg, 16, pad, zlead,  leftalign);
	  l += _putX(arg);
	  l += _padn32(arg, 16, pad, zlead, !leftalign);
	  break;
	}
      case 's':
	{
	  const char * str = va_arg(va,const char *);
	  l += _pads(str, pad,  leftalign);
	  l += _puts(str);
	  l += _pads(str, pad, !leftalign);
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
#if(__WITH_64BIT_MATH)
	      int64_t arg = va_arg(va, int64_t);
	      l += _padn64(arg, 10, pad, zlead,  leftalign);
	      l += _putll(arg);
	      l += _padn64(arg, 10, pad, zlead, !leftalign);
#endif
	      break;
	    }
	  case 'u':
	    {
#if(__WITH_64BIT_MATH)
	      uint64_t arg = va_arg(va, uint64_t);
	      l += _padn64(arg, 10, pad, zlead,  leftalign);
	      l += _putlu(arg);
	      l += _padn64(arg, 10, pad, zlead, !leftalign);
#endif
	      break;
	    }
	  case 'x':
	    {
#if(__WITH_64BIT_MATH)
	      uint64_t arg = va_arg(va, uint64_t);
	      l += _padn(arg, 16, pad, zlead,  leftalign);
	      l += _putlx(arg);
	      l += _padn(arg, 16, pad, zlead, !leftalign);
#endif
	      break;
	    }
	  case 'X':
	    {
#if(__WITH_64BIT_MATH)
	      uint64_t arg = va_arg(va, uint64_t);
	      l += _padn64(arg, 16, pad, zlead,  leftalign);
	      l += _putlX(arg);
	      l += _padn64(arg, 16, pad, zlead, !leftalign);
#endif
	      break;
	    }
	  }
	  break;
	}
      }
    }
    else {
      l += _putc(*format++);
    }
  }
  
  return err ? -1 : l;
}

int32_t kprintf(const char * format, ...) {

  int32_t r;
  
  va_list va;
  va_start(va, format);
  
  r = vkprintf(format, va);
  
  va_end(va);
  
  return r;
}
