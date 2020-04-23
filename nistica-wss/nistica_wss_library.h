#define READ_CMD 0x02

#define SUCCESS 0

 /************************************************
*************************************************
*		   Module Information
*************************************************
************************************************/

int get_vendor_name_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *vendor_name );

int get_minimum_frequency_bound_of_nistica_wss_module( unsigned int uart_port_number, unsigned short* minimum_frequency_bound );
