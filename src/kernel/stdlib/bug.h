#pragma once

void _break(const char * file, const char * func, int line);
void _bug(const char * file, const char * func, int line);

#define _BREAK() _break(__FILE__,__FUNCTION__,__LINE__)
#define _BUG()   _bug(__FILE__,__FUNCTION__,__LINE__)
#define _BUG_ON(x) do{if((x)) _bug(__FILE__,__FUNCTION__,__LINE__);}while(0)
