#include "uart_lib.h"

//#define DBG printf
#define DBG (void)
// if you get lots of rx_overruns, increase this (or read the data quicker!)
#define UART_RX_QUEUE_SIZE 32
xQueueHandle  uart_rx_queue;
volatile uint16_t uart_rx_overruns;
volatile uint16_t uart_rx_bytes;




/*
 * UART rx Interrupt routine
 */
static void 
uart_isr(void)
{
	uint8_t temp;
	signed portBASE_TYPE ret;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	if (UART_RXFIFO_FULL_INT_ST != (READ_PERI_REG(UART_INT_ST(UART0)) & UART_RXFIFO_FULL_INT_ST))
	{
		return;
	}
	WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR);

    while (READ_PERI_REG(UART_STATUS(UART0)) & (UART_RXFIFO_CNT << UART_RXFIFO_CNT_S)) {
		temp = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF;
		ret = xQueueSendToBackFromISR
                    (
                        uart_rx_queue,
						&temp,
                        &xHigherPriorityTaskWoken
                    );
		if (ret != pdTRUE)
		{
			uart_rx_overruns++;
		} 
		else
		{
			uart_rx_bytes++;
		}
	}
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}
/******************************************************************************
 * FunctionName : uart1_tx_one_char
 * Description  : Internal used function
 *                Use uart1 interface to transfer one char
 * Parameters   : uint8 TxChar - character to tx
 * Returns      : OK
*******************************************************************************/
LOCAL STATUS
uart_tx_one_char(uint8 uart, uint8 TxChar)
{
    while (true)
    {
      uint32 fifo_cnt = READ_PERI_REG(UART_STATUS(uart)) & (UART_TXFIFO_CNT<<UART_TXFIFO_CNT_S);
      if ((fifo_cnt >> UART_TXFIFO_CNT_S & UART_TXFIFO_CNT) < 126) {
        break;
      }
    }

    WRITE_PERI_REG(UART_FIFO(uart) , TxChar);
    return OK;
}

/******************************************************************************
 * FunctionName : uart1_write_char
 * Description  : Internal used function
 *                Do some special deal while tx char is '\r' or '\n'
 * Parameters   : char c - character to tx
 * Returns      : NONE
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR
uart1_write_char(char c)
{
  if (c == '\n')
  {
    uart_tx_one_char(UART1, '\r');
    uart_tx_one_char(UART1, '\n');
  }
  else if (c == '\r')
  {
  }
  else
  {
    uart_tx_one_char(UART1, c);
  }
}
/******************************************************************************
 * FunctionName : uart0_tx_buffer
 * Description  : use uart0 to transfer buffer
 * Parameters   : uint8 *buf - point to send buffer
 *                uint16 len - buffer len
 * Returns      :
*******************************************************************************/
void ICACHE_FLASH_ATTR
uart0_tx_buffer(uint8 *buf, uint16 len)
{
  uint16 i;

  for (i = 0; i < len; i++)
  {
    uart_tx_one_char(UART0, buf[i]);
  }
}

/******************************************************************************
 * FunctionName : uart0_sendStr
 * Description  : use uart0 to transfer buffer
 * Parameters   : uint8 *buf - point to send buffer
 *                uint16 len - buffer len
 * Returns      :
*******************************************************************************/
void uart0_sendStr(const char *str)
{
	while(*str)
	{
		uart_tx_one_char(UART0, *str++);
	}
}

/******************************************************************************
 * FunctionName : uart0_rx_intr_handler
 * Description  : Internal used function
 *                UART0 interrupt handler, add self handle code inside
 * Parameters   : void *para - point to ETS_UART_INTR_ATTACH's arg
 * Returns      : NONE
*******************************************************************************/
//extern void at_recvTask(void);

LOCAL void
uart0_rx_intr_handler(void *para)
{
  /* uart0 and uart1 intr combine togther, when interrupt occur, see reg 0x3ff20020, bit2, bit0 represents
    * uart1 and uart0 respectively
    */
//  RcvMsgBuff *pRxBuff = (RcvMsgBuff *)para;
//  uint8 RcvChar;
  uint8 uart_no = UART0;//UartDev.buff_uart_no;

//  if (UART_RXFIFO_FULL_INT_ST != (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_FULL_INT_ST))
//  {
//    return;
//  }
  if (UART_RXFIFO_FULL_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_FULL_INT_ST))
  {
    //at_recvTask();
    WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_RXFIFO_FULL_INT_CLR);
  }

//  WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_RXFIFO_FULL_INT_CLR);

//  if (READ_PERI_REG(UART_STATUS(uart_no)) & (UART_RXFIFO_CNT << UART_RXFIFO_CNT_S))
//  {
//    RcvChar = READ_PERI_REG(UART_FIFO(uart_no)) & 0xFF;
//    at_recvTask();
//    *(pRxBuff->pWritePos) = RcvChar;

//    system_os_post(at_recvTaskPrio, NULL, RcvChar);

//    //insert here for get one command line from uart
//    if (RcvChar == '\r')
//    {
//      pRxBuff->BuffState = WRITE_OVER;
//    }
//
//    pRxBuff->pWritePos++;
//
//    if (pRxBuff->pWritePos == (pRxBuff->pRcvMsgBuff + RX_BUFF_SIZE))
//    {
//      // overflow ...we may need more error handle here.
//      pRxBuff->pWritePos = pRxBuff->pRcvMsgBuff ;
//    }
//  }
}


/*
 * Get a char from the RX buffer. Wait up to timeout ms for data
 * -ve timeout value means wait forever
 * return the char, or -1 on error
 */
int ICACHE_FLASH_ATTR 
uart_getchar_ms(int timeout)
{
	portBASE_TYPE ticks;
	unsigned char ch;
	if (timeout < 0)
	{
		ticks = portMAX_DELAY;
	}
	else
	{
		ticks = timeout / portTICK_RATE_MS; 
	}
	DBG("ticks=%d\r\n", ticks);
	if ( xQueueReceive(uart_rx_queue, &ch, ticks) != pdTRUE)
	{
		DBG("no data\r\n");
		return -1;
	}
	DBG("returning %d\r\n", ch);
	return (int)ch;
}


/*
 * Read at most len-1 chars from the UART
 * last char will in buf will be a null
 */
char * ICACHE_FLASH_ATTR
uart_gets(char *buf, int len)
{
	int i=0; 
	char *p=buf;
	int ch;
	while (i<len)
	{
		ch = uart_getchar();
		if (ch == '\r' || ch == '\n' || ch == -1 || ch == 0x03)
		{
			*p='\0';
			return buf;	
		}
		*p=(char)ch;
		os_putc(*p);
		p++;
		i++;
	}
	*p='\0';
	return buf;
}

/* 
 * Return the number of characters available to read
 */
int ICACHE_FLASH_ATTR
uart_rx_available(void)
{
	return uxQueueMessagesWaiting(uart_rx_queue);
}

void ICACHE_FLASH_ATTR 
uart_set_baud(int uart, int baud)
{
	uart_div_modify(uart, UART_CLK_FREQ / (baud));
}

/*
 * Initialise the uart receiver
 */
void ICACHE_FLASH_ATTR
uart_rx_init(void)
{ 
	uart_rx_queue = xQueueCreate( 
		UART_RX_QUEUE_SIZE,
		sizeof(char)
	);
	uart_rx_overruns=0;
	uart_rx_bytes=0;
	/* _xt_isr_mask seems to cause Exception 20 ? */
	//_xt_isr_mask(1<<ETS_UART_INUM);
	_xt_isr_attach(ETS_UART_INUM, uart_isr);
	_xt_isr_unmask(1<<ETS_UART_INUM);
}



// vim: ts=4 sw=4 noexpandtab
