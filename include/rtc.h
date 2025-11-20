#ifndef RTC_H
#define RTC_H
/** 
 * @file    rtc.h
 * @brief   Simule le comportement d'une horloge temps réel (RTC)
 */
#include <stdint.h>
#include <time.h>

typedef struct {
	time_t timestamp;
    struct tm base_time;
    char timestr[64];
	time_t delta;
} RTC;

void RTC_Init(RTC *rtc);
void RTC_fromStamp(RTC *rtc);
void RTC_SyncWithSystem(RTC *rtc);
void RTC_GetDateTime(RTC *rtc);
void RTC_SaveToEEPROM(RTC *rtc, uint8_t addr);
void RTC_LoadFromEEPROM(RTC *rtc, uint8_t addr);

#ifndef EEPROM_H
static inline int16_t READ_EEPROM_INT16(uint8_t addr) { return 0; }  
static inline void WRITE_EEPROM_INT16(uint8_t addr, int16_t val)  {}
static inline int8_t READ_EEPROM(uint8_t addr) { return 0; }  
static inline void WRITE_EEPROM(uint8_t addr,int8_t val)  {}
#endif // EEPROM_H

#endif // RTC_H