//implements ring buffer for chars
#include <buffer.h>
#include <printf.h>
#include <led.h>

static int RingBuffer_next( unsigned int pos)
{
        if( pos >= BUFFERSIZE-1 )
                return 0;
        return pos + 1;
}

void RingBuffer_init(struct ringbuffer *rb) //Gets next element of the ring buffer
{
        rb->elements = 0;
        rb->curr_read = 0;
        rb->curr_write = 0;
        rb->working = 0;
        rb->mode = BUSYWAIT;
}

void RingBuffer_printBuffer(struct ringbuffer *rb) //Gets next element of the ring buffer
{
        int i = 0;
        int tmp = rb->curr_read;
        for(;i<rb->elements;i++){
                printf("%c",rb->buffer[rb->curr_read]);
                rb->curr_read = RingBuffer_next(rb->curr_read);
        }
        rb->curr_read = tmp;
}

char RingBuffer_get(struct ringbuffer *rb) //Gets next element of the ring buffer
{
        rb->working = 1;
        char c = 0;
        if( rb->elements ){
                c = rb->buffer[rb->curr_read];
                rb->buffer[rb->curr_read] = 0;
                rb->curr_read = RingBuffer_next(rb->curr_read);
                rb->elements--;
        }
        if( !rb->elements ){
                rb->elements = 0;
                rb->curr_read = 0;
                rb->curr_write = 0;
        }
        rb->working = 0;
        return c;
}

int RingBuffer_hasElements(struct ringbuffer *rb)
{
        return (rb->elements != 0);
}

//Puts new element in the ring buffer, dealing with full buffer depends on buffer mode
void RingBuffer_put(char c,struct ringbuffer *rb) 
{
        rb->working = 1;
        if( rb->elements < BUFFERSIZE ){
                rb->buffer[rb->curr_write] = c;
                rb->elements++;
        }
        else{
                if( rb->mode == OVERWRITE){
                        rb->buffer[rb->curr_write] = c;
                        rb->curr_read = RingBuffer_next(rb->curr_read);
                        rb->curr_write = RingBuffer_next(rb->curr_write);
                        rb->working = 0;
                        return;
                }
                if( rb->mode == IGNORE){
                        rb->working = 0;
                        return;
                }
                
                while(rb->elements >= BUFFERSIZE ){
                        yellow_on();
                }
                yellow_off();
                rb->buffer[rb->curr_write] = c;
                rb->elements++;
        }
        rb->curr_write = RingBuffer_next(rb->curr_write);
        rb->working = 0;
}