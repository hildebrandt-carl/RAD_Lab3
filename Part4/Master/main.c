#include "header.h"
#include "contiki.h"
#include <stdio.h>

// Quasi-Global variables
static volatile int virtualClock;
static uint8_t PROGRESS=0;

PROCESS(main_process, "Main Task");
AUTOSTART_PROCESSES(&main_process);

// Logs error or status information to stdout
void Logging(char* str)
{
	printf("TIME - %d : Status Log: %s\r\n",virtualClock/16,str);
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
			Logging("ERROR: invalid input to set PROGRESS function");
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
	Logging("Starting up the system RAD_TEAM");
	static int iCnt = 0;
	static int SentTime = 0;

	//Start receiving
	rf1a_start_rx();

	// Quasi-global variables used for communicating with the slave
	static uint8_t oldReceivedMsg[50] = "" ;
	static uint8_t msg[50];

	// Set all the LED's off
	P1OUT &= P1OUT & (0<<7);
	P3OUT &= P3OUT & (0<<6);
	P3OUT &= P3OUT & (0<<7);
	// Set all the LED's output direction to output
	P1DIR |= (1<<7);
	P3DIR |= (1<<6);
	P3DIR |= (1<<7);
	
	// Set the virtual clock to 0 as you enter the main loop
	virtualClock = 0;
	while(1)
	{
		// The virtual clock is running at 16Hz, so once every 16 clock ticks print out how many seconds
		// the program has been running for.
		if(virtualClock % 16 == 8)
		{
			// Print out how long the system has been running
			char secondCounterStr[50];
			sprintf(secondCounterStr,"System has been running for %d seconds.",virtualClock/16);
			// Log this using our logging function
			Logging(secondCounterStr);
			// Dont need this TODO
			SentTime = virtualClock;

			// Wait until this if statement is not true 
			// This stops this block being called twice
			while(virtualClock % 16 == 8);
		}
		
		if(virtualClock % 48==4)
		{
			// Save the sent time for clock desynchronization later
			SentTime = virtualClock;
			
			// Request the new virtual clock from the slave
			char msgSendingThing[50];
			Logging("Requesting Virtual Clock");
			sprintf(msgSendingThing,"R%d",virtualClock);
			unicast_send(msgSendingThing,12,4);

			// Wait until this if statement is not true 
			// This stops this block being called twice
			while(virtualClock % 48==4);
		}

		// If the virtual clock is 0 seconds, or 1 seconds
		if((virtualClock % 96==0) || (virtualClock % 96==16))
		{
			// Toggle LED1
			process_start(&LED1,NULL);
			Logging("LED 1 Change");

			// Wait for the virtual clock to change
			while(virtualClock%16==0);
		}
		// If the virtual clock is 2 seconds, or 3 seconds
		else if((virtualClock % 96==32) || (virtualClock % 96==48))
		{
			// Toggle LED1
			process_start(&LED2,NULL);
			Logging("LED 2 Change");
			
			// Wait for the virtual clock to change
			while(virtualClock%16==0);
		}
		// If the virtual clock is 4 seconds, or 5 seconds
		else if((virtualClock % 96==64) || (virtualClock % 96==80))
		{
			// Toggle LED3
			process_start(&LED3,NULL);
			Logging("LED 1 Change");
			
			// Wait for the virtual clock to change
			while(virtualClock%16==0);
		}

		// Check for new messages
		// This function was created in contiki by our team
		// It gets the previous message recieved by the wizzimote and stores it in msg
		getReceivedMessage(msg);

		// If this message has changed
		if(strcmp(msg,oldReceivedMsg) != 0)
		{
			// Copy this message into the old message variable
			// This stops the function from being called again
			strcpy(oldReceivedMsg,msg);

			// Convert the previous message into an integer
			int slaveTime = atoi(msg);
			// Calculate the difference between the clocks
			int difference = ((SentTime + virtualClock) / 2) - slaveTime;

			// Log the difference
			char messageLog[50];
			sprintf(messageLog,"The difference is: %d\r\n",difference);
			Logging(messageLog);

			// Send back the difference to the slave, so that the slave can correct this
			char returnMSG[50];
			sprintf(returnMSG,"C%d",difference);
			unicast_send(returnMSG,14,4);
		}
	}
	PROCESS_END();
}

// Interupt handler
// Timer1 A0 interupt
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1A0ISR(void)
{
	virtualClock++;
}

