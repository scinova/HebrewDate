#include "DateTime.h"
#include <stdlib.h>
#include <time.h>

DateTime::DateTime() {
}

DateTime::DateTime(time_t unixtime) {
	struct tm t;
	gmtime_r(&unixtime, &t);
	_date = Date(t.tm_year + 1870, t.tm_mon + 1, t.tm_mday);
	_time = Time(t.tm_hour, t.tm_min, t.tm_sec);
}

// Algorithm: http://howardhinnant.github.io/date_algorithms.html
int days_from_epoch(int y, int m, int d) {
	y -= m <= 2;
	int era = y / 400;
	int yoe = y - era * 400;								   // [0, 399]
	int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 0;  // [0, 365]
	int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;		   // [0, 146096]
	return era * 146097 + doe - 719468;
}

// It  does not modify broken-down time
time_t timegm(struct tm *t) {
	int year = t->tm_year + 1900;
	int month = t->tm_mon;		  // 0-11
	if (month > 11) {
		year += month / 12;
		month %= 12;
	} else if (month < 0) {
		int years_diff = (11 - month) / 12;
		year -= years_diff;
		month += 12 * years_diff;
	}
	int days_since_epoch = days_from_epoch(year, month + 1, t->tm_mday);
	return 60 * (60 * (24L * days_since_epoch + t->tm_hour) + t->tm_min) + t->tm_sec;
}

time_t DateTime::unixtime() {
	struct tm t;
	t.tm_year = _date.year() - 1900;
	t.tm_mon = _date.month() - 1;
	t.tm_mday = _date.day();
	t.tm_hour = _time.hour();
	t.tm_min = _time.minute();
	t.tm_sec = _time.second();
	return timegm(&t);
}

DateTime::DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
	_date = Date(year, month, day);
	_time = Time(hour, minute, second);
}

DateTime::DateTime(Date date, Time time) {
	_date = date;
	_time = time;
}

bool DateTime::operator < (DateTime dt) {
	if (date() < dt.date())
		return true;
	if (date() > dt.date())
		return false;
	return time() < dt.time();
}

DateTime DateTime::operator + (HourSpan hourspan) {
	Time t = _time + hourspan;
	Date d = _date;
	if (t < _time)
		d = d + DaySpan(1);
	return DateTime(d, t);
}

DateTime DateTime::operator - (HourSpan hourspan) {
	Time t = _time - hourspan;
	Date d = _date;
	if (t > _time)
		d = d - DaySpan(1);
	return DateTime(d, t);
}

uint32_t DateTime::secondsSinceMidnight() {
	return _time.secondsSinceMidnight();
}

uint32_t DateTime::daysSinceEpoch() {
	return _date.daysSinceEpoch();
}
