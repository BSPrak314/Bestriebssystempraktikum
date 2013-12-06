
#include <dbgu.h>
#include <buffer.h>

/* address DEBUG UNIT*/
#define DBGU 0xfffff200
/* Control Bits */
#define RSTRX (1 << 2)          /* 1 = Receiver reset and disabled      */
#define RSTTX (1 << 3)          /* 1 = Transmitter reset and disabled   */
#define RXEN (1 << 4)           /* 1 = Receiver enabled IF RXDIS = 0    */
#define RXDIS (1 << 5)          /* 1 = Receiver disabled                */
#define TXEN (1 << 6)           /* 1 = Transmitter enabled IF TXDIS = 0 */
#define TXDIS (1 << 7)          /* 1 = Transmitter disabled             */
#define RSTSTA (1 << 8)         /* Resets the status bits PARE, FRAME and OVRE in DBGU_SR */
/* Status Bits */
#define RXRDY (1)               /* Receiver has char recived ?          */
#define TXRDY (1 << 1)          /* Transmitter ready ?                  */
#define ENDRX (1 << 3)          /* end of transfer from Receiver        */
#define ENDTX (1 << 4)          /* end of transfer from Transmitter     */
#define OVRE (1 << 5)           /* overrun error, since last RSTSTA     */
#define FRAME (1 << 6)          /* framing error, since last RSTSTA     */
#define PARE (1 << 7)           /* parity error, since last RSTSTA      */
#define TXEMPTY (1 << 9)        /* 1: no char in DBGU_THR && no char beeing processed
                                 * 0:    char in DBGU_THR || TX disabled */
#define TXBUFE (1 << 11)        /* 1: TX Buffer is empty                */
#define RXBUFF (1 << 12)        /* 1: RX Buffer is full                 */
#define COMMTX (1 << 30)        /* commtx channel from prcessor 
                                 * is 0: off / 1: on                    */
#define COMMRX (1 << 31)        /* commrx channel from processor 
                                 * is 0: off / 1: on                    */
/*
 * status bits also where used by
 * DBGU_IER to enable corresponding interrupts
 * DBGU_IDR to disable corresponding interrupts 
 * DBGU_IMR to get the status of the corresponding interrupt
 */
struct dbgu_interface {
        unsigned int DBGU_CR;           /* Write-only */
        unsigned int DBGU_MR;           /* Read & Write */
        unsigned int DBGU_IER;          /* Write-only */
        unsigned int DBGU_IDR;          /* Write-only */
        unsigned int DBGU_IMR;          /* Read-only */
        unsigned int DBGU_SR;           /* Read-only */
        unsigned int DBGU_RHR;          /* Read-only */
        unsigned int DBGU_THR;          /* Write-only */
        unsigned int DBGU_BRGR;         /* Read & Write */
};
/* actual struct to manage the address sensitive  registers of the dbgu*/
static volatile
struct dbgu_interface * const dbgu = (struct dbgu_interface *)DBGU;

/* two ringbuffer for buffered IO */
struct ringbuffer buf_IO_out;
struct ringbuffer buf_IO_in;

/* get char from Input Reg of dbgu and put it into input buffer
 * is only used after RXRDY Interrupt */
void dbgu_inputBuffering( void )
{
        while(buf_IO_in.working )
                ;
        RingBuffer_put((char)dbgu->DBGU_RHR, &buf_IO_in);
}

/* get char from Output Buffer and put it into output reg of dbgu
 * is only used after TXRDY Interrupt */
void dbgu_outputBufferPrint( void )
{
        dbgu->DBGU_THR = (unsigned int)RingBuffer_get(&buf_IO_out);
}

/* get char from printf and put it into output buffer */
void dbgu_bufferedOutput( char c )
{
        while(buf_IO_out.working )
                ;
        RingBuffer_put(c, &buf_IO_out);
        /* needs TXRDY interrupt to print from OutputBuffer */
        dbgu_enableTXRDYInterrupt();
}

/* get char from InputBuffer */
char dbgu_nextInputChar( void )
{       
        //if( !buf_IO_in.working )
                return RingBuffer_get(&buf_IO_in);
        //else
        //        return 0;
}

void dbgu_printInputBuffer( void )
{
        RingBuffer_printBuffer(&buf_IO_in);
}

/* clean buffer by reseting all relevant pointers */
void dbgu_cleanInputBuffer( void )
{
        RingBuffer_init(&buf_IO_in);
}

/* clean buffer by reseting all relevant pointers */
void dbgu_cleanOutputBuffer( void )
{
        RingBuffer_init(&buf_IO_out);
}

int dbgu_hasBufferedInput( void )
{
        return RingBuffer_hasElements(&buf_IO_in);
}

int dbgu_hasBufferedOutput( void )
{
        return RingBuffer_hasElements(&buf_IO_out);
}

/* set control flags to enable dgbu to transmit chars via com1  */
void dbgu_enableWriting( void )
{
        dbgu->DBGU_CR = TXEN;
}

/* set control flags to enable dbgu to receive chars via com1  */
void dbgu_enableReading( void )
{
        dbgu->DBGU_CR = RXEN;
}

/* init IO Buffers and makes sure reading/writing is enabled*/
void dbgu_start( void )
{
        dbgu_enableWriting();
        dbgu_enableReading();
        dbgu_enableRXRDYInterrupt();
        dbgu_cleanInputBuffer();    
        dbgu_cleanOutputBuffer();
        buf_IO_in.mode = IGNORE;
}

/* the irq exception_handler uses this function if RXRDY or TXRDY Interrupt was trigged */
void dbgu_dealWithInterrupts( void )
{
        if( dbgu_triggeredRXRDY() ){
                dbgu_inputBuffering();
        }
        if( dbgu_triggeredTXRDY() ){
                if( dbgu_hasBufferedOutput() ){
                        dbgu_outputBufferPrint();
                }else{
                        dbgu_disableTXRDYInterrupt();
                        dbgu_cleanOutputBuffer();
                }
        }
}

/* return 1 (true) if Interrupt is enabled and corresponding status bit is set - 0 (false) otherwise
 * here for RXRDY Interrupt */
int dbgu_triggeredRXRDY( void )
{
        return (dbgu->DBGU_SR & RXRDY) && (dbgu->DBGU_IMR & RXRDY);
}

/* return 1 (true) if Interrupt is enabled and corresponding status bit is set - 0 (false) otherwise
 * here for TXRDY Interrupt */
int dbgu_triggeredTXRDY( void )
{
        return (dbgu->DBGU_SR & TXRDY) && (dbgu->DBGU_IMR & TXRDY);
}

/* return 1 (true) if corresponding status bit is set - 0 (false) otherwise */
int dbgu_isRXRDY( void )
{
        return dbgu->DBGU_SR & RXRDY;
}

/* return 1 (true) if corresponding status bit is set - 0 (false) otherwise */
int dbgu_isTXRDY( void )
{
        return dbgu->DBGU_SR & TXRDY;
}

void dbgu_enableRXRDYInterrupt( void )
{
        dbgu->DBGU_IER = RXRDY;
}

void dbgu_enableTXRDYInterrupt( void )
{
        dbgu->DBGU_IER = TXRDY;
}

void dbgu_disableRXRDYInterrupt( void )
{
        dbgu->DBGU_IDR = RXRDY;
}

void dbgu_disableTXRDYInterrupt( void )
{
        dbgu->DBGU_IDR = TXRDY;
}

/* receive char via polling */
char dbgu_readChar( void )
{
        while( !(dbgu->DBGU_SR & RXRDY)  )
                ;
        return (char)dbgu->DBGU_RHR;
}
/* transmit char via polling */
void dbgu_writeChar( char c )
{
        while( !(dbgu->DBGU_SR & TXRDY) )
                ;
        dbgu->DBGU_THR = (unsigned int)c;
}