#ifndef Time_h
#define Time_h
#include <stdint.h>

class HourSpan {
	public:
		HourSpan(uint8_t hours);
		inline uint8_t hours() { return _hours; };
	private:
		uint8_t _hours;
};

class Time {
	public:
		Time();
		Time(uint8_t hour, uint8_t minute, uint8_t second);
		Time(uint32_t seconds_since_midnight);
		inline uint8_t hour() { return _hour; };
		inline uint8_t minute() { return _minute; };
		inline uint8_t second() { return _second; };
		uint32_t secondsSinceMidnight();
		bool operator < (Time t);
		bool operator <= (Time t);
		bool operator > (Time t);
		bool operator >= (Time t);
		bool operator != (Time t);
		bool operator == (Time t);
		Time operator + (HourSpan hours);
		Time operator - (HourSpan hours);
	private:
		uint8_t _hour;
		uint8_t _minute;
		uint8_t _second;
};
#endif
