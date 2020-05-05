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

Configure Channels in Add/Drop Ports and apply Attenuation
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

This function is used to configure multiple channels to add/drop ports of WSS
and applies attenuation for those channels. Channel configuration is the
significant operation of WSS module. Channels of different wavelengths are
configured to add/drop ports of WSS module. Attenuation is applied to each channel
ranging from ``0 to 24.4 dB``. ``25.5`` is considered as block state or OFF state.

Function takes **six arguments**. It forms a Long command structure consisting of
Start bytes, Message ID, Command, Object ID, WSS ID, Altconf ID, Channel numbers,
Port numbers, Atenuation values, Checksum and Stop bytes. Usage of long command
format helps in multiple channel configuration with a transmit size of ``1536 bytes``
as mentioned by Nistica.

.. code-block::c

 int set_channel_port_and_attenuation_of_nistica_wss_module( unsigned int uart_port_number, unsigned short wss_id, unsigned short altconf_id, unsigned char *channel_numbers, unsigned char *port_data, unsigned char *attenuation_data );

The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning
and '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
synchronization between control module and WSS. Value for each field is given below.

MID = 0x04; Long Command format matching bytes = 0x00, 0xFF;
LEN = 0x00, 0x00; (will be updated later once complete packet is formed)
CMD = 0x01, 0x21; Presence of Table and Altconf byte (1 + 32 + 256) 

First few bytes of 'packet_to_transmit' is initialized with Start bytes, MID,
Length(initially made as 0x00) and Command. Here Length and Command fields are two
bytes each.

.. code-block::c

	unsigned char packet_to_transmit[1600] = {0xdd, 0x01, 0x04, 0x00, 0xFF, 0x00, 0x00, 0x01, 0x21};	

Number of channels is found from 'channel_numbers' argument received and stored
into ``number_of_channels``. Attenuation data is converted from float to integer
data as per command structure. Based on the number of channels, the structure is
formed containing Object ID, WSS ID, Altconf ID, Channel numbers, Port numbers
and Attenuation values.

.. code-block::c

	for(int i=0; i<number_of_channels; i++)
	{
		attenuation_values_in_integer[i] = 10 * attenuation_data[i]; //255 = 10 * 25.5
		packet_to_transmit[9+(i*7)] = 0xA9;
		packet_to_transmit[10+(i*7)] = wss_id;
		packet_to_transmit[11+(i*7)] = altconf_id;
		packet_to_transmit[12+(i*7)] = channel_numbers[i];
		packet_to_transmit[13+(i*7)] = 0x01; //set point parameter
		packet_to_transmit[14+(i*7)] = port_data[i];
		packet_to_transmit[15+(i*7)] = attenuation_values_in_integer[i];
	}	

The 9th, 16th, 23rd,... bytes of the packet corresponds to ``Object ID 169(0xA9)``.
Bytes 10, 17, 21 etc,. corresponds to WSS ID. Similarly the position where the
data to be filled in command structure is known before and logic is written which
dynamically makes the command structure based on number of channels. Code snippet
above gets repeated for every channel presence.

Once the dynamic packet is formed into packet_to_transmit, length fields are updated
as below.

.. code-block::c

	packet_to_transmit[5] = strlen(packet_to_transmit) >> 8;
	packet_to_transmit[6] = strlen(packet_to_transmit) & 0xFF; //updating length bytes

Then the checksum is calculated using function ``calculate_crc_16_checksum`` which
uses CRC-16-IBM format. Finally stop bytes are added thus completing the command
structure.

.. code-block::c

	checksum = calculate_crc_16_checksum( packet_to_transmit[2], strlen(packet_to_transmit));
	packet_to_transmit[strlen(packet_to_transmit)] = checksum;
	packet_to_transmit[strlen(packet_to_transmit)] = 0xdd;
	packet_to_transmit[strlen(packet_to_transmit)] = 0x02;

UART port is used to transmit and receive data packets from/to Nistica WSS module.
Response for the transmit message is received from module through UART. Certain
validation in the receive packet such as Message ID and Result is done and status
is returned.

.. code-block::c
 
	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
 
	receive_packet_via_uart_port(uart_port_number, uart_received_packet_return,
 
	Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
	Validate RES -> SUCCESS == uart_received_packet_return[4];

