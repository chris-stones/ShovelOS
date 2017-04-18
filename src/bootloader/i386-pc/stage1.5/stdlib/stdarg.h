#pragma once

#if defined(__GNUC__)
	#define va_list         __builtin_va_list
	#define va_start(v,l)   __builtin_va_start(v,l)
	#define va_end(v)       __builtin_va_end(v)
	#define va_arg(v,l)     __builtin_va_arg(v,l)
#endif

#if defined(_MSC_VER)
	// taken from mingw-w64 - public domain.
	#define _ADDRESSOF(v) (&(v))
	typedef char *  va_list;
	#if defined(_M_IA64)
		#define _VA_ALIGN 8
		#define _SLOTSIZEOF(t) ((sizeof(t) + _VA_ALIGN - 1) & ~(_VA_ALIGN - 1))
		#define _VA_STRUCT_ALIGN 16
		#define _ALIGNOF(ap) ((((ap)+_VA_STRUCT_ALIGN - 1) & ~(_VA_STRUCT_ALIGN -1)) - (ap))
		#define _APALIGN(t,ap) (__alignof(t) > 8 ? _ALIGNOF((uintptr_t) ap) : 0)
	#else
		#define _SLOTSIZEOF(t) (sizeof(t))
		#define _APALIGN(t,ap) (__alignof(t))
	#endif
	#if defined(_M_IX86)
		#define _INTSIZEOF(n) ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))
		#define _crt_va_start(v,l)	((v) = (va_list)_ADDRESSOF(l) + _INTSIZEOF(l))
		#define _crt_va_arg(v,l)	(*(l *)(((v) += _INTSIZEOF(l)) - _INTSIZEOF(l)))
		#define _crt_va_end(v)		((v) = (va_list)0)
		#define _crt_va_copy(d,s)	((d) = (s))
	#elif defined(_M_AMD64)
		#define _PTRSIZEOF(n) ((sizeof(n) + sizeof(void*) - 1) & ~(sizeof(void*) - 1))
		#define _ISSTRUCT(t)  ((sizeof(t) > sizeof(void*)) || (sizeof(t) & (sizeof(t) - 1)) != 0)
		#define _crt_va_start(v,l)	((v) = (va_list)_ADDRESSOF(l) + _PTRSIZEOF(l))
		#define _crt_va_arg(v,t)	_ISSTRUCT(t) ?						\
			(**(t**)(((v) += sizeof(void*)) - sizeof(void*))) : \
			(*(t *)(((v) += sizeof(void*)) - sizeof(void*)))
		#define _crt_va_end(v)		((v) = (va_list)0)
		#define _crt_va_copy(d,s)	((d) = (s))
	#else
		#error unsupported platform/compiler combination.
	#endif
	// end of mingw-w64 public domain code.

	#define va_start _crt_va_start
	#define va_arg _crt_va_arg
	#define va_end _crt_va_end
#endif
