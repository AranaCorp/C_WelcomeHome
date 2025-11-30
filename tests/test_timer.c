/**
 * @file basic_prgm.c
 * @brief Emulate a basic program (Ctrl+C to stop)
 * 
 * @compilation:
 *      Win32: gcc basic_prgm.c -o basic_prgm.exe ; ./basic_prgm.exe 
 *             ./run.bat basic_prgm.c  
 *      Linux: gcc basic_prgm.c -o basic_prgm ; ./basic_prgm
 *                 ./run.sh basic_prgm.c 
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

// ==================== GLOBAL ====================
#define TICK_INTERVAL_MS 200u
const double period_ticks = (TICK_INTERVAL_MS / 1000.0) * CLOCKS_PER_SEC;
time_t timestamp = 0;

bool state = false;

void timestamp_to_string(uint32_t ts, char *out)
{
    // Epoch: 01/01/2024 00:00:00
    struct tm base = {
        .tm_sec  = 0,
        .tm_min  = 0,
        .tm_hour = 0,
        .tm_mday = 1,
        .tm_mon  = 0,
        .tm_year = 124  // 2024 - 1900
    };

    time_t epoch = mktime(&base);
    time_t current = epoch + ts;
    struct tm *t = localtime(&current);

    sprintf(out, "%02d/%02d/%04d %02d:%02d:%02d",
            t->tm_mday,
            t->tm_mon + 1,
            t->tm_year + 1900,
            t->tm_hour,
            t->tm_min,
            t->tm_sec);
}

// ==================== MAIN ====================
int main() {
    //----------------- Initialization -----------------
    char name[] = "PRGM";
    printf("[%s] Initialized\n",name);
    double elapsed_time = clock() + period_ticks;
    char timebuf[32] = {0};
    //----------------- Main loop -----------------
    uint16_t counter = 0;
    while (1) {
        // Simulate MCU tasks
        double now = clock();
        if (now >= elapsed_time)
        {   
            elapsed_time += period_ticks;
            state = !state;
            counter++;
            if(counter % (int)(1000/TICK_INTERVAL_MS) == 0){
                timestamp++;
                timestamp_to_string(timestamp, timebuf);
            }

            printf("[%s] Main loop running... Counter=%d , time=%s, ticks: %f, LED: %d\n", name, counter, timebuf , (double)((elapsed_time-now)/CLOCKS_PER_SEC*1000), state);

        }
    }
    printf("[%s] Exited\n",name);
    return 0;
}