#if !defined(UART_H)
#define UART_H

#define FOSC 16000000 // Clock Speed is 16 MHz
#define BAUD 9600
#define MYUBRR FOSC / 16 / BAUD - 1

class USART
{
private:
    /* data */
public:
    void USART_Init(unsigned int ubrr);
    void USART_Transmit(unsigned char data);
    void USART_SendString(const char *str);
    unsigned char USART_Receive(void);
    void USART_Flush(void);
    void USART_ReceiveString(char *str, unsigned int length);
};

#endif // UART_H)
