#ifndef DateTime_h
#define DateTime_h
#include "Date.h"
#include "Time.h"
#include <stdint.h>
#include <time.h>

class DateTime {
	public:
		DateTime();
		DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour=0, uint8_t minute=0, uint8_t second=0);
		DateTime(Date date, Time time);
		DateTime(time_t unixtime);
		uint32_t secondsSinceMidnight();
		uint32_t daysSinceEpoch();
		time_t unixtime();
		bool operator > (DateTime dt);
		bool operator < (DateTime dt);
		bool operator == (DateTime dt);
		DateTime operator + (HourSpan hours);
		DateTime operator - (HourSpan hours);
		inline Date date() { return _date; };
		inline Time time() { return _time; };
		inline uint16_t year() { return _date.year(); };
		inline uint8_t month() { return _date.month(); };
		inline uint8_t day() { return _date.day(); };
		inline uint8_t hour() { return _time.hour(); };
		inline uint8_t minute() { return _time.minute(); };
		inline uint8_t second() { return _time.second(); };
	private:
		Date _date;
		Time _time;
};
#endif
