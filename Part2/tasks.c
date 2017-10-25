#include "header.h"

PROCESS(TASK1,"TASK1 TASK");
PROCESS(TASK2,"TASK2 TASK");
PROCESS(TASK3,"TASK3 TASK");
PROCESS(WDTCHECK,"WDTCHECK TASK");

// Process 1: control GREEN LED
PROCESS_THREAD(TASK1,ev,data)
{	
	PROCESS_BEGIN();

	errorLogging("TASK 1 Finished task");

	PROCESS_END();
}

// Process 2: control YELLOW LED
PROCESS_THREAD(TASK2,ev,data)
{
	PROCESS_BEGIN();
	
	errorLogging("TASK 2 Finished task");

	PROCESS_END();
}

// Process 3: control RED LED
PROCESS_THREAD(TASK3,ev,data)
{
	PROCESS_BEGIN();

	errorLogging("TASK 3 Finished task");

	PROCESS_END();
}

// Function to kick the watchdog
PROCESS_THREAD(WDTCHECK,ev,data)
{
	PROCESS_BEGIN();

	uint8_t p = getPROGRESS();
	if((p & 0x0E) == 0x0E)
	{
		clearPROGRESS();
		errorLogging("Watchdog being kicked");
		WDTCTL = (WDTCTL & 0xFF) + WDTPW + WDTCNTCL; // kick watchdog
	}

	PROCESS_END();
}
