#ifndef Date_h
#define Date_h
#include <stdint.h>

class DaySpan {
	public:
		DaySpan(uint16_t days);
		inline uint16_t days() { return _days;};
	private:
		uint16_t _days;
};
 
class Date {
	public:
		static bool isLeapYear(uint16_t);
		static uint8_t numDaysInMonth(uint8_t month, uint16_t year);
		Date();
		Date(uint16_t year, uint8_t month, uint8_t day);
		Date(int32_t days_since_epoch);
		inline uint16_t year() { return _year; };
		inline uint8_t month() { return _month; };
		inline uint8_t day() { return _day; };
		int32_t daysSinceEpoch();
		uint8_t dayOfWeek();
		const char *monthName();
		const char *monthShortName();
		const char *dayOfWeekName();
		const char *dayOfWeekShortName();
		bool operator < (Date d);
		bool operator <= (Date d);
		bool operator == (Date d);
		bool operator >= (Date d);
		bool operator > (Date d);
		Date operator + (DaySpan);
		Date operator - (DaySpan);
	private:
		uint16_t _year;
		uint8_t _month;
		uint8_t _day;
};
#endif
