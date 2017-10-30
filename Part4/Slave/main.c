#include "header.h"
#include "contiki.h"
#include <stdio.h>

// Counters to keep track of task progress
static uint16_t count=0;

// Other variables
static uint8_t PROGRESS=0;
static struct etimer et;

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

	// Begin status logging
	errorLogging("Starting up the system RAD_TEAM");
	static int iCnt = 0;

	rf1a_start_rx();

	while(1)
	{
 		// Delay 1 second - start the event timer and set its event period to 1 second
		etimer_set(&et, 1*CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		// Count how many seconds the system has been running and log this information
		iCnt++;	
		char* secondCounterStr[50];
		sprintf(secondCounterStr,"System has been running for %d seconds.",iCnt);
		errorLogging(secondCounterStr);		

		// Call the different processes in tasks.c
		if((count==0) || (count==1))
		{
			process_start(&LED1,NULL);
		}
		else if((count==2) || (count==3))
		{
			process_start(&LED2,NULL);
		}
		else if((count==4) || (count==5))
		{
			process_start(&LED3,NULL);
		}
		count++;
		if(count==6){count = 0;}	
	}
	PROCESS_END();

}

