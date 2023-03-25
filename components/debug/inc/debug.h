#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef DEBUG_SEGGER
    #include <SEGGER_RTT.h>

    #define DEBUG_PRINT_STRING(str) SEGGER_RTT_WriteString(0, (str));
    #define DEBUG_PRINT_INT(val) { char buf[16]; itoa(val, buf, 16); SEGGER_RTT_WriteString(0, buf); } 
#else
    #define DEBUG_PRINT_STRING(str)
    #define DEBUG_PRINT_INT(...)
#endif

void Debug_CrashHandler(void);
void Debug_PrintMemoryUsage(void);
void Debug_Breakpoint(void);

#endif // DEBUG_H_
