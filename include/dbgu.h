
#ifndef _dbgu_h_
#define _dbgu_h_

void	dbgu_bufferedOutput( char );
void    dbgu_inputBuffering( void );
void    dbgu_outputBufferPrint( void );
void 	dbgu_printInputBuffer( void );
char 	dbgu_nextInputChar( void );
void 	dbgu_cleanInputBuffer( void );
void 	dbgu_cleanOutputBuffer( void );
int 	dbgu_hasBufferedInput( void );
int 	dbgu_hasBufferedOutput( void );
void 	dbgu_enableWriting( void );
void 	dbgu_enableReading( void );
void 	dbgu_start( void );
void 	dbgu_dealWithInterrupts( void );
int 	dbgu_triggeredTXRDY( void );
int 	dbgu_triggeredRXRDY( void );
int 	dbgu_isRXRDY( void );
int 	dbgu_isTXRDY( void );
void 	dbgu_enableRXRDYInterrupt( void );
void 	dbgu_enableTXRDYInterrupt( void );
void 	dbgu_disableRXRDYInterrupt(void);
void 	dbgu_disableTXRDYInterrupt(void);
char 	dbgu_readChar( void );
void 	dbgu_writeChar( char );

#endif
