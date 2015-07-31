#include "fifo.h"


unsigned char buf_flags;

static unsigned char buffer[FIFO_BUF_SIZE];
static unsigned int in_index, out_index;


/* Write n data to buffer */
int WriteBuf(unsigned char *data, unsigned int n)
{
	unsigned int i = 0;
	unsigned int data_count = 0;

	while (n--) {

		/* If buffer overflowed, drop new data */
		if (buf_flags & overflow_flag) {
			//return OVFERR;
			return 0;
		}
		else {
			buffer[in_index++] = data[i++];
			in_index &= FIFO_BUF_MASK;
			/* Clear overflow flag */
			buf_flags &= ~empty_flag;
			data_count++;
			/* Set overflow flag */
			if (in_index == out_index) {
				buf_flags |= overflow_flag;
			}
		}
	}
	return data_count;
}


/* Flush buffer */
void FlushBuf(void)
{
	out_index = in_index;
	/* Set empty flag */
	buf_flags |= empty_flag;
}


/* Read n data from buffer */
int ReadBuf(unsigned char *out_buf, unsigned int n)
{
	unsigned int i = 0;
	unsigned int data_count = 0;

	while (n--) {
		/* Buffer is overflowed or empty */
		if (out_index == in_index) {
			if (!(buf_flags & overflow_flag)) {
				/* Buffer is empty */
				buf_flags |= empty_flag;
				//return EMPTERR;
				return 0;
			}
		}
		out_buf[i++] = buffer[out_index++];
		out_index &= FIFO_BUF_MASK;
		data_count++;
		/* Clear overflow flag */
		buf_flags &= ~overflow_flag;
	}
	return data_count;
}


/* Get unread data count in buffer */
int GetBufDataCount()
{
	/* Check if buffer is overflowed or empty */
	if (out_index == in_index) {
		if (!(buf_flags & overflow_flag)) {
			/* Buffer is empty */
			buf_flags |= empty_flag;
			return 0;
		}
		else {
			return FIFO_BUF_SIZE;
		}
	}
	if (in_index > out_index) {
		return in_index - out_index;
	}
	else {
		return FIFO_BUF_SIZE - out_index + in_index;
	}
}


/* Look data in buffer, but not read out */
void LookBuf(unsigned char *out_buf)
{
	unsigned int i = 0;

	while (i < FIFO_BUF_SIZE) {
		out_buf[i] = buffer[i];
		i++;
	}
}
