/** 
 * @file    rtc.c
 * @brief   Simule le comportement d'une horloge temps réel (RTC)
 */
#include <stdio.h>
#include <string.h>

#include "eeprom.h"
#include "rtc.h"



// Initialisation
void RTC_Init(RTC *rtc) {
    //memset(rtc,0,sizeof(RTC));
    //rtc->timestamp = time(NULL);
	RTC_LoadFromEEPROM(rtc,0x02);
    RTC_fromStamp(rtc);
	time_t now = time(NULL);
    rtc->delta = now - rtc->timestamp;
	
	printf("[RTC] delta with current time : %ld\n",rtc->delta);
}

// Compute time data from timestamp
void RTC_fromStamp(RTC *rtc){
    if (rtc->timestamp>=0){
        rtc->base_time = *localtime(&rtc->timestamp);
        //printf("[RTC] base ime from  %d\n",rtc->base_time);
        strftime(rtc->timestr, sizeof(rtc->timestr), "%d/%m/%Y %H:%M:%S", &rtc->base_time);
    }
}

// Synchroniser avec l'heure système
void RTC_SyncWithSystem(RTC *rtc) {
    rtc->delta = 0;
}

// Obtenir l'heure courante du RTC
void RTC_GetDateTime(RTC *rtc) {
    time_t now = time(NULL);
    //rtc->delta = now - rtc->timestamp;
    rtc->timestamp = now - rtc->delta;
	RTC_fromStamp(rtc);
	RTC_SaveToEEPROM(rtc,0x02);
}

// Sauvegarder la date/heure courante dans l'EEPROM
void RTC_SaveToEEPROM(RTC *rtc, uint8_t addr) {
    struct tm now;
    //RTC_GetDateTime(rtc,&now);
    RTC_fromStamp(rtc);
	now = rtc->base_time;
	
    // Stockage sur 7 octets : année (2), mois, jour, heure, minute, seconde
    int16_t year = now.tm_year + 1900;
    WRITE_EEPROM_INT16(addr, year);
    WRITE_EEPROM(addr+2, (int8_t)(now.tm_mon+1));
    WRITE_EEPROM(addr+3, (int8_t)now.tm_mday);
    WRITE_EEPROM(addr+4, (int8_t)now.tm_hour);
    WRITE_EEPROM(addr+5, (int8_t)now.tm_min);
    WRITE_EEPROM(addr+6, (int8_t)now.tm_sec);

    //printf("[RTC] Saved in EEPROM: %04d-%02d-%02d %02d:%02d:%02d\n",
    //       year, now.tm_mon+1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
}

// Charger la date/heure depuis l'EEPROM et resynchroniser
void RTC_LoadFromEEPROM(RTC *rtc,uint8_t addr) {
    int16_t year = READ_EEPROM_INT16(addr);
    int8_t month = READ_EEPROM(addr+2);
    int8_t day   = READ_EEPROM(addr+3);
    int8_t hour  = READ_EEPROM(addr+4);
    int8_t min   = READ_EEPROM(addr+5);
    int8_t sec   = READ_EEPROM(addr+6);

    struct tm t = {0};
    t.tm_year = year - 1900;
    t.tm_mon  = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min  = min;
    t.tm_sec  = sec;

    rtc->base_time = t;
    rtc->timestamp = mktime(&t);

    printf("[RTC] Loaded from EEPROM: %04d-%02d-%02d %02d:%02d:%02d\n",
           year, month, day, hour, min, sec);
}