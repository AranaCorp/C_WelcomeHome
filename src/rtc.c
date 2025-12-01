/** 
 * @file    rtc.c
 * @brief   Simule le comportement d'une horloge temps réel (RTC)
 */
#include <stdio.h>
#include <string.h>

#include "eeprom.h"
#include "rtc.h"

// Initialisation
void rtc_init(RTC *rtc) {
    //memset(rtc,0,sizeof(RTC));
    //rtc->timestamp = time(NULL);
	rtc_load_from_eeprom(rtc,0x02);
    rtc_get_timestring(rtc);
	time_t now = time(NULL);
    rtc->delta = now - rtc->timestamp;
	
	printf("[RTC] delta with current time : %lld\n",rtc->delta);
}

// Compute time data from timestamp
void rtc_get_timestring(RTC *rtc) {
    if (rtc->timestamp >= 0)
    {
        rtc->base_time = *localtime(&rtc->timestamp);
        // printf("[RTC] base ime from  %d\n",rtc->base_time);
        strftime(rtc->timestr, sizeof(rtc->timestr), "%d/%m/%Y %H:%M:%S", &rtc->base_time);
    }
}

// Synchroniser avec l'heure système
void rtc_sync_time(RTC *rtc, time_t ts) {
    if(ts <= 0) ts = time(NULL);
    rtc->timestamp = ts;
    rtc->delta = 0;
    //send_response("ACK SYNC\n");
}

// Obtenir l'heure courante du RTC
void rtc_update_time(RTC *rtc) {
    rtc->timestamp += 1; //time in seconds
	rtc_save_to_eeprom(rtc,0x02);
}

// Sauvegarder la date/heure courante dans l'EEPROM
void rtc_save_to_eeprom(RTC *rtc, uint8_t addr) {
    rtc_get_timestring(rtc);
	struct tm now = rtc->base_time;
	
    // Stockage sur 7 octets : année (2), mois, jour, heure, minute, seconde
    int16_t year = now.tm_year + 1900;
    eeprom_write_int16(addr, year);
    eeprom_write(addr+2, (int8_t)(now.tm_mon+1));
    eeprom_write(addr+3, (int8_t)now.tm_mday);
    eeprom_write(addr+4, (int8_t)now.tm_hour);
    eeprom_write(addr+5, (int8_t)now.tm_min);
    eeprom_write(addr+6, (int8_t)now.tm_sec);

    //printf("[RTC] Saved in EEPROM: %04d-%02d-%02d %02d:%02d:%02d\n",
    //       year, now.tm_mon+1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
}

// Charger la date/heure depuis l'EEPROM et resynchroniser
void rtc_load_from_eeprom(RTC *rtc,uint8_t addr) {
    int16_t year = eeprom_read_int16(addr);
    int8_t month = eeprom_read(addr+2);
    int8_t day   = eeprom_read(addr+3);
    int8_t hour  = eeprom_read(addr+4);
    int8_t min   = eeprom_read(addr+5);
    int8_t sec   = eeprom_read(addr+6);

    struct tm t = {0};
    t.tm_year = year - 1900;
    t.tm_mon  = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min  = min;
    t.tm_sec  = sec;

    rtc->base_time = t;
    rtc->timestamp = mktime(&t);
    if (rtc->timestamp <= 0) rtc->timestamp = 1; 
    printf("[RTC] Loaded from EEPROM: %04d-%02d-%02d %02d:%02d:%02d\n",
           year, month, day, hour, min, sec);
}