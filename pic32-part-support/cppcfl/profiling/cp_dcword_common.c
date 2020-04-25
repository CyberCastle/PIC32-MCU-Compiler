// Data capture word for MPLAB X to monitor

volatile int __DCWord __attribute__ ((persistent)); 

// Word transmit pause time for MPLAB X to initialize (based on user-selected
// clock speed). This uses the same IDE mechanism as for instrumented trace
// (and hence why this is not included in the data found in the TxCPData
// structure).

volatile int __DCDelay __attribute__ ((persistent));

