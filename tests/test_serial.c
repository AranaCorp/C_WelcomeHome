/**
 * @file test_serial.c
 * @brief Emulate a basic serial communication with command.exe (Ctrl+C to stop)
 * 
 * @compilation:
 *      Win32: gcc  src\*.c -Iinclude tests\test_serial.c -o test_serial.exe -lws2_32

 *      Linux: gcc tests\test_file.c src\*.c -I includes -o build.exe

 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "serial.h"
// ==================== GLOBAL ====================
bool state = false;
char buf[256];

// ==================== SERIAL ====================
int execute_command(const char *cmd) {
    if (strcmp(cmd,"on")==0) {
        state = true;
        send_response("ACK ON\n");
    } else if (strcmp(cmd,"off")==0) {
        state = false;
        send_response("ACK OFF\n");
    } else if (strncmp(cmd,"quit",4)==0) {
        send_response("BYE\n");
        return -1; //break;
    } else {
        send_response("CMD UNKNOWN\n");
    }
    return 0;
}   

// ==================== MAIN ====================
int main() {
    //----------------- Initialization -----------------
    char name[] = "PRGM";
    printf("[%s] Initialized\n",name);
    
    serial_init();
    int client = -1;//serial_accept();

    //----------------- Main loop -----------------
    uint16_t counter = 0;
    while (1) {
        while (client<0) client = serial_accept();
        receive_request(buf,256);
        if(strlen(buf) == 0){
            if (counter % 10000 == 0) {
                //state = !state;
                printf("[%s] Main loop running... Counter=%d \tstate: %d\n", name, counter, state);
            }
        }else{

            printf("[%s] Recv: %s [%d]\n",name,buf,strlen(buf));
            if(execute_command(buf)<0) {
                break;
            }
        }
        counter++;
    }
    printf("[%s] Exited\n",name);
    return 0;
}