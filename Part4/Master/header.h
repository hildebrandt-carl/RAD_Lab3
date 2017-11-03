#ifndef HEADER_H
#define HEADER_H

#include "contiki.h"
#include "platform.h"
 #include "dev/serial-line.h"
#include "config.h"

// Declare the processes
PROCESS_NAME(main_process); // main process
PROCESS_NAME(LED1); // process 1: control RED LED
PROCESS_NAME(LED2); // process 2: control YELLOW LED
PROCESS_NAME(LED3); // process 3: control GREEN LED
//PROCESS_NAME(WDTCHECK); // function to kick the watchdog

// Helper functions
uint8_t getPROGRESS();
void setPROGRESS(uint8_t);
void clearPROGRESS();

void errorLogging(char*);

#endif
