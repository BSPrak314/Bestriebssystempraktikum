void waitBusy(int loops)
{
	for(; loops > 0;loops--)
		asm("" ::: "memory");
}

int strCompare(char* str1, char* str2)
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

int startsWith(char* str1, char* str2)
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