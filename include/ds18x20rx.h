#ifndef DS18X20RX_H_
#define DS18X20RX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>

// decicelsius functions disabled(0) or enabled(1):
#define DS18X20_DECICELSIUS       1
// max. resolution functions disabled(0) or enabled(1):
#define DS18X20_MAX_RESOLUTION    0


#if DS18X20_DECICELSIUS
extern int16_t DS18X20_raw_to_decicelsius( uint8_t reg, uint16_t measure );
extern uint8_t DS18X20_format_from_decicelsius( int16_t decicelsius, char s[], uint8_t n);
#endif /* DS18X20_DECICELSIUS */


#if DS18X20_MAX_RESOLUTION
// temperature unit for max. resolution is �C * 10e-4
// examples: -250625 -> -25.0625�C, 1250000 -> 125.0000 �C
extern int32_t DS18X20_raw_to_maxres( uint8_t reg, uint16_t measure);
extern uint8_t DS18X20_format_from_maxres( int32_t temperaturevalue, char s[], uint8_t n);
#endif /* DS18X20_MAX_RESOLUTION */


#ifdef __cplusplus
}
#endif

#endif
