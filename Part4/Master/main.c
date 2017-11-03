#include "header.h"
#include "contiki.h"
#include <stdio.h>

// Other variables
static uint8_t PROGRESS=0;
static struct etimer et;
static volatile int virtualClock;

PROCESS(main_process, "Main Task");
AUTOSTART_PROCESSES(&main_process);

// Logs error or status information to stdout
void errorLogging(char* str)
{
	printf("Status Log: %s\r\n",str);
}

// Returns the value of the progress variable
uint8_t getPROGRESS()
{
	return PROGRESS;
}

// Clears the progress variable - sets all bits to 0
void clearPROGRESS()
{
	PROGRESS=0;
}

// Takes an integer which represents which task has completed and sets the PROGRESS bit for that task
void setPROGRESS(uint8_t i)
{
	switch(i)
	{
		case 1:
			PROGRESS |= (1<<1);
			break;
		case 2:
			PROGRESS |= (1<<2);
			break;
		case 3:
			PROGRESS |= (1<<3);
			break;
		default:
			errorLogging("ERROR: invalid input to setPROGRESS function");
			break;
	}
}

PROCESS_THREAD(main_process, ev, data)
{
	// Begin the process
	PROCESS_BEGIN();	
	
	// Sets up the watchdog timer to use ACLK input and an interval of 16s
	WDTCTL = WDTPW + WDTSSEL0 + WDTHOLD + WDTIS1 + WDTIS0;
	
	// Start the watchdog
	WDTCTL = (WDTCTL_L&~(WDTHOLD))+ WDTPW;
	
	TA1CTL = TASSEL0 + TAIE + MC0;
	//TA1CTL |= ~(ID0);
	//TA1CTL |= ~(ID1);
	TA1CCR0 = 0x0800;
	TA1CCTL0 = CCIE;

	// Begin status logging
	errorLogging("Starting up the system RAD_TEAM");
	static int iCnt = 0;
	static int SentTime = 0;

	//Start receiving
	rf1a_start_rx();

	static uint8_t oldReceivedMsg[50] = "" ;
	static uint8_t msg[50];

	P1OUT &= P1OUT & (0<<7);
	P3OUT &= P3OUT & (0<<6);
	P3OUT &= P3OUT & (0<<7);
	
	virtualClock = 0;
	while(1)
	{
		
		if(virtualClock % 16 == 8)
		{
			char secondCounterStr[50];
			sprintf(secondCounterStr,"System has been running for %d seconds.",virtualClock/16);
			errorLogging(secondCounterStr);
			SentTime = virtualClock;
			while(virtualClock % 16 == 8);
		}
		
		if(virtualClock % 48==4)
		{
			// Send message 12 characters long to the ID 4
			char msgSendingThing[50];
			printf("Requesting Virtual Clock\r\n");
			sprintf(msgSendingThing,"R%d",virtualClock);
			SentTime = virtualClock;
			unicast_send(msgSendingThing,12,4);

			while(virtualClock % 48==4);
		}

		// Call the different processes in tasks.c
		if((virtualClock % 96==0) || (virtualClock % 96==16))
		{
			process_start(&LED1,NULL);
			int currentClock = virtualClock;
			printf("LED 1 Change\r\n");
			while(currentClock == virtualClock);
		}
		else if((virtualClock % 96==32) || (virtualClock % 96==48))
		{
			process_start(&LED2,NULL);
			int currentClock = virtualClock;
			printf("LED 2 Change\r\n");
			while(currentClock == virtualClock);
		}
		else if((virtualClock % 96==64) || (virtualClock % 96==80))
		{
			process_start(&LED3,NULL);
			int currentClock = virtualClock;
			printf("LED 3 Change\r\n");
			while(currentClock == virtualClock);
		}

		getReceivedMessage(msg);
		if(strcmp(msg,oldReceivedMsg) != 0)
		{
			strcpy(oldReceivedMsg,msg);

			int InTime = atoi(msg);
			int difference = ((SentTime + virtualClock) / 2) - InTime;

			printf("The difference is: %d\r\n", difference);

			char returnMSG[50];
			sprintf(returnMSG,"C%d",difference);
			unicast_send(returnMSG,14,4);
		}
	}
	PROCESS_END();
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1A0ISR(void)
{
	virtualClock++;
	//printf("In the interrupt, clock is %d\r\n", virtualClock);
}

