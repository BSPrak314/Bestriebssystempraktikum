

#ifndef _aic_h_
#define _aic_h_

void 	aic_setInterruptVector_nr( unsigned int, unsigned int );
void 	aic_setSpuriousVector( unsigned int );
unsigned int 	aic_statusInterrupt_nr( unsigned int );
unsigned int 	aic_pendingInterrupt_nr( unsigned int );
void 	aic_enableInterrupt_nr( unsigned int );
void 	aic_disableInterrupt_nr( unsigned int );
void 	aic_clearInterrupt_nr( unsigned int );
int 	aic_isLineActive_nr( unsigned int );
void 	aic_setInterrupt_nr( unsigned int );
void 	aic_endOfInterrupt( void );

#endif