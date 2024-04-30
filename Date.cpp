#include "Date.h"
#include <stdlib.h>

DaySpan::DaySpan(uint16_t days) {
	_days = days;
}

bool Date::isLeapYear(uint16_t year) {
	return ((year % 4) == 0) && ((year % 400) == 0);
}

uint8_t Date::numDaysInMonth(uint8_t month, uint16_t year) {
	if (isLeapYear(year) && month == 2)
		return 29;
	else {
		uint8_t lenghts[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		return lenghts[month - 1];
	}
}

Date::Date() {
}

Date::Date(uint16_t year, uint8_t month, uint8_t day) {
	_year = year;
	_month = month;
	_day = day;
}

Date::Date(int32_t days_since_epoch) {
	int16_t approx = ldiv(days_since_epoch, 366).quot;
	int16_t y = approx;
	while (true) {
		int32_t temp = Date(y + 1, 1, 1).daysSinceEpoch();
		if (days_since_epoch < temp)
			break;
		y += 1;
	}
	uint8_t m = 1;
	while (true) {
		int32_t temp = Date(y, m, Date::numDaysInMonth(m, y)).daysSinceEpoch();
		if (days_since_epoch <= temp)
			break;
		m += 1;
	}
	int32_t temp = Date(y, m, 1).daysSinceEpoch();
	uint8_t d = days_since_epoch - temp + 1;
	_year = y;
	_month = m;
	_day = d;
}

int32_t Date::daysSinceEpoch() {
	int32_t res = _day;
	for (uint8_t m = 1; m < _month; m++)
		res += numDaysInMonth(m, _year);
	res += ((int32_t)365 * (_year - 1));
	res += div(_year - 1, 4).quot;
	res -= div(_year - 1, 100).quot;
	res += div(_year - 1, 400).quot;
	return res;
}

uint8_t Date::dayOfWeek() {
	static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
	uint16_t y = _year;
	if (_month < 3)
		y -= 1;
	return (y + y / 4 - y / 100 + y / 400 + t[_month - 1] + _day) % 7 + 1;
}

bool Date::operator < (Date date) { return daysSinceEpoch() < date.daysSinceEpoch(); }
bool Date::operator <= (Date date) { return daysSinceEpoch() <= date.daysSinceEpoch(); }
bool Date::operator == (Date date) { return daysSinceEpoch() == date.daysSinceEpoch(); }
bool Date::operator >= (Date date) { return daysSinceEpoch() >= date.daysSinceEpoch(); }
bool Date::operator > (Date date) { return daysSinceEpoch() > date.daysSinceEpoch(); }

Date Date::operator + (DaySpan dayspan) {
	return Date(daysSinceEpoch() + dayspan.days());
}

Date Date::operator - (DaySpan dayspan) {
	return Date(daysSinceEpoch() - dayspan.days());
}

static const char * _day_of_week_names[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
static const char * _day_of_week_short_names[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char * _month_names[12] = {"January", "February", "March", "April", "May", "June", 
		"July", "August", "September", "October", "November", "December"};
static const char * _month_short_names[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

const char *Date::monthName() {
	return _month_names[_month - 1];
}

const char *Date::monthShortName() {
	return _month_short_names[_month - 1];
}

const char *Date::dayOfWeekName() {
	return _day_of_week_names[this->dayOfWeek() - 1];
}

const char *Date::dayOfWeekShortName() {
	return _day_of_week_short_names[this->dayOfWeek() - 1];
}
