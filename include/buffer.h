#ifndef _buffer_H_
#define _buffer_H_

#define BUFFERSIZE 200
#define BUSYWAIT 0
#define OVERWRITE 1
#define IGNORE 2

struct ringbuffer
{
	int mode;
	int curr_read;
	int curr_write;
	int elements;
	int working;
	char buffer[BUFFERSIZE];
};

void 	RingBuffer_init(struct ringbuffer*);

void 	RingBuffer_printBuffer(struct ringbuffer*);

char	RingBuffer_get(struct ringbuffer*);

void	RingBuffer_put(char, struct ringbuffer*);

int 	RingBuffer_hasElements(struct ringbuffer*);

#endif