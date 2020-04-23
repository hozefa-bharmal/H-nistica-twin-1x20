#define READ_CMD 0x02

#define SUCCESS 0

 /************************************************
*************************************************
*		   Module Information
*************************************************
************************************************/

int get_vendor_name_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *vendor_name );

int get_minimum_frequency_bound_of_nistica_wss_module( unsigned int uart_port_number, unsigned short* minimum_frequency_bound );

int get_maximum_frequency_bound_of_nistica_wss_module( unsigned int uart_port_number, unsigned short* maximum_frequency_bound );

int get_minimum_channel_bandwidth_of_nistica_wss_module( unsigned int uart_port_number, unsigned short* minimum_channel_bandwidth );

int get_maximum_wss_id_value_of_nistica_wss_module( unsigned int uart_port_number, short* maximum_wss_id_value );

int get_maximum_ocm_id_value_of_nistica_wss_module( unsigned int uart_port_number, short* maximum_ocm_id_value );

int get_maximum_waveplan_id_value_of_nistica_wss_module( unsigned int uart_port_number, short* maximum_ocm_id_value );
