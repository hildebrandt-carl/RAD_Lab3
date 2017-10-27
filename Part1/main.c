#include "header.h"
#include <stdio.h>

static struct etimer et;

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

	// Configure P2.2 to output ACLK for oscilloscope measurement
	P2SEL |= (1<<2);
	P2DIR |= (1<<2);
	PMAPPWD = 0x02D52;
 	P2MAP2 = PM_ACLK;
	
	// Configure P2.3 to output SMCLK for oscilloscope measurement
	P2SEL |= (1<<3);
	P2DIR |= (1<<3);
	PMAPPWD = 0x02D52;
 	P2MAP3 = PM_SMCLK;

	PMAPCTL |= PMAPRECFG; // set configuration
  	PMAPPWD = 0x00; // lock configuration
	
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

