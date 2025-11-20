/** 
 * @file    welcomehome.c
 * @brief   Simule le comportement d’un microcontrôleur
 *           - Serial : Communique via USB (USART)
 *           - Acq : Gère l'acquisition d'un signal discret et analogique
 *           - LED : Génère une commande PWM
 *           - RTC : Gère le temps réel (RTC)
 *           - EEPROM : Gère l'EEPROM 
 * @hardware:
 *          - PIC16F19156
 *          - TX2 : RC6
 *          - RX2 : RC7
 * @compilation:
 *          Linux/Mac : gcc device.c eeprom.c rtc.c serial.c -o device.exe ; gcc commande.c -o commande.exe
 *          Windows   : gcc device.c eeprom.c rtc.c serial.c -o device.exe -lws2_32 ; gcc commande.c -o commande.exe -lws2_32

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
RTC rtc = {0};
static time_t delay_start_time = 0;   // 

typedef struct {
    boolean btn_status;   	// 0=released, 1=pressed
    boolean led_status;     // 0=off, 1=on    
	uint16_t safety_delay; //ms  
    float sensor_value;   // valeur analogique
} DeviceState_t;
DeviceState_t state = {true,false,200,0.0};


// ==================== OUTPUT ====================
bool safe_delay_ok(void) {
    if (!state.btn_status){
        send_response("ERR DIS_STATE\n");
        return false;
    }
    time_t now = time(NULL);
    bool safety_delay_passed = (now - delay_start_time) >= state.safety_delay;
    if (safety_delay_passed == true){
        printf("[MCU] Safe delay terminated\n");
        return true;
    }else{
        printf("[MCU] Safe delay not terminated : %d >= %d \n",(now - delay_start_time),state.safety_delay);
        send_response("ERR SAFETY_DELAY\n");
    }
    return safety_delay_passed;
}

void close_device() {
    printf("[MCU] LED OFF\n");
	state.led_status = 0;
	send_response("ACK CLOSED\n");
}
void open_device() {
    if (!safe_delay_ok()) return;
    printf("[MCU] LED ON\n");
    state.led_status = 1;
	send_response("ACK OPENED\n");
}

// ==================== COMMANDS ====================

int execute_command(const char *cmd) {
    // --- Commandes Button ---
    if(strcmp(cmd,"btn out")==0) {
        state.btn_status=false;
        printf("[MCU] Button released\n");
        send_response("ACK DIS_LOW\n");
    } else if(strcmp(cmd,"btn in")==0) {
        state.btn_status=true;
        delay_start_time=time(NULL);
        printf("[MCU] Button pressed\n");
        send_response("ACK DIS_HGH\n");
    // --- Commandes  ---
    } else if (strcmp(cmd,"cmd on")==0) {
        open_device();
    } else if (strcmp(cmd,"cmd off")==0) {
        close_device();
    // --- Commandes Sensor ---
    } else if (strncmp(cmd,"set_analog",10)==0) {
        float val = 0.0f;
        if (sscanf(cmd,"set_analog %f",&val) == 1) {
            state.sensor_value = val;
            send_response("OK\n");
            WRITE_EEPROM_INT16(0x10,(int16_t)(state.sensor_value*100)); //sauvegarde dans l'eeprom
        } else {
            send_response("ERR Bad SET_ANALOG\n");
        }

    } else if (strncmp(cmd,"set_delay",9)==0) {
        int v = 0;
        if (sscanf(cmd,"set_delay %d",&v) == 1) {
            state.safety_delay = (int16_t)v;
            send_response("set_delay ok\n");
            WRITE_EEPROM_INT16(0x20,state.safety_delay); //sauvegarde dans l'eeprom
        } else {
            send_response("ERR Bad SET_DELAY\n");
        }
    // --- Sync time --- 
    } else if (strncmp(cmd,"status",6)==0) {
        //RTC_GetDateTime(&rtc);
        char resp[256];
        snprintf(resp,sizeof(resp),
                 "TIME=%s GOUP=%d ANALOG=%.2f DELAY=%d LED=%d\n",
                 rtc.timestr, state.btn_status, state.sensor_value, state.safety_delay, state.led_status);
        send_response(resp);
    } else if (strncmp(cmd,"date",4)==0) {
        RTC_GetDateTime(&rtc);
        char resp[128];
        snprintf(resp,sizeof(resp),"DATE=%s\n",rtc.timestr);
        send_response(resp);
    } else if (strncmp(cmd,"stamp",5)==0) {
        RTC_GetDateTime(&rtc);
        char resp[128];
        snprintf(resp,sizeof(resp),"STAMP=%d\n",rtc.timestamp);
        send_response(resp);
    } else if (strncmp(cmd,"sync",4)==0) {
        RTC_SyncWithSystem(&rtc);
        send_response("synchronized\n");
    } else if (strncmp(cmd,"quit",4)==0) {
        send_response("BYE\n");
        return -1; //break;
    } else {
        send_response("CMD UNKNOWN\n");
    }

    return 0;
}   

// ==================== MAIN ====================
void main(void)
{
    // ---------------- Initialisation -------------------
    load_eeprom(); // Charger EEPROM au démarrage
	RTC_Init(&rtc);

    serial_init();
    int client = -1;//serial_accept();
    
    state.sensor_value = (float)READ_EEPROM_INT16(0x10)/100.0f; //restauration de la valeur analogique
    state.safety_delay = READ_EEPROM_INT16(0x20); //restauration du delai de securite
    // ---------------- Boucle principale ----------------
    char buf[256];
    while(1) {
        while (client<0) client = serial_accept();
        
        //read from usb/observer
        receive_request(buf,256);
        if(strlen(buf) == 0){
            //printf("loop running\n");
            continue;
        }else{
            printf("[DEVICE] Recv: %s [%d]\n",buf,strlen(buf));
            if(execute_command(buf)<0) {
                break;
            }
        }

    }

    printf("[DEVICE] Arret.\n");
    //return 0;
}
