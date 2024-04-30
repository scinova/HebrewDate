#include "Time.h"

HourSpan::HourSpan(uint8_t hours) {
	_hours = hours;
}

Time::Time() {
}

Time::Time(uint8_t hour, uint8_t minute, uint8_t second) {
	_hour = hour;
	_minute = minute;
	_second = second;
}

Time::Time(uint32_t seconds_since_midnight) {
	uint32_t s = seconds_since_midnight % 86400;
	_hour = s / (60 * 60);
	s -= _hour * (60 * 60);
	_minute = s / 60;
	s -= _minute * 60;
	_second = s;
}

uint32_t Time::secondsSinceMidnight() {
	return _second + 60 * _minute + 3600 * _hour;
}

bool Time::operator < (Time time) {
	return secondsSinceMidnight() < time.secondsSinceMidnight();
}

bool Time::operator <= (Time time) {
	return secondsSinceMidnight() <= time.secondsSinceMidnight();
}

bool Time::operator > (Time time) {
	return secondsSinceMidnight() > time.secondsSinceMidnight();
}

bool Time::operator >= (Time time) {
	return secondsSinceMidnight() >= time.secondsSinceMidnight();
}

bool Time::operator == (Time time) {
	return secondsSinceMidnight() == time.secondsSinceMidnight();
}

bool Time::operator != (Time time) {
	return secondsSinceMidnight() != time.secondsSinceMidnight();
}

Time Time::operator + (HourSpan hourspan) {
	uint8_t hour = (_hour + hourspan.hours()) % 24;
	Time t = Time(hour, _minute, _second);
	return t;
}

Time Time::operator - (HourSpan hourspan) {
	int16_t hour = (_hour - hourspan.hours()) % 24;
	if (hour < 0)
		hour += 24;
	Time t = Time(hour, _minute, _second);
	return t;
}
