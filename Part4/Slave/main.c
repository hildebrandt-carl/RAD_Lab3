#include "header.h"
#include "contiki.h"
#include <stdio.h> 

// Other variables
static uint8_t PROGRESS=0;
static volatile int virtualClock;

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

	// Start LEDs off
	P1OUT = 0; // turn LEDs off
	P3OUT = 0; // turn LEDs off

	// Begin status logging
	Logging("Starting up the system RAD_TEAM");

	//Start receiving
	rf1a_start_rx();

	// Quasi-global variables used for communicating with the master
	static uint8_t msg[50] = "";
	static uint8_t oldMessage[50] = "";

	// Set up timer 
	TA1CTL = TASSEL0 + TAIE + MC0;
	// Counts up to 2048 decimal (1/16) of a second
	TA1CCR0 = 0x0800;
	// Enable interupts
	TA1CCTL0 = CCIE;

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
		// If the virtual clock is 0 seconds, or 1 seconds
		if((virtualClock%96==0) || (virtualClock%96==16))
		{
			//Turn off other LEDs
			P3OUT &= P3OUT & ~(1<<7);
			P3OUT &= P3OUT & ~(1<<6);

			// Toggle LED1
			process_start(&LED1,NULL);
			Logging("LED 1 Change");

			// Wait for this value to change
			while(virtualClock%16==0);
		}
		// If the virtual clock is 2 seconds, or 3 seconds
		else if((virtualClock%96==32) || (virtualClock%96==48))
		{
			//Turn off other LEDs
			P1OUT &= P1OUT & ~(1<<7);
			P3OUT &= P3OUT & ~(1<<6);

			// Toggle LED2
			process_start(&LED2,NULL);
			Logging("LED 2 Change");

			// Wait for this value to change
			while(virtualClock%16==0);
		}
		// If the virtual clock is 4 seconds, or 5 seconds
		else if((virtualClock%96==64) || (virtualClock%96==80))
		{
			//Turn off other LEDs, toggle LED1
			P1OUT &= P1OUT & ~(1<<7);
			P3OUT &= P3OUT & ~(1<<7);

			// Toggle LED3
			process_start(&LED3,NULL);
			Logging("LED 3 Change");

			// Wait for this value to change
			while(virtualClock%16==0);
		}

		// Check for new messages
		// This function was created in contiki by our team
		// It gets the previous message recieved by the wizzimote and stores it in msg
		getReceivedMessage(msg);

		// If this message has changed
		if(strcmp(msg, oldMessage) != 0){
			// Print out the recieved message
			char logMsg[50] ;
			sprintf(logMsg,"Recieved Message : %s",msg) ; 
			Logging(logMsg);

			// Copy this message into the old message variable
			// This stops the function from being called again
			strcpy(oldMessage, msg);

			// If the first letter is "R"
			// This means request the clock value
			if(msg[0] == 'R')
			{
				// TODO: disable interrupts?

				// Send the current virtual clock value
				char sendMessage[50];
				sprintf(sendMessage,"%d",virtualClock);
				unicast_send(sendMessage,12,5);

			// If the first letter is "C"
			// This means change the value by
			}else if(msg[0] == 'C'){
				// TODO: disable interrupts?

				// Adjust the clock value by this much
				int adjustment = atoi(&msg[1]);
				virtualClock += adjustment;
			}
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


