#include "header.h"
#include <stdio.h>

static struct etimer et;
static int virtualClock;

PROCESS(main_process, "Main Task");
AUTOSTART_PROCESSES(&main_process);

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

	TA1CCR0 = 0x8000;
	TA1CCTL0 = CCIE;
	
	virtualClock = 0;
	
	while(1)
	{
 		// Delay 1 second - start the event timer and set its event period to 1 second
		etimer_set(&et, 1*CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		// Kick the watchdog
		WDTCTL = (WDTCTL & 0xFF) + WDTPW + WDTCNTCL; // kick watchdog	
	}
	PROCESS_END();

}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1A0ISR(void)
{
	virtualClock++;
	printf("In the interrupt, clock is %d\r\n", virtualClock);
}


