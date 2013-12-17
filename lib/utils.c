
#include <interrupt_handler.h>
#include <printf.h>

/*
 * memcpy() - primitives, nicht optimiertes memcpy()
 *
 * @dest: Ziel
 * @src: Quelle
 * @n: Menge zu kopierender Bytes
 *
 * Kompatibel zu dem memcpy() aus der "normalen" C-Library.
 */
void *memcpy(void *dest, const void *src, unsigned int n)
{
  	const char *s = src; 
 	char *d = dest; 
 
 	while (n--)
 		*d++ = *s++;
 
 	return dest;
}

void clearMemory( unsigned int start, unsigned int end)
{
	unsigned int * address = (unsigned int *)start;
	while( start < end ){
		//print("clearing memory at address %x\n",address);
		*address = 0;
		start+=4;
		address = (unsigned int *)start;
	}
	return;
}

void printStack( unsigned int * stack, unsigned int blocks )
{
	unsigned int i = 0;
	for(;i<blocks;i++){
		print("sp -4* %x has value : %x\n", i, (unsigned int)*stack );
		stack++;
	}
}

void waitBusy( int loops )
{
	for(; loops > 0;loops--)
		asm("" ::: "memory");
}

int strCompare( char* str1, char* str2 )
{
	char c1 = (char)*str1;
	char c2 = (char)*str2;
	
	if(c1 != c2)
		return 0;
	
	while(c1 != 0 && c2 != 0){
	  
		if( c1 != c2 )
			return 0;
		str1++;
		str2++;
		c1 = (char)*str1;
		c2 = (char)*str2;
		
	}
	return 1;
}

int startsWith( char* str1, char* str2 )
{
	char c1 = (char)*str1;
	char c2 = (char)*str2;
	
	do{
		if(c1 != c2 || c1 == 0)
			return 0;
		str1++;
		str2++;
		c1 = (char)*str1;
		c2 = (char)*str2;
		
	}while(c2 != 0);
	
	return 1;
}