/** 
 * @file    welcomehome.c
 * @brief   Simule le comportement d’un microcontrôleur
 *           - Serial : Communique via USB (USART)
 *           - Acq : Gère l'acquisition d'un signal discret et analogique
 *           - LED : Génère une commande PWM
 *           - RTC : Gère le temps réel (RTC)
 *           - EEPROM : Gère l'EEPROM 
 * 
 * @compilation:
 *          Linux/Mac : gcc  src\*.c -Iinclude welcomehome.c -o welcomehome.exe ; gcc ./tests/commande.c -o commande.exe
 *          Windows   : gcc  src\*.c -Iinclude welcomehome.c -o welcomehome.exe -lws2_32 ; gcc ./tests/commande.c -o commande.exe -lws2_32

 */
// ==================== INCLUDES ====================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

#include "eeprom.h"
#include "rtc.h"
#include "serial.h"

// ==================== GLOBAL VARIABLES ====================
#define DEFAULT_SAFETY_DELAY 20u
#define TICK_INTERVAL_MS 100u
const double period_ticks = (TICK_INTERVAL_MS / 1000.0) * CLOCKS_PER_SEC;

RTC rtc = {0};
static unsigned long previous_time = 0;    /* elapsed since Start in ms */

typedef struct
{
    float sensor_value;    // valeur analogique
    uint16_t analog_value; // 0..1023
    uint16_t safety_delay; // ms
    bool btn_status;       // 0=released, 1=pressed
    bool cmd_status;       // 0=off, 1=on
    bool led_on;           // 0=off, 1=on
    bool armed;            // safety delay passed
} DeviceState_t;
DeviceState_t state = {0.0f, 0, DEFAULT_SAFETY_DELAY, false, false, false,false};
static time_t delay_start_time = 0;   // 

// ==================== OUTPUT ====================
bool safe_delay_ok(void) {
    if (!state.btn_status){
        send_response("ERR DIS_STATE\n");
        return false;
    }
    //time_t now = time(NULL);
    time_t now = rtc.timestamp;
    state.armed = (now - delay_start_time) >= state.safety_delay;
    if (state.armed == true){
        printf("[MCU] Safe delay terminated\n");
        return true;
    }else{
        printf("[MCU] Safe delay not terminated : %lld >= %d \n",(now - delay_start_time),state.safety_delay);
        send_response("ERR SAFETY_DELAY\n");
    }
    return state.armed;
}

void stop_device() {
	state.cmd_status = 0;
	send_response("ACK CLOSED\n");
}
void start_device() {
    if (!safe_delay_ok()) return;
    state.cmd_status = 1;
	send_response("ACK OPENED\n");
}

// ==================== REQUESTS ====================
int manage_request(const char *cmd) {
    // --- Commandes Button ---
    if(strcmp(cmd,"btn out")==0) {
        state.btn_status=false;
        printf("[MCU] Button released\n");
        send_response("ACK DIS_LOW\n");
        stop_device();
    } else if(strcmp(cmd,"btn in")==0) {
        state.btn_status=true;
        delay_start_time=rtc.timestamp; //time(NULL);
        printf("[MCU] Button pressed\n");
        send_response("ACK DIS_HGH\n");
    // --- Commandes  ---
    } else if (strcmp(cmd,"cmd on")==0) {
        printf("[MCU] CMD ON\n");
        start_device();
    } else if (strcmp(cmd,"cmd off")==0) {
        printf("[MCU] CMD OFF\n");
        stop_device();
    // --- Commandes Sensor ---
    } else if (strncmp(cmd,"set_analog",10)==0) {
        float val = 0.0f;
        if (sscanf(cmd,"set_analog %f",&val) == 1) {
            state.sensor_value = val;
            state.analog_value = (int)(val*1023/12.0);
            //printf("[MCU] Set analog to %.2f V (%d)\n",state.sensor_value,state.analog_value);
            send_response("ACK SET_ANALOG OK\n");
            eeprom_write_int16(0x10,(int16_t)(state.analog_value)); //sauvegarde dans l'eeprom
        } else {
            send_response("ERR Bad SET_ANALOG\n");
        }

    } else if (strncmp(cmd,"set_delay",9)==0) {
        int v = 0;
        if (sscanf(cmd,"set_delay %d",&v) == 1) {
            state.safety_delay = (int16_t)v;
            send_response("ACK SET_DELAY OK\n");
            eeprom_write_int16(0x20,state.safety_delay); //sauvegarde dans l'eeprom
        } else {
            send_response("ERR Bad SET_DELAY\n");
        }
    // --- Sync time --- 
    } else if (strncmp(cmd,"status",6)==0) {
        char resp[256];
        snprintf(resp,sizeof(resp),
                 "TIME=%s BTN=%d ANALOG=%.2f DELAY=%d LED=%d\n",
                 rtc.timestr, state.btn_status, state.sensor_value, state.safety_delay, state.cmd_status);
        send_response(resp);
    } else if (strncmp(cmd,"date",4)==0) {
        char resp[128];
        snprintf(resp,sizeof(resp),"DATE=%s\n",rtc.timestr);
        send_response(resp);
    } else if (strncmp(cmd,"stamp",5)==0) {
        char resp[128];
        snprintf(resp,sizeof(resp),"STAMP=%lld\n",rtc.timestamp);
        send_response(resp);
    } else if (strncmp(cmd,"sync",4)==0) {
        rtc_sync_time(&rtc);
        send_response("ACK SYNC\n");
    } else if (strncmp(cmd,"quit",4)==0) {
        send_response("BYE\n");
        return -1; //break;
    } else {
        send_response("CMD UNKNOWN\n");
    }

    return 0;
}   

// ==================== MAIN ====================
int main(void)
{
    // ---------------- Initialisation -------------------
    eeprom_load(); // Charger EEPROM au démarrage
	rtc_init(&rtc);

    serial_init();
    int client = -1;//serial_accept();
    
    state.analog_value = eeprom_read_int16(0x10); //restauration de la valeur analogique
    state.safety_delay = eeprom_read_int16(0x20); //restauration du delai de securite
    state.sensor_value = (float)(state.analog_value * 12.0 / 1023.0);
    previous_time = clock() + period_ticks;
    // ---------------- Boucle principale ----------------
    char buf[256];
    uint16_t counter = 0;
    while(1) {
        while (client<0) client = serial_accept();
        
        // RTC update every second
        if (clock() >= previous_time) {
            
            previous_time += period_ticks;
            state.led_on = !state.led_on; //toggle led
            if(counter % (int)(1000/TICK_INTERVAL_MS) == 0){
                rtc_update_time(&rtc); 
            }
            counter++;
        }    

        //read from usb/observer
        receive_request(buf,256);
        if(strlen(buf) == 0){
            //printf("loop running\n");
            continue;
        }else{
            printf("[DEVICE] Recv: %s [%lld]\n",buf,strlen(buf));
            if(manage_request(buf)<0) {
                break;
            }
        }

    }

    printf("[DEVICE] Arret.\n");
    return 0;
}
