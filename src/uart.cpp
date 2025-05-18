#include "uart.h"

#include <avr/io.h>
#include <util/delay.h>

void USART::USART_Init(unsigned int ubrr)
{
    UBRRH = (unsigned char)(ubrr >> 8);
    UBRRL = (unsigned char)ubrr;
    UCSRB = (1 << RXEN) | (1 << TXEN);
    UCSRC = (1 << URSEL) | (1 << USBS) | (3 << UCSZ0);
}

void USART::USART_Transmit(unsigned char data)
{
    while (!(UCSRA & (1 << UDRE)))
        ;
    UDR = data;
}

void USART::USART_SendString(const char *str)
{
    while (*str)
    {
        USART_Transmit(*str++);
    }
}

unsigned char USART::USART_Receive(void)
{
    /* Wait for data to be received */
    while (!(UCSRA & (1 << RXC)))
        ;
    /* Get and return received data from buffer */
    return UDR;
}

void USART::USART_Flush(void)
{
    unsigned char dummy;
    while (UCSRA & (1 << RXC))
        dummy = UDR;
}

void USART::USART_ReceiveString(char *str, unsigned int length)
{
    for (unsigned int i = 0; i < length; i++)
    {
        str[i] = USART_Receive();
    }
    str[length] = '\0'; // Null-terminate the string
}
