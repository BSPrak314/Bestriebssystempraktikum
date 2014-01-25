#ifndef _utils_H_
#define _utils_H_  

void 	*memcpy(void *dest, const void *src, unsigned int n);
void 	waitBusy( int );
int 	strCompare( char* , char* );
int 	startsWith( char* , char* );
void 	clearMemory( unsigned int , unsigned int );

#endif 