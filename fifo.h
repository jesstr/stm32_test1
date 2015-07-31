#ifndef __FIFO_H
#define __FIFO_H


/* FIFO buffer size, should be n^2 value */
#define FIFO_BUF_SIZE	16
#define FIFO_BUF_MASK	FIFO_BUF_SIZE - 1

/* Buffer state flags */
#define overflow_flag	0x01
#define empty_flag		0x02
/* Buffer state register */
extern unsigned char buf_flags;


/* Write n data to buffer */
int WriteBuf(unsigned char *data, unsigned int n);
/* Read n data from buffer */
int ReadBuf(unsigned char *out_buf, unsigned int n);
/* Flush buffer */
void FlushBuf(void);
/* Get unread data count in buffer */
int GetBufDataCount();
/* Look data in buffer, but not read out */
void LookBuf(unsigned char *out_buf);


#endif /* __FIFO_H */
