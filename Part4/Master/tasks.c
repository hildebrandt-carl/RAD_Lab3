#include "header.h"

PROCESS(LED1,"LED1 TASK");
PROCESS(LED2,"LED2 TASK");
PROCESS(LED3,"LED3 TASK");
//PROCESS(WDTCHECK,"WDTCHECK TASK");

// Process 1: control GREEN LED
PROCESS_THREAD(LED1,ev,data)
{	
	PROCESS_BEGIN();
	
	P1OUT ^= (1<<7); // toggle bit 7 in P1OUT (RED LED)
	P1DIR |= (1<<7); // toggle bit 7 in P1DIR (RED LED)
		
	setPROGRESS(1);
	
	//process_start(&WDTCHECK, NULL);
	PROCESS_END();
}

// Process 2: control YELLOW LED
PROCESS_THREAD(LED2,ev,data)
{
	PROCESS_BEGIN();

	P3OUT ^= (1<<7); // toggle bit 7 in P3OUT (YELLOW LED)
	P3DIR |= (1<<7); // toggle bit 7 in P3DIR (YELLOW LED)

	setPROGRESS(2);

	//process_start(&WDTCHECK, NULL);
	PROCESS_END();
}

// Process 3: control RED LED
PROCESS_THREAD(LED3,ev,data)
{
	PROCESS_BEGIN();

	P3OUT ^= (1<<6); // toggle bit 6 in P3OUT (GREEN LED)
	P3DIR |= (1<<6); // set bit 6 in P3DIR (GREEN LED)

	setPROGRESS(3);			

	//process_start(&WDTCHECK, NULL);
	PROCESS_END();
}

// Function to kick the watchdog
/*PROCESS_THREAD(WDTCHECK,ev,data)
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
}*/
