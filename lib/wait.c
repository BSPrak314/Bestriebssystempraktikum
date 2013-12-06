void waitBusy(int loops)
{
	for(; loops > 0;loops--)
		asm("" ::: "memory");
}