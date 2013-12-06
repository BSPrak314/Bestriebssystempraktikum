
#ifndef _printf_h_
#define _printf_h_

//__attribute__( ( format(printf,1,2) ) )
// there is no attribute for binar attributes - so i prefer no typechecking
void printf( char *str, ... );

__attribute__( ( format(printf,1,2) ) )
void print( char *str, ... );

#endif