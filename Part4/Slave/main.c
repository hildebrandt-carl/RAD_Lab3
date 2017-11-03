#include "header.h"
#include "contiki.h"
#include <stdio.h>

// Counters to keep track of task progress
static uint16_t count=0;

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

	// Start LEDs off
	P1OUT = 0; // turn LEDs off
	P3OUT = 0; // turn LEDs off

	// Begin status logging
	errorLogging("Starting up the system RAD_TEAM");

	rf1a_start_rx();

	// Set up Timer	
	TA1CTL = TASSEL0 + TAIE + MC0;
	TA1CCR0 = 0x800;
	TA1CCTL0 = CCIE;
	
	virtualClock = 0;

	while(1)
	{
 		// Delay 1 second - start the event timer and set its event period to 1 second
		//etimer_set(&et, 1*CLOCK_SECOND);
		//PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));


		// Call the different processes in tasks.c
		if((virtualClock%96==0) || (virtualClock%96==16))
		{
			//Turn off other LEDs, toggle LED1
			P3OUT &= P3OUT & ~(1<<7);
			P3OUT &= P3OUT & ~(1<<6);
			process_start(&LED1,NULL);
			while(virtualClock%16==0);
		}
		else if((virtualClock%96==32) || (virtualClock%96==48))
		{
			//Turn off other LEDs, toggle LED1
			P1OUT &= P1OUT & ~(1<<7);
			P3OUT &= P3OUT & ~(1<<6);
			process_start(&LED2,NULL);
			while(virtualClock%16==0);
		}
		else if((virtualClock%96==64) || (virtualClock%96==80))
		{
			//Turn off other LEDs, toggle LED1
			P1OUT &= P1OUT & ~(1<<7);
			P3OUT &= P3OUT & ~(1<<7);
			process_start(&LED3,NULL);
			while(virtualClock%16==0);
		}

		// Get received message
		static uint8_t msg[50];
		static uint8_t oldMessage[50];
		getReceivedMessage(msg);
		if(strcmp(msg, oldMessage) != 0){
			printf("%s\r\n",msg);
			strcpy(oldMessage, msg);
			if(msg[0] == 'R'){
				char sendMessage[50];
				//TODO: disable interrupts?
				int clock = virtualClock;
				sprintf(sendMessage,"%d",clock);
				unicast_send(sendMessage,12,5);
			}else if(msg[0] == 'C'){
				//TODO: disable interrupts?
				int adjustment = atoi(&msg[1]);
				virtualClock += adjustment;
			}
		}
	}
	PROCESS_END();

}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1A0ISR(void)
{
	virtualClock++;
	//printf("In the interrupt, clock is %d in 16th of a second\r\n", virtualClock);
}


