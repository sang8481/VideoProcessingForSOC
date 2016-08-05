/************************************************************************
  Title     : Robot Body Protocol Source File
  File name : robot_protocol.c

  Author    : adc inc. (oxyang@adc.co.kr)
  History
		+ v0.0  2007/2/14
		+ v1.0  2008/8/6
************************************************************************/
#include <stdio.h>
#include <string.h>
#include "robot_protocol.h"
#include "uart_api.h"
#include "amazon2_sdk.h"
#include "graphic_api.h"
//////////////////////////////////////////////////// Protocol Test

void DelayLoop(int delay_time)
{
	while(delay_time)
		delay_time--;
}

void Send_Command(unsigned char command)
{
	int i;
	unsigned char Command_Buffer[1] = {0,};

	Command_Buffer[0] = command;	// Command Byte
	 	for(i=0; i<1; i++) printf("send 0x%x\n",Command_Buffer[i]);

	uart1_buffer_write(Command_Buffer, 1);
}

unsigned char Receive_Ack(void){
	int i;
	unsigned char Receiving_Buffer[1] = {0,};
	uart1_buffer_read(Receiving_Buffer, 1);
	for(i = 0; i < 1; i++) printf("Received 0x%x\n", Receiving_Buffer[i]);
	return Receiving_Buffer[0];
}

#define ERROR 0
#define OK	1

/* Command Function */
void walk(void)
{
	Send_Command(1);
	printf("Command 1.\n");
}

void tumbling(void){
	Send_Command(2);
	printf("Command 2.\n");
}
