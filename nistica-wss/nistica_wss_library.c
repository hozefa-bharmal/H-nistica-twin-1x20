/** nistica_wss_library.c
 * @file nistica_wss_library.c
 * 
 * @author S.Prasanna, Gattu Shivakrishna, Premsai, Shanmukha
 * 
 * @version 1.0
 * 
 * @brief This Module is Linux User Space Driver which communicates with Nistica WSS Module via UART.
 *
 * @details
 * The Nistica WSS library consists of functions to assign/switch/attenuate channels in Nistica WSS 
 * module. It acts as an interface to communicate and obtain several information from the WSS module.
 * General module configurations related to boot, firmware related operations, PIN controls, Waveplan
 * configuration and management, OCM power monitoring, querying performance and alarms, etc,. have been
 * provided APIs with which necessary operations can be done in WSS and OCM module.
 *
 * The UART configuration for Nistica WSS module is :
 *		115200 Baud rate, 8 bits, non-parity and 1 stop-bit.
 *
 * The Nistica WSS LUD categorizes APIs into the following types :
 *		1. General Module Information
 *		2. Channel Configurations
 *		3. Module Operation status
 *		4. Alarm Information
 *		5. Firmware Upgradation
 *
 * @date 22nd April 2020.
 */

#include <stdio.h>
#include <stdlib.h>
 
#include "nistica_wss_library.h"
 
 /************************************************
*************************************************
*		   Module Information
*************************************************
************************************************/

/**
* @brief Function will Get Vendor Name from Nistica WSS module
*
* This function reads the Module Information from WSS module and then the required
* data i.e., Vendor name is extracted. Function forms a command structure consisting
* of Message ID, Command, Object ID, Instance, Parameter and Checksum.
* The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
* '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
* synchronization between control module and WSS.
*
* Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
*		SUM (1 byte) = XOR from MID to PAR
*
* Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
*
* Function takes two arguments - UART port number and character pointer. UART port
* is used to transmit and receive data packets from/to Nistica WSS module. Certain
* validation in the receive packet such as Message ID, Length and Result is done
* by the function and the Vendor name data is extracted and returned.
* 
* @param uart_port_number
* an unsigned integer, represents the particular UART port to which Nistica WSS is connected
*
*	Example: 1
*
* @param vendor_name
* a character pointer, represents the data in which Vendor name is returned.
*
*	Example: Nistica
*
* @return an integer
*  <BR>  0  : Success
*  <BR> -1  : Failure
*/
int get_vendor_name_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *vendor_name )
{
	char packet_to_transmit[11]={0};
	char uart_received_packet_return[110]={0};

	int transmit_packet=0,
		receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	packet_to_transmit = { 0xdd, 0x01, 0x01, 0x05, READ_CMD, 0x06, 0x01, 0x00, 0x01, 0xdd, 0x02 };
	length_of_packet_to_transmit = strlen(packet_to_transmit); 

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
		printf("Error : Failed to transmit packet via UART Port in get_vendor_name_of_nistica_wss_module()\n");
		return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
   		printf("Error : Failed to receive packet via UART Port in get_vendor_name_of_nistica_wss_module()\n");
    	return FAILURE;
	}

	Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
	Validate RES -> SUCCESS == uart_received_packet_return[4];
	Validate LEN -> 0x6c == uart_received_packet_return[3];
	
	If all validation passes ->
				extract Vendor Name from uart_received_packet_return from 5th byte to 11th byte
	
	If any validation fail -> 
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get Minimum Frequency Bound value of Nistica WSS module
 *
 * This function reads the lower possible channel edge value from WSS module.
 * The value remains the lower boundary to define the waveplan allowed for the flexible
 * waveplan management commands.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x19; OBJ=0x80; INS=0x04; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the
 * function and the data is extracted and returned.
 * 
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param minimum_frequency_bound
 * an unsigned short pointer, represents the data in which Minimum Frequency bound is returned.
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/
int get_minimum_frequency_bound_of_nistica_wss_module( unsigned int uart_port_number, unsigned short* minimum_frequency_bound )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x19, 0x05, READ_CMD, 0x80, 0x04, 0x00, 0x9A, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
		receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit); 

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
		printf("Error : Failed to transmit packet via UART Port in get_minimum_frequency_bound_of_nistica_wss_module()\n");
		return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
   		printf("Error : Failed to receive packet via UART Port in get_minimum_frequency_bound_of_nistica_wss_module()\n");
    	return FAILURE;
	}

	Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
	Validate RES -> SUCCESS == uart_received_packet_return[4];
	
	If above validation passes ->
				extract Minimum Frequency bound value from uart_received_packet_return[5] and uart_received_packet_return[6], OR both bytes
    			multiply the result by 3.125 and store it in minimum_frequency_band
	
	If any validation fail -> 
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get Maximum Frequency Bound value of Nistica WSS module
 *
 * This function reads the upper possible channel edge value from WSS module.
 * The value remains the lower boundary to define the waveplan allowed for the flexible
 * waveplan management commands.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x19; OBJ=0x80; INS=0x05; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the
 * function and the data is extracted and returned.
 * 
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param maximum_frequency_bound
 * an unsigned short pointer, represents the data in which Maximum Frequency bound is returned.
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/
int get_maximum_frequency_bound_of_nistica_wss_module( unsigned int uart_port_number, unsigned short* maximum_frequency_bound )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x19, 0x05, READ_CMD, 0x80, 0x05, 0x00, 0x9B, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
		receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit); 

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
		printf("Error : Failed to transmit packet via UART Port in get_maximum_frequency_bound_of_nistica_wss_module()\n");
		return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
   		printf("Error : Failed to receive packet via UART Port in get_maximum_frequency_bound_of_nistica_wss_module()\n");
    	return FAILURE;
	}

	Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
	Validate RES -> SUCCESS == uart_received_packet_return[4];
	
	If above validation passes ->
				extract Maximum Frequency bound value from uart_received_packet_return[5] and uart_received_packet_return[6], OR both bytes
    			multiply the result by 3.125 and store it in maximum_frequency_band
	
	If any validation fail -> 
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get Minimum Channel Bandwidth value of Nistica WSS module
 *
 * This function reads the smallest channel that can be defined in WSS module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x19; OBJ=0x80; INS=0x04; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function
 * and Data is extracted and returned.
 * The value returned by the module will be in terms of 3.125 Ghz unit. Inorder to obtain
 * the actual Frequency in Ghz, resultant is multiplied with 3.125.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param minimum_channel_bandwidth
 * an unsigned short pointer, represents the data in which minimum channel bandwidth is returned.
 *
 *	Example: Nistica
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/
int get_minimum_channel_bandwidth_of_nistica_wss_module( unsigned int uart_port_number, unsigned short* minimum_channel_bandwidth )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x19, 0x05, READ_CMD, 0x80, 0x06, 0x00, 0x9C, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
		receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit); 

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
		printf("Error : Failed to transmit packet via UART Port in get_minimum_channel_bandwidth_of_nistica_wss_module()\n");
		return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
   		printf("Error : Failed to receive packet via UART Port in get_minimum_channel_bandwidth_of_nistica_wss_module()\n");
    	return FAILURE;
	}

	Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
	Validate RES -> SUCCESS == uart_received_packet_return[4];
	
	If above validation passes ->
				extract Minimum channel bandwidth value from uart_received_packet_return[5] and uart_received_packet_return[6], OR both bytes
    			multiply the result by 3.125 and store it in minimum_channel_bandwidth
	
	If any validation fail -> 
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get Maximum WSS ID value present Nistica WSS module
 *
 * This function reads the maximum WSS ID value that is defined in WSS module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x19; OBJ=0x80; INS=0x0F; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param maximum_wss_id_value
 * a signed short pointer, represents the data in which Maximum WSS ID value is returned.
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/
int get_maximum_wss_id_value_of_nistica_wss_module( unsigned int uart_port_number, short* maximum_wss_id_value )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x19, 0x05, READ_CMD, 0x80, 0x0F, 0x00, 0x91, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in get_minimum_channel_bandwidth_of_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in get_minimum_channel_bandwidth_of_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				extract Maximum WSS ID value from uart_received_packet_return[5] and uart_received_packet_return[6], OR it and store the result in maximum_wss_id_value

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get Maximum OCM ID value present Nistica WSS module
 *
 * This function reads the maximum OCM ID value that is defined in WSS module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x19; OBJ=0x80; INS=0x10; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param maximum_ocm_id_value
 * a signed short pointer, represents the data in which Maximum OCM ID value is returned.
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/
int get_maximum_ocm_id_value_of_nistica_wss_module( unsigned int uart_port_number, short* maximum_ocm_id_value )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x19, 0x05, READ_CMD, 0x80, 0x10, 0x00, 0x8E, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in get_minimum_channel_bandwidth_of_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in get_minimum_channel_bandwidth_of_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				extract Maximum OCM ID value from uart_received_packet_return[5] and uart_received_packet_return[6], OR it and store the result in maximum_ocm_id_value

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}


/**
 * @brief Function will Get Maximum Waveplan ID value present Nistica WSS module
 *
 * This function reads the maximum Waveplan ID value that is defined in WSS module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x19; OBJ=0x80; INS=0x12; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param maximum_waveplan_id_value
 * a signed short pointer, represents the data in which Maximum Waveplan ID value is returned.
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/
int get_maximum_waveplan_id_value_of_nistica_wss_module( unsigned int uart_port_number, short* maximum_waveplan_id_value )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x19, 0x05, READ_CMD, 0x80, 0x12, 0x00, 0x8E, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in get_maximum_waveplan_id_value_of_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in get_maximum_waveplan_id_value_of_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				extract Maximum Waveplan ID value from uart_received_packet_return[5] and uart_received_packet_return[6], OR it and store the result in maximum_ocm_id_value

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get Multiple Commands Fearture value present Nistica WSS module
 *
 * This function reads the multiple commands fearture value that is defined in WSS module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x10; OBJ=0x80; INS=0x02; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param multicmd
 * a signed short pointer, represents the data in which multiple commands fearture value is returned.
 * This variable will have '1' if the module supports long commands feature, '0' if not.
 *
 * @return an integer
 *  <BR>  0 : Success
 *  <BR> -1  : Failure
**/
int get_module_supports_multi_commands_of_nistica_wss_module( unsigned int uart_port_number, short* multicmd )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x10, 0x05, READ_CMD, 0x80, 0x02, 0x00, 0x95 , 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in get_module_supports_multi_commands_of_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in get_module_supports_multi_commands_of_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				extract multicmd value from uart_received_packet_return[5] and uart_received_packet_return[6], OR it and store the result in maximum_ocm_id_value

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get Table and Rows Fearture value present Nistica WSS module
 *
 * This function reads the table and rows fearture value that is defined in WSS module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x10; OBJ=0x80; INS=0x0E; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param table_row_cmd
 * a signed short pointer, represents the data in which table and rows fearture value is returned.
 * This variable will have '1' if the module supports long commands feature, '0' if not.
 *
 * @return an integer
 *  <BR>  0 : Success
 *  <BR> -1  : Failure
**/
int get_module_supports_table_and_row_commands_of_nistica_wss_module( unsigned int uart_port_number,short* table_row_cmd )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x11, 0x05, READ_CMD, 0x80, 0x0E, 0x00, 0x98, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in get_module_supports_table_and_row_commands_of_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in get_module_supports_table_and_row_commands_of_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				extract table_row_cmd value from uart_received_packet_return[5] and uart_received_packet_return[6], OR it and store the result in maximum_ocm_id_value

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get Long Commands Feature value present Nistica WSS module
 *
 * This function reads the long commands feature value that is defined in WSS module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x19; OBJ=0x80; INS=0x11; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param longcmd
 * a signed short pointer, represents the data in which Maximum Waveplan ID value is returned.
 * This variable will have '1' if the module supports long commands feature, '0' if not.
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/
int get_module_supports_long_cmd_format_of_nistica_wss_module( unsigned int uart_port_number,short* longcmd )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x12, 0x05, READ_CMD, 0x80, 0x11, 0x00, 0x84, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in get_module_supports_long_cmd_format_of_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in get_module_supports_long_cmd_format_of_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				extract longcmd value from uart_received_packet_return[5] and uart_received_packet_return[6], OR it and store the result in maximum_ocm_id_value

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get Broadcast Feature value present Nistica WSS module
 *
 * This function reads the broadcast feature value that is defined in WSS module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x19; OBJ=0x80; INS=0x0A; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param broadcast
 * a signed short pointer, represents the data in which Broadcast Feature value is returned.
 * This pointer will have '1' if the module supports Switching at the output ports at a time,
 * '0' if not.
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/
int get_module_supports_broadcast_of_nistica_wss_module( unsigned int uart_port_number,short* broadcast )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x13, 0x05, READ_CMD, 0x80, 0x0A, 0x00, 0x9E, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in get_module_supports_broadcast_of_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in get_module_supports_broadcast_of_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				extract broadcast value from uart_received_packet_return[5] and uart_received_packet_return[6], OR it and store the result in maximum_ocm_id_value

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get the result of any boot-time self-tests of Nistica WSS module
 *
 * This function reads the result of any boot-time self-tests like SDRAM,Flash image, 
 * calibration data, Optical switch hardware. PowerOnSelfTest failures 
 * leave the module in a non-operational state. It may not even be fully booted,
 * so there is no alarm mechanism running. The value can be checked along with the Module 
 * Status and the BootMode to get a full view of the current state of the module. 
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x01; OBJ=0x03; INS=0x01; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param power_test_result
 * a signed short pointer, It normally returns 0, indicating all tests have passed.
 * If errors occurred, the PowerOnSelfTest value is a bitmask.
 * The possible error bits include:
 *		0x0001 SDRAM test failed
 *		0x0002 Flash image verification failed
 *		0x0004 Calibration data verification failed
 *		0x0008 Optical Switch hardware failed
 *		0x0010 HWRESET held active > 2s ("safe mode")
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/

int get_power_on_self_test_results_of_nistica_wss_module( unsigned int uart_port_number, short* power_test_result )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x01, 0x05, READ_CMD, 0x03, 0x01, 0x00, 0x04, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in get_power_on_self_test_results_of_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in get_power_on_self_test_results_of_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				return the power_test_result

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get the result of Nistica WSS module is fully operational or not.
 *
 * This function reads the result module status, It returns 0 when the module is initializing,
 * and 1 when the module is fully operational. This can be queried to see if all boot sequencing
 * is complete.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x02; OBJ=0x04; INS=0x01; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param power_test_result
 * a signed short pointer, Represents module is fully operational or not.
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/

int get_module_status_of_nistica_wss_module( unsigned int uart_port_number, short* module_status )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x02, 0x05, READ_CMD, 0x04, 0x01, 0x00, 0x00, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in  get_module_status_of_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in  get_module_status_of_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				return the module_status

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get the return the type of reset that module was last experienced.
 *
 * This function returns the bootmode like no boot, cold boot,warm boot,hot boot, Watchdog reset.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x03; OBJ=0x91; INS=0x01; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param power_test_result
 * a signed short pointer, Represents bootmode.
 * The possible return values are:
 *   0: No boot since the Boot Mode was cleared with a write.
 *   1: Cold boot (initiated via power-on or the Module Reset pin)
 *   2: Warm boot (initiated via Hardware Reset pin)
 *   4: Watchdog reset (initiated by watchdog timeout)
 *   8: Hot boot (initiated via software command)
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/

int get_boot_mode_of_nistica_wss_module( unsigned int uart_port_number, short* boot_mode )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x03, 0x05, READ_CMD, 0x91, 0x01, 0x00, 0x94, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in  get_boot_mode_of_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in  get_boot_mode_of_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				return the boot_mode

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will set cold boot to the nistica module.
 *
 * This function reset the boot state as cold boot for module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR DATA SUM 0xdd 0x02
 *		MID=0x03; OBJ=0x91; INS=0x01; PAR=0x00; DATA=0x01 SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param power_test_result
 * a signed short pointer, Represents bootmode.
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/

int set_cold_boot_mode_for_nistica_wss_module( unsigned int uart_port_number, short* boot_mode )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x03, 0x05, WRITE_CMD, 0x91, 0x01, 0x00, 0x00, 0x01, 0x97, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in set_cold_boot_mode_for_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in set_cold_boot_mode_for_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				return the boot_mode

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will set warm boot to the nistica module.
 *
 * This function reset the boot state as warm boot for module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR DATA SUM 0xdd 0x02
 *		MID=0x03; OBJ=0x91; INS=0x01; PAR=0x00; DATA=0x02 SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param power_test_result
 * a signed short pointer, Represents bootmode.
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/

int set_warm_boot_mode_for_nistica_wss_module( unsigned int uart_port_number, short* boot_mode )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x03, 0x05, WRITE_CMD, 0x91, 0x01, 0x00, 0x00, 0x02, 0x94, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in set_warm_boot_mode_for_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in set_warm_boot_mode_for_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				return the boot_mode

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will set Watchdog reset boot to the nistica module.
 *
 * This function reset the boot state as Watchdog reset for module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR DATA SUM 0xdd 0x02
 *		MID=0x03; OBJ=0x91; INS=0x01; PAR=0x00; DATA=0x04 SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param power_test_result
 * a signed short pointer, Represents bootmode.
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/

int set_watchdog_reset_boot_mode_for_nistica_wss_module( unsigned int uart_port_number, short* boot_mode )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x03, 0x05, WRITE_CMD, 0x91, 0x01, 0x00, 0x00, 0x04, 0x92, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in set_Watchdog_reset_boot_mode_for_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in set_Watchdog_reset_boot_mode_for_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				return the boot_mode

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will set Hot boot to the nistica module.
 *
 * This function reset the boot state as Hot boot for module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR DATA SUM 0xdd 0x02
 *		MID=0x03; OBJ=0x91; INS=0x01; PAR=0x00; DATA=0x08 SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param power_test_result
 * a signed short pointer, Represents bootmode.
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/

int set_hot_boot_mode_for_nistica_wss_module( unsigned int uart_port_number, short* boot_mode )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x03, 0x05, WRITE_CMD, 0x91, 0x01, 0x00, 0x00, 0x08, 0x9E, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in set_hot_boot_mode_for_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in set_hot_boot_mode_for_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				return the boot_mode

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

 /************************************************
*************************************************
*		Channel Configurations
*************************************************
************************************************/
/**

/**
 * @brief Function will Set Waveplan in Nistica WSS module
 *
 * This function is used to set the Waveplan configuration into WSS module.
 * Function forms a command structure consisting of Message ID, Command, several
 * Object IDs set such as Waveplan First channel centre frequency, Per-Channel Bandwidth,
 * Waveplan Options, Number of Channels and lastly Clear to Waveplan that writes
 * the Waveplan into module. To each Object - Instance, Parameter and Checksum are
 * associated.
 *
 * Center frequency and Bandwidth is converted in units of 3.125 and then added
 * to the command structure. Example: Center Frequency = 191.3 Thz (61216 * 3.125)
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Function takes four arguments - UART port number, Number of channels, First
 * Channel Center Frequency and Bandwidth. UART port is used to transmit and
 * receive data packets from/to Nistica WSS module. Response for the transmit
 * message is received from module through UART. Certain validation in the receive
 * packet such as Message ID and Result is done and status is returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *	Example: 1
 *
 * @param number_of_channels
 * an unsigned short, represents the number of channels to be created in waveplan
 *	Example: 40
 *
 * @param center_frequency_in_Thz
 * an unsigned short, represents the number of channels to be created in waveplan
 *	Example: 191.3 Thz
 *
 * @param bandwidth_in_Ghz
 * an unsigned short, represents the number of channels to be created in waveplan
 *	Example: 50 Ghz
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/
int set_waveplan_of_nistica_wss_module( unsigned int uart_port_number, unsigned short number_of_channels, float center_frequency_in_Thz, float bandwidth_in_Ghz )
{
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	int center_frequency_in_3125_multiple = 1000 * (center_frequency_in_Thz / 3.125);
	int bandwidth_in_3125_multiple = 1000 * (bandwidth_in_Ghz / 3.125);
	
	unsigned char *number_of_channels_in_memory = (unsigned char*) &number_of_channels;
	unsigned char *center_frequency_in_memory = (unsigned char*) &center_frequency_in_3125_multiple;
	unsigned char *bandwidth_in_memory = (unsigned char*) &bandwidth_in_3125_multiple;
	
	unsigned char packet_to_transmit[] = {0xdd, 0x01, 0x01, 0x1B, MULTI_OBJ_WRITE, 
										0xA3, 0x01, 0x01, number_of_channels_in_memory[0], number_of_channels_in_memory[1],
										0xA0, 0x01, 0x01, center_frequency_in_memory[0], center_frequency_in_memory[1],
										0xA1, 0X01, 0X01, bandwidth_in_memory[0], bandwidth_in_memory[1],
										0XA2, 0X01, 0X01, 0X00, 0X01,
										0XA4, 0X01, 0X01, 0X00, 0X01,
										0x73, 0xdd, 0x02};

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in get_maximum_waveplan_id_value_of_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in get_maximum_waveplan_id_value_of_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				return SUCCESS

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

int set_channel_port_of_nistica_wss_module( unsigned int uart_port_number, unsigned short start_of_channel, 
										 unsigned short end_of_channel, char port_id)
{
	int length = end_of_channel + 5;
	char packet_to_transmit[] = { 0xdd, 0x01, 0x20, length, ARRAY_WRITE, 0xAA, start_of_channel, 0x01, end_of_channel, port_id,"calcludate the checksum", 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in  get_boot_mode_of_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in  get_boot_mode_of_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				return the SUCCESS

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

/**
* @brief Function will Get active partition number from Nistica WSS module
*
* This function reads the Flash Information from WSS module and then the required
* data i.e., active partition number is extracted. Function forms a command structure consisting
* of Message ID, Command, Object ID, Instance, Parameter and Checksum.
* The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
* '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
* synchronization between control module and WSS.
*
* Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
*		SUM (1 byte) = XOR from MID to PAR
*
* Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
*
* Function takes two arguments - UART port number and character pointer. UART port
* is used to transmit and receive data packets from/to Nistica WSS module. Certain
* validation in the receive packet such as Message ID, Length and Result is done
* by the function and the active partition number is extracted and returned.
* 
* @param uart_port_number
* an unsigned integer, represents the particular UART port to which Nistica WSS is connected
*
*	Example: 1
*
* @param active_partition_number
* a character pointer, represents the data in which active_partition_number is returned.
*
*	Example: 1
*
* @return an integer
*  <BR>  0  : Success
*  <BR> -1  : Failure
*/
int get_active_partition_number_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *active_partition_number )
{
	char packet_to_transmit[11]={0};
	char uart_received_packet_return[110]={0};

	int transmit_packet=0,
		receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	packet_to_transmit = { 0xdd, 0x01, 0x01, 0x05, READ_CMD, 0x0b, 0x01, 0x00, 0x0f, 0xdd, 0x02 };
	length_of_packet_to_transmit = strlen(packet_to_transmit); 

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
		printf("Error : Failed to transmit packet via UART Port in get_active_partition_of_nistica_wss_module()\n");
		return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
   		printf("Error : Failed to receive packet via UART Port in get_active_partition_of_nistica_wss_module()\n");
    	return FAILURE;
	}

	Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
	Validate RES -> SUCCESS == uart_received_packet_return[4];
	Validate LEN -> 0x66 == uart_received_packet_return[3];
	
	If all validation passes ->
				extract active partition number from uart_received_packet_return from 5th byte
	
	If any validation fail -> 
				Return error with appropriate error message/reason for failure
}

/**
* @brief Function will Get Booted partition number from Nistica WSS module
*
* This function reads the Flash Information from WSS module and then the required
* data i.e., Booted partition number is extracted. Function forms a command structure consisting
* of Message ID, Command, Object ID, Instance, Parameter and Checksum.
* The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
* '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
* synchronization between control module and WSS.
*
* Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
*		SUM (1 byte) = XOR from MID to PAR
*
* Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
*
* Function takes two arguments - UART port number and character pointer. UART port
* is used to transmit and receive data packets from/to Nistica WSS module. Certain
* validation in the receive packet such as Message ID, Length and Result is done
* by the function and the Booted partition number is extracted and returned.
* 
* @param uart_port_number
* an unsigned integer, represents the particular UART port to which Nistica WSS is connected
*
*	Example: 1
*
* @param booted_partition_number
* a character pointer, represents the data in which booted_partition_number is returned.
*
*	Example: 2
*
* @return an integer
*  <BR>  0  : Success
*  <BR> -1  : Failure
*/
int get_booted_partition_number_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *booted_partition_number )
{
	char packet_to_transmit[11]={0};
	char uart_received_packet_return[110]={0};

	int transmit_packet=0,
		receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	packet_to_transmit = { 0xdd, 0x01, 0x01, 0x05, READ_CMD, 0x0b, 0x01, 0x00, 0x0f, 0xdd, 0x02 };
	length_of_packet_to_transmit = strlen(packet_to_transmit); 

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
		printf("Error : Failed to transmit packet via UART Port in get_booted_partition_of_nistica_wss_module()\n");
		return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
   		printf("Error : Failed to receive packet via UART Port in get_booted_partition_of_nistica_wss_module()\n");
    	return FAILURE;
	}

	Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
	Validate RES -> SUCCESS == uart_received_packet_return[4];
	Validate LEN -> 0x66 == uart_received_packet_return[3];
	
	If all validation passes ->
				extract booted partition number from uart_received_packet_return from 7th byte
	
	If any validation fail -> 
				Return error with appropriate error message/reason for failure
}

/**
* @brief Function will get partition 1 firmware version from Nistica WSS module
*
* This function reads the Flash Information from WSS module and then the required
* data i.e., partition 1 firmware version is extracted. Function forms a command
* structure consisting of Message ID, Command, Object ID, Instance, Parameter and
* Checksum. The Transmit and Receive packet is encoded with '0xdd 0x01' in the
* beginning and 0xdd 0x02' in the end representing RS-232 frame marker required to
* ensure synchronization between control module and WSS.
*
* Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
*		SUM (1 byte) = XOR from MID to PAR
*
* Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
*
* Function takes two arguments - UART port number and character pointer. UART port
* is used to transmit and receive data packets from/to Nistica WSS module. Certain
* validation in the receive packet such as Message ID, Length and Result is done
* by the function and the partition 1 firmware version is extracted and returned.
* 
* @param uart_port_number
* an unsigned integer, represents the particular UART port to which Nistica WSS is connected
*
*	Example: 1
*
* @param firmware_version
* a character pointer, represents the data in which partition_1_firmware_version is returned.
*
*	Example: 1.2.0.73
*
* @return an integer
*  <BR>  0  : Success
*  <BR> -1  : Failure
*/

int get_partition_1_firmware_version_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *firmware_version )
{
	char packet_to_transmit[11]={0};
	char uart_received_packet_return[110]={0};

	int transmit_packet=0,
		receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	packet_to_transmit = { 0xdd, 0x01, 0x01, 0x05, READ_CMD, 0x0b, 0x01, 0x00, 0x0f, 0xdd, 0x02 };
	length_of_packet_to_transmit = strlen(packet_to_transmit); 

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
		printf("Error : Failed to transmit packet via UART Port in get_partition_1_firmware_of_nistica_wss_module()\n");
		return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
   		printf("Error : Failed to receive packet via UART Port in get_partition_1_firmware_of_nistica_wss_module()\n");
    	return FAILURE;
	}

	Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
	Validate RES -> SUCCESS == uart_received_packet_return[4];
	Validate LEN -> 0x66 == uart_received_packet_return[3];
	
	If all validation passes ->
				extract partition_1_firmware number from uart_received_packet_return from 9th byte to 16th byte
	
	If any validation fail -> 
				Return error with appropriate error message/reason for failure
}

/**
* @brief Function will get partition 2 firmware version from Nistica WSS module
*
* This function reads the Flash Information from WSS module and then the required
* data i.e., partition 2 firmware version is extracted. Function forms a command
* structure consisting of Message ID, Command, Object ID, Instance, Parameter and
* Checksum. The Transmit and Receive packet is encoded with '0xdd 0x01' in the
* beginning and 0xdd 0x02' in the end representing RS-232 frame marker required to
* ensure synchronization between control module and WSS.
*
* Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
*		SUM (1 byte) = XOR from MID to PAR
*
* Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
*
* Function takes two arguments - UART port number and character pointer. UART port
* is used to transmit and receive data packets from/to Nistica WSS module. Certain
* validation in the receive packet such as Message ID, Length and Result is done
* by the function and the partition 2 firmware version is extracted and returned.
* 
* @param uart_port_number
* an unsigned integer, represents the particular UART port to which Nistica WSS is connected
*
*	Example: 1
*
* @param firmware_version
* a character pointer, represents the data in which partition_2_firmware_version is returned.
*
*	Example: 2.0.0.76
*
* @return an integer
*  <BR>  0  : Success
*  <BR> -1  : Failure
*/

int get_partition_2_firmware_version_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *firmware_version )
{
	char packet_to_transmit[11]={0};
	char uart_received_packet_return[110]={0};

	int transmit_packet=0,
		receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	packet_to_transmit = { 0xdd, 0x01, 0x01, 0x05, READ_CMD, 0x0b, 0x01, 0x00, 0x0f, 0xdd, 0x02 };
	length_of_packet_to_transmit = strlen(packet_to_transmit); 

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
		printf("Error : Failed to transmit packet via UART Port in get_partition_2_firmware_of_nistica_wss_module()\n");
		return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
   		printf("Error : Failed to receive packet via UART Port in get_partition_2_firmware_of_nistica_wss_module()\n");
    	return FAILURE;
	}

	Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
	Validate RES -> SUCCESS == uart_received_packet_return[4];
	Validate LEN -> 0x66 == uart_received_packet_return[3];
	
	If all validation passes ->
				extract partition_2_firmware number from uart_received_packet_return from 18th byte to 25th byte
	
	If any validation fail -> 
				Return error with appropriate error message/reason for failure
}

/**
* @brief Function will get bootloader version from Nistica WSS module
*
* This function reads the Flash Information from WSS module and then the required
* data i.e., bootloader version is extracted. Function forms a command
* structure consisting of Message ID, Command, Object ID, Instance, Parameter and
* Checksum. The Transmit and Receive packet is encoded with '0xdd 0x01' in the
* beginning and 0xdd 0x02' in the end representing RS-232 frame marker required to
* ensure synchronization between control module and WSS.
*
* Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
*		SUM (1 byte) = XOR from MID to PAR
*
* Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
*
* Function takes two arguments - UART port number and character pointer. UART port
* is used to transmit and receive data packets from/to Nistica WSS module. Certain
* validation in the receive packet such as Message ID, Length and Result is done
* by the function and the bootloader version is extracted and returned.
* 
* @param uart_port_number
* an unsigned integer, represents the particular UART port to which Nistica WSS is connected
*
*	Example: 1
*
* @param bootloader_version
* a character pointer, represents the data in which bootloader_version is returned.
*
*	Example: 1.2.0
*
* @return an integer
*  <BR>  0  : Success
*  <BR> -1  : Failure
*/

int get_bootloader_version_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *bootloader_version )
{
	char packet_to_transmit[11]={0};
	char uart_received_packet_return[110]={0};

	int transmit_packet=0,
		receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	packet_to_transmit = { 0xdd, 0x01, 0x01, 0x05, READ_CMD, 0x0b, 0x01, 0x00, 0x0f, 0xdd, 0x02 };
	length_of_packet_to_transmit = strlen(packet_to_transmit); 

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
		printf("Error : Failed to transmit packet via UART Port in get_bootloader_version_of_nistica_wss_module()\n");
		return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
   		printf("Error : Failed to receive packet via UART Port in get_bootloader_version_of_nistica_wss_module()\n");
    	return FAILURE;
	}

	Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
	Validate RES -> SUCCESS == uart_received_packet_return[4];
	Validate LEN -> 0x66 == uart_received_packet_return[3];
	
	If all validation passes ->
				extract bootloader_version number from uart_received_packet_return from 27th byte to 31th byte
	
	If any validation fail -> 
				Return error with appropriate error message/reason for failure
}

/**
* @brief Function will get module uptime low word from Nistica WSS module
*
* This function reads the uptime Information from WSS module and then the required
* data i.e., module uptime low word is extracted. Function forms a command
* structure consisting of Message ID, Command, Object ID, Instance, Parameter and
* Checksum. The Transmit and Receive packet is encoded with '0xdd 0x01' in the
* beginning and 0xdd 0x02' in the end representing RS-232 frame marker required to
* ensure synchronization between control module and WSS.
*
* Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
*		SUM (1 byte) = XOR from MID to PAR
*
* Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
*
* Function takes two arguments - UART port number and character pointer. UART port
* is used to transmit and receive data packets from/to Nistica WSS module. Certain
* validation in the receive packet such as Message ID, Length and Result is done
* by the function and the module uptime low word is extracted and returned.
* 
* @param uart_port_number
* an unsigned integer, represents the particular UART port to which Nistica WSS is connected
*
*	Example: 1
*
* @param module_uptime_low
* a character pointer, represents the data in which module uptime low word is returned.
*
*	Example: 1
*
* @return an integer
*  <BR>  0  : Success
*  <BR> -1  : Failure
*/

int get_module_uptime_low_word_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *module_uptime_low )
{
	char packet_to_transmit[11]={0};
	char uart_received_packet_return[75]={0};

	int transmit_packet=0,
		receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	packet_to_transmit = { 0xdd, 0x01, 0x01, 0x05, READ_CMD, 0x92, 0x01, 0x00, 0x96, 0xdd, 0x02 };
	length_of_packet_to_transmit = strlen(packet_to_transmit); 

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
		printf("Error : Failed to transmit packet via UART Port in get_module_uptime_low_word_of_nistica_wss_module()\n");
		return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
   		printf("Error : Failed to receive packet via UART Port in get_module_uptime_low_word_of_nistica_wss_module()\n");
    	return FAILURE;
	}

	Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
	Validate RES -> SUCCESS == uart_received_packet_return[4];
		
	If all validation passes ->
				extract module uptime low word from uart_received_packet_return from 5th byte
	
	If any validation fail -> 
				Return error with appropriate error message/reason for failure
}

/**
* @brief Function will get module uptime high word from Nistica WSS module
*
* This function reads the uptime Information from WSS module and then the required
* data i.e., module uptime high word is extracted. Function forms a command
* structure consisting of Message ID, Command, Object ID, Instance, Parameter and
* Checksum. The Transmit and Receive packet is encoded with '0xdd 0x01' in the
* beginning and 0xdd 0x02' in the end representing RS-232 frame marker required to
* ensure synchronization between control module and WSS.
*
* Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
*		SUM (1 byte) = XOR from MID to PAR
*
* Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
*
* Function takes two arguments - UART port number and character pointer. UART port
* is used to transmit and receive data packets from/to Nistica WSS module. Certain
* validation in the receive packet such as Message ID, Length and Result is done
* by the function and the module uptime high word is extracted and returned.
* 
* @param uart_port_number
* an unsigned integer, represents the particular UART port to which Nistica WSS is connected
*
*	Example: 1
*
* @param module_uptime_high
* a character pointer, represents the data in which module uptime high word is returned.
*
*	Example: 1
*
* @return an integer
*  <BR>  0  : Success
*  <BR> -1  : Failure
*/

int get_module_uptime_high_word_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *module_uptime_high )
{
	char packet_to_transmit[11]={0};
	char uart_received_packet_return[75]={0};

	int transmit_packet=0,
		receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	packet_to_transmit = { 0xdd, 0x01, 0x01, 0x05, READ_CMD, 0x92, 0x02, 0x00, 0x95, 0xdd, 0x02 };
	length_of_packet_to_transmit = strlen(packet_to_transmit); 

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
		printf("Error : Failed to transmit packet via UART Port in get_module_uptime_high_word_of_nistica_wss_module()\n");
		return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
   		printf("Error : Failed to receive packet via UART Port in get_module_uptime_high_word_of_nistica_wss_module()\n");
    	return FAILURE;
	}

	Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
	Validate RES -> SUCCESS == uart_received_packet_return[4];
		
	If all validation passes ->
				extract module uptime low word from uart_received_packet_return from 38th byte
	
	If any validation fail -> 
				Return error with appropriate error message/reason for failure
}

/**
* @brief Function will get module supports array command feature from Nistica WSS module
*
* This function reads the ArrayCmdsFeature Information from WSS module and then the required
* data i.e., module supports array command is extracted. Function forms a command
* structure consisting of Message ID, Command, Object ID, Instance, Parameter and
* Checksum. The Transmit and Receive packet is encoded with '0xdd 0x01' in the
* beginning and 0xdd 0x02' in the end representing RS-232 frame marker required to
* ensure synchronization between control module and WSS.
*
* Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
*		SUM (1 byte) = XOR from MID to PAR
*
* Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
*
* Function takes two arguments - UART port number and character pointer. UART port
* is used to transmit and receive data packets from/to Nistica WSS module. Certain
* validation in the receive packet such as Message ID, Length and Result is done
* by the function and the status of module supports array command is extracted and returned.
* 
* @param uart_port_number
* an unsigned integer, represents the particular UART port to which Nistica WSS is connected
*
*	Example: 1
*
* @param status_of_array_cmd
* a character pointer, represents the data in which status of module supports array command is returned.
*
*	Example: 1
*
* @return an integer
*  <BR>  0  : Success
*  <BR> -1  : Failure
*/

int get_module_supports_array_commands_of_nistica_wss_module( unsigned int uart_port_number, unsigned int status_of_array_cmd )
{
	char packet_to_transmit[11]={0};
	char uart_received_packet_return[15]={0};

	int transmit_packet=0,
		receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	packet_to_transmit = { 0xdd, 0x01, 0x01, 0x05, READ_CMD, 0x78, 0x01, 0x00, 0x7c, 0xdd, 0x02 };
	length_of_packet_to_transmit = strlen(packet_to_transmit); 

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
		printf("Error : Failed to transmit packet via UART Port in get_module_supports_array_commands_of_nistica_wss_module()\n");
		return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
   		printf("Error : Failed to receive packet via UART Port in get_module_supports_array_commands_of_nistica_wss_module()\n");
    	return FAILURE;
	}

	Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
	Validate RES -> SUCCESS == uart_received_packet_return[4];
		
	If all validation passes ->
				extract module uptime low word from uart_received_packet_return from 5th byte
	
	If any validation fail -> 
				Return error with appropriate error message/reason for failure
}

