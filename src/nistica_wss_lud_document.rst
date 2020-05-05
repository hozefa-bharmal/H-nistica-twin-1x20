Assign Waveplan ID for particular waveplan configuration
++++++++++++++++++++++++++++++++++++++++++++++++++++++++

This function is used to set the ``Waveplan ID`` for a particular waveplan configuration.
When this function is called, it triggers a switchover in WSS module. If
``BootSwRestore`` is enabled, this active configuration is part of the state saved
to non-volatile memory and restored at cold boot. If not, the module cold
boots into configuration 0, and all configurations will be blocked.

Function takes **four arguments** - UART port number, WSS ID, Altconf ID and
Waveplan ID. WSS ID refers to WSS in the module and Altconf is the configuration
ID to which the Waveplan ID needs to be assigned. 

.. code-block::c

 int assign_particular_waveplan_of_nistica_wss_module( unsigned int uart_port_number, unsigned short wss_id, unsigned short altconf_id, unsigned short waveplan_id );

Function forms a command structure consisting of Start bytes, Message ID, Length,
Command, Object ID, WSS ID, Altconf ID, Parameter, Waveplan ID, Checksum and Stop
bytes.

The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning
and '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
synchronization between control module and WSS. Below snippet is the command
structure format with essential fields inorder to assign Waveplan ID.

.. code-block::c

	unsigned char packet_to_transmit[] = {0xdd, 0x01, MID, LEN, CMD, OBJ, TABLE_BYTE, INSTANCE, PARAMETER, DATA[0], DATA[1] };

The use of a separate Altconf byte requires a flag in the Command word which is
only supported in the Long Command packet format. The interpretation of the ``TABLE
byte`` has been extended to include the altconfig id in the top 4 bits as well as
the WSS id in the lower 4 bits, avoiding the use of the long packet format to
specify Altconf. The logic below forms a single byte out of WSS ID and Altconf ID.

.. code-block::c

	unsigned char table_byte = (char)(( altconf_id << 4 )| wss_id);

Waveplan ID received as argument is stored in a char pointer. By this the value
will be stored in memory so while reading back, two bytes data can be read individually
so as to fill the two byte data field in command structure.
	
.. code-block::c

	unsigned char *waveplan_id_pointer = (unsigned char*) &waveplan_id;

	unsigned char packet_to_transmit[] = {0xdd, 0x01, 0x01, 0x08, WRITE_CMD | 0x20, 0xA8, table_byte, 0x06, 0x01, waveplan_id_pointer[0], waveplan_id_pointer[1] };

	checksum = calculate_checksum( packet_to_transmit[2], strlen(packet_to_transmit));
	packet_to_transmit[11] = checksum;
	packet_to_transmit[12] = 0xdd;
	packet_to_transmit[13] = 0x02;

To indicate the presence of Table field, CMD field is added with ``32``(0x20) by
which the module would intepret it. Checksum is calculated using the function
``calculate_checksum`` which internally XORs all bytes from MID to last DATA byte.
Finally, stop bytes are initialized in packet.

UART port is used to transmit and receive data packets from/to
Nistica WSS module. Response for the transmit message is received from module
through UART. Certain validation in the receive packet such as Message ID
and Result is done and status is returned.

.. code-block::c
 
 transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
 
 receive_packet_via_uart_port(uart_port_number, uart_received_packet_return,
 
 Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
 Validate RES -> SUCCESS == uart_received_packet_return[4];

