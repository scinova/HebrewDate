#include "HebrewDate.h"
#include "Date.h"
#include <stdint.h>
#include <stdbool.h>
#include "torah.h"
#include <stdlib.h>

bool HebrewDate::isLeapYear(uint16_t year) {
	if ((((year * 7) + 1) % 19) < 7)
		return true;
	else
		return false;
}

uint8_t HebrewDate::numMonthsInYear(uint16_t year) {
	if (isLeapYear(year))
		return 13;
	else
		return 12;
}

uint32_t HebrewDate::yearElapsedDays(uint16_t year) {
	uint32_t months_elapsed = (uint32_t)235 * ldiv(year - 1, 19).quot;
	months_elapsed += 12 * ((year - 1) % 19);
	months_elapsed += ldiv(((((year - 1) % 19) * 7) + 1), 19).quot;
	uint32_t parts_elapsed = ((months_elapsed % 1080) * 793) + 204;
	uint32_t hours_elapsed = 5 + (months_elapsed * 12) + \
			(ldiv(months_elapsed, 1080).quot * 793) + ldiv(parts_elapsed, 1080).quot;
	uint32_t day = 1 + (29 * months_elapsed) + ldiv(hours_elapsed, 24).quot;
	uint32_t parts = ((hours_elapsed % 24) * 1080) + (parts_elapsed % 1080);
	uint32_t alt_day;
	if (parts >= 19440 || \
			(day % 7 == 2 && parts >= 9924 && !isLeapYear(year)) || \
			(day % 7 == 1 && parts >= 16789 && isLeapYear(year - 1)))
		alt_day = day + 1;
	else
		alt_day = day;
	if (alt_day % 7 == 0 || alt_day % 7 == 3 || alt_day % 7 == 5)
		alt_day += 1;
	return alt_day;
}

uint16_t HebrewDate::numDaysInYear(uint16_t year) {
	return (yearElapsedDays(year + 1) - yearElapsedDays(year));
}

bool HebrewDate::isLongHeshvanYear(uint16_t year) {
	if ((numDaysInYear(year) % 10) == 5)
		return true;
	else
		return false;
}

bool HebrewDate::isShortKislevYear(uint16_t year) {
	if ((numDaysInYear(year) % 10) == 3)
		return true;
	else
		return false;
}

uint8_t HebrewDate::numDaysInMonth(uint16_t year, uint8_t month) {
	if (month == 2 || month == 4 || month == 6 || month == 10 || month == 13)
		return 29;
	if (month == 12 && !isLeapYear(year))
		return 29;
	if (month == 8 && !isLongHeshvanYear(year))
		return 29;
	if (month == 9 && isShortKislevYear(year))
		return 29;
	return 30;
}

HebrewDate::HebrewDate() {
}

HebrewDate::HebrewDate(uint16_t year, uint8_t month, uint8_t day) {
	_year = year;
	_month = month;
	_day = day;
}

bool HebrewDate::operator < (HebrewDate& d) {
	return daysSinceEpoch() < d.daysSinceEpoch();
}

bool HebrewDate::operator > (HebrewDate& d) {
	return daysSinceEpoch() > d.daysSinceEpoch();
}

HebrewDate HebrewDate::operator + (DaySpan span) {
	return HebrewDate(daysSinceEpoch() + span.days());
}

HebrewDate HebrewDate::operator - (DaySpan span) {
	return HebrewDate(daysSinceEpoch() - span.days());
}

DaySpan HebrewDate::operator - (HebrewDate& d) {
	return DaySpan(daysSinceEpoch() - d.daysSinceEpoch());
}

void HebrewDate::yearNumber(char * buf) {
	int2heb(_year, buf, true);
}

void HebrewDate::dayNumber(char * buf) {
	int2heb(_day, buf, true);
}

int32_t HebrewDate::daysSinceEpoch() {
	int32_t value;
	int32_t res = _day;
	if (_month < 7) {
		for (int m = 7; m < numMonthsInYear(_year) + 1; m++) {
			value = numDaysInMonth(_year, m);
			res += value;
		}
		for (int m = 1; m < _month; m++) {
			value = numDaysInMonth(_year, m);
			res += value;
		}
	} else {
		for (int m = 7; m < _month; m++) {
			value = numDaysInMonth(_year, m);
			res += value;
		}
	}
	res += yearElapsedDays(_year);
	res -= 1373429;
	return res;
}

HebrewDate::HebrewDate(int32_t days) {
	int32_t absdate = days;
	uint8_t month, day;
	int16_t year = ldiv(absdate + 1373429, 366).quot;
	while (1) {
		if (absdate < HebrewDate(year + 1, 7, 1).daysSinceEpoch())
			break;
		year += 1;
	}
	if (absdate < HebrewDate(year, 1, 1).daysSinceEpoch())
		month = 7;
	else
		month = 1;
	while (1) {
		if (absdate <= HebrewDate(year, month, HebrewDate::numDaysInMonth(year, month)).daysSinceEpoch())
			break;
		month += 1;
	}
	day = (absdate - HebrewDate(year, month, 1).daysSinceEpoch() + 1);
	_year = year;
	_month = month;
	_day = day;
}

static const char * _month_names[] = {"ניסן", "אייר", "סיוון", "תמוז", "אב", "אלול",
		"תשרי", "חשוון", "כסלו", "טבת", "שבט", "אדר", "אדר א", "אדר ב"};
static const char * _month_names_en[] = {"Nisan", "Iyar", "Sivan", "Tammuz", "Av", "Elul",
		"Tishrei", "Cheshvan", "Kislev", "Tevet", "Shevat", "Adar", "Adar I", "Adar II"};

const char * HebrewDate::monthName() {
	return _month_names[(_month > 11 && isLeapYear(_year) ? _month + 1 : _month) - 1];
}

const char * HebrewDate::monthNameEn() {
	return _month_names_en[(_month > 11 && isLeapYear(_year) ? _month + 1 : _month) - 1];
}

uint8_t HebrewDate::dayOfWeek() {
	return (uint8_t)((daysSinceEpoch() % 7) + 1);
}

static const char * _day_of_week_hebrew_names[] = {"ראשון", "שני", "שלישי", "רביעי", "חמישי", "שישי", "שבת"};

const char *HebrewDate::dayOfWeekName() {
	return _day_of_week_hebrew_names[dayOfWeek() - 1];
}

HebrewHoliday HebrewDate::holiday(bool diaspora) {
	HebrewHoliday holiday;
	// Nisan
	if (_month == 1) {
		if (_day == 15)
			holiday = PESACH;
		if (_day == 16) {
			if (diaspora)
				holiday = PESACH_DIASPORA;
			else
				holiday = CHOL_HAMOED_PESACH;
		}
		if (_day == 17 || _day == 18 || _day == 19 || _day == 20)
			holiday = CHOL_HAMOED_PESACH;
		if (_day == 21)
			holiday = PESACH_7;
		if (_day == 22 && diaspora)
			holiday = PESACH_7_DIASPORA;

		HebrewDate holocaustDay = HebrewDate(_year, 1, 27);
		if (holocaustDay.dayOfWeek() == 6) {
			if (_day == 26)
				holiday = HOLOCAUST_DAY;
		} else if (_year >= 5757 && holocaustDay.dayOfWeek() == 1) {
			if (_day == 28)
				holiday = HOLOCAUST_DAY;
		} else if (_day == 27)
			holiday = HOLOCAUST_DAY;
	}

	// Iyar
	if (_month == 2) {
		// Memorial Day
		// if on Friday, two days before
		// if on Thursday, one day before
		// after 2004, if on Sunday, one day later
		HebrewDate memorialDay = HebrewDate(_year, 2, 4);
		if (memorialDay.dayOfWeek() == 6) {
			if (_day == 2)
				holiday = REMEMBRANCE_DAY;
		} else if (memorialDay.dayOfWeek() == 5) {
			if (_day == 3)
				holiday = REMEMBRANCE_DAY;
		} else if (_year >= 5764 && memorialDay.dayOfWeek() == 1) {
			if (_day == 5)
				holiday = REMEMBRANCE_DAY;
		} else if (_day == 4)
			holiday = REMEMBRANCE_DAY;

		// Independence Day
		// if on Shabat, two days before
		// if on Friday, one day before
		// after 2004, if on Monday, one day later
		HebrewDate independenceDay = HebrewDate(_year, 2, 5);
		if (independenceDay.dayOfWeek() == 7) {
			if (_day == 3)
				holiday = INDEPENDENCE_DAY;
		} else if (independenceDay.dayOfWeek() == 6) {
			if (_day == 4)
				holiday = INDEPENDENCE_DAY;
		} else if (_year >= 5764 && independenceDay.dayOfWeek() == 2) {
			if (_day == 6)
				holiday = INDEPENDENCE_DAY;
		} else if (_day == 5)
			holiday = INDEPENDENCE_DAY;

		if (_day == 14)
			holiday = SECOND_PESACH;
		if (_day == 18)
			holiday = LAG_BAOMER;
		if (_day == 28)
			holiday = JERUSALEM_DAY;
	}
	// Sivan
	if (_month == 3) {
		if (_day == 6)
			holiday = SHAVUOT;
		if (_day == 7 && diaspora)
			holiday = SHAVUOT_DIASPORA;
	}
	// Tammuz
	if (_month == 4) {
		HebrewDate fastDay = HebrewDate(_year, 4, 17);
		if (fastDay.dayOfWeek() == 7) {
			if (_day == 18)
				holiday = FAST_OF_TAMUZ;
		} else if (_day == 17)
			holiday = FAST_OF_TAMUZ;
	}
	// Av
	if (_month == 5) {
		HebrewDate fastDay = HebrewDate(_year, 5, 9);
		if (fastDay.dayOfWeek() == 7) {
			if (_day == 10)
				holiday = FAST_OF_AV;
		} else if (_day == 9)
			holiday = FAST_OF_AV;
		if (_day == 15)
			holiday = TU_BE_AV;
	}
	// Tishrei
	if (_month == 7) {
		if (_day == 1)
			holiday = ROSH_HASHANA;
		if (_day == 2)
			holiday = ROSH_HASHANA_2;
		HebrewDate fastOfGedaliah = HebrewDate(_year, 7, 3);
		if (fastOfGedaliah.dayOfWeek() == 7) {
			if (_day == 4)
				holiday = FAST_OF_GEDALIAH;
		} else if (_day == 3)
			holiday = FAST_OF_GEDALIAH;
		if (_day == 10)
			holiday = KIPPUR;
		if (_day == 15)
			holiday = SUKKOT;
		if (_day == 16) {
			if (diaspora)
				holiday = SUKKOT_DIASPORA;
			else
				holiday = CHOL_HAMOED_SUKKOT;
		}
		if (_day == 17 || _day == 18 || _day == 19 || _day == 20)
			holiday = CHOL_HAMOED_SUKKOT;
		if (_day == 21)
			holiday = HOSHAANA_RABBA;
		if (_day == 22) {
			if (!diaspora)
				holiday = SHEMINI_ATZERET_SIMCHAT_TORAH;
			else
				holiday = SHEMINI_ATZERET;
		}
		if (_day == 23 && diaspora)
			holiday = SIMCHAT_TORAH;
	}
	// Kislev
	if (_month == 9) {
		if (_day == 25)
			holiday = CHANUKKA_1;
		if (_day == 26)
			holiday = CHANUKKA_2;
		if (_day == 27)
			holiday = CHANUKKA_3;
		if (_day == 28)
			holiday = CHANUKKA_4;
		if (_day == 29)
			holiday = CHANUKKA_5;
		if (numDaysInMonth(_year, 9) == 30 && _day == 30)
			holiday = CHANUKKA_6;
	}
	// Tevet
	if (_month == 10) {
		if (numDaysInMonth(_year, 9) == 30) {
			if (_day == 1)
				holiday = CHANUKKA_7;
			if (_day == 2)
				holiday = CHANUKKA_8;
		} else {
			if (_day == 1)
				holiday = CHANUKKA_6;
			if (_day == 2)
				holiday = CHANUKKA_7;
			if (_day == 3)
				holiday = CHANUKKA_8;
		}
		if (_day == 10)
			holiday = FAST_OF_TEVET;
	}
	// Shevat
	if (_month == 11) {
		if (_day == 15)
			holiday = TU_BI_SHVAT;
	}
	// Adar I/Adar II
	int monthEsther;
	if (isLeapYear(_year))
		monthEsther = 13;
	else
		monthEsther = 12;
	if (_month == monthEsther) {
		HebrewDate fastOfEsther = HebrewDate(_year, monthEsther, 13);
		if (fastOfEsther.dayOfWeek() == 7) {
			if (_day == 11)
				holiday = FAST_OF_ESTHER;
		} else {
			if (_day == 13)
				holiday = FAST_OF_ESTHER;
		}
		if (_day == 14)
			holiday = PURIM;
		if (_day == 15)
			holiday = SHUSHAN_PURIM;
	}
	if (isLeapYear(_year) && _month == 12) {
		if (_day == 14)
			holiday = PURIM_KATAN;
		if (_day == 15)
			holiday = SHUSHAN_PURIM_KATAN;
	}
	return holiday;
}

static const char *_holiday_names_en[] {
	"",
	"Pesach",
	"Pesach (diaspora)",
	"Chol Hamoed Pesach",
	"Pesach 7",
	"Pesach 7 (diaspora)",
	"Holocaust Day",
	"Remembrance Day",
	"Independence Day",
	"Second Pesach",
	"Lag Baomer",
	"Jerusalem Day",
	"Shavuot",
	"Shavuot (diaspora)",
	"Fast of Tamuz",
	"Fast of Av",
	"Tu beAv",
	"Rosh haShana",
	"Rosh haShana 2",
	"Fast of Gedaliah",
	"Kippur",
	"Sukkot",
	"Sukkot Diaspora",
	"Chol haMoed Sukkot",
	"Hoshaana Rabba",
	"Shemini Atzeret Simchat Torah",
	"Shemini Atzeret",
	"Simchat Torah",
	"Chanukka 1",
	"Chanukka 2",
	"Chanukka 3",
	"Chanukka 4",
	"Chanukka 5",
	"Chanukka 6",
	"Chanukka 7",
	"Chanukka 8",
	"Fast of Tevet",
	"Tu BiShvat",
	"Fast of Esther",
	"Purim",
	"Shushan Purim",
	"Purim Katan",
	"Shushan Purim Katan"
};

static const char *_holiday_names[] {
	"",
	"פסח א",
	"פסח ב",
	"חול המועד פסח",
	"פסח ז",
	"פסח ח",
	"יום השואה",
	"יום הזכרון",
	"יום העצמאות",
	"פסח שני",
	"לג בעומר",
	"יום רושלים",
	"שבועות",
	"שבועוץ ב",
	"יז בתמוז",
	"ט באב",
	"טו באב",
	"ראש השנה א",
	"ראש השנה ב",
	"צום גדליה",
	"כיפור",
	"סוכות",
	"סוכות ב",
	"חול המועד סוכות",
	"הושענה רבא",
	"שמיני עצרת שמחת תורה",
	"שמיני עצרת",
	"שמחת תורה",
	"חנוכה א",
	"חנוכה ב",
	"חנוכה ג",
	"חנוכה ד",
	"חנוכה ה",
	"חנוכה ו",
	"חנוכה ז",
	"חנוכה ח",
	"י בטבת",
	"טו בשבט",
	"תענית אסתר",
	"פורים",
	"שושן פורים",
	"פורים קטן",
	"שושן פורים קטן"
};

const char *HebrewDate::holidayName() {
	return _holiday_names[holiday()];
}

const char *HebrewDate::holidayNameEn() {
	return _holiday_names_en[holiday()];
}

uint8_t HebrewDate::yearType(uint16_t year) {
	uint8_t new_year_day_of_week = HebrewDate(year, 7, 1).dayOfWeek();
	uint16_t yearLength = (new HebrewDate(year + 1, 7, 1))->daysSinceEpoch() - (new HebrewDate(year, 7, 1))->daysSinceEpoch();
	uint8_t pesachWeekday = (new HebrewDate(year, 1, 15))->dayOfWeek();
	uint8_t _types[14][4] = {
		2, 53, 3, Y2H3, //בחג
		7, 53, 1, Y7H1,
		3, 54, 5, Y3C5,
		5, 54, 7, Y5C7,
		2, 55, 5, Y2S5, //בשה
		5, 55, 1, Y5S1,
		7, 55, 3, Y7S3,
		2, 83, 5, Y2H5, //בחה
		5, 83, 1, Y5H1,
		7, 83, 3, Y7H3,
		3, 84, 7, Y3C7,
		2, 85, 7, Y2S7, //בשז
		5, 85, 3, Y5S3,
		7, 85, 5, Y7S5};
	uint8_t res;
	for (int t = 0; t < 14; t++) {
		if (new_year_day_of_week == _types[t][0] &&
				yearLength - 300 == _types[t][1] &&
				pesachWeekday == _types[t][2])
			return _types[t][3];
	}
	return _types[0][3]; // should not be here
}

static const char * _type_names[] = {
	"בחג", "זחא", "גכה", "הכז", "בשה", "זשא", "זשג",
	"בחה", "החא", "זחג", "גכז", "בשז", "השג", "זשה"
};

static const char * _type_names_en[] = {
	"BHG", "ZHA", "GCH", "HCZ", "BSH", "ZSA", "ZSG",
	"BHH", "HHA", "ZHG", "GCZ", "BSZ", "HSG", "ZSH"
};

const char * HebrewDate::yearTypeName(uint16_t year) {
	return _type_names[yearType(year)];
}

const char * HebrewDate::yearTypeNameEn(uint16_t year) {
	return _type_names_en[yearType(year)];
}

HebrewDate getBereshitDate(uint16_t year) {
	HebrewDate simchatTorah = HebrewDate(year, 7, 23);
	while (simchatTorah.dayOfWeek() != 7)
		simchatTorah = simchatTorah + DaySpan(1);
	return simchatTorah;
}

uint8_t HebrewDate::torahPortion(bool additional) {
	HebrewDate bereshithDate = getBereshitDate(_year);
	HebrewDate torahDate = HebrewDate(_year, _month, _day);
	while (torahDate.dayOfWeek() != 7)
		torahDate = torahDate + DaySpan(1);
	uint16_t referenceYear;
	if (torahDate < bereshithDate)
		referenceYear = _year - 1;
	else
		referenceYear = _year;
	bereshithDate = getBereshitDate(referenceYear);
	uint8_t weekno = (torahDate - bereshithDate).days() / 7;
	uint8_t yeartype = yearType(referenceYear);
	return _torah_portions[yeartype][weekno][(additional ? 1 : 0)];
}

const char * HebrewDate::torahPortionNameEn() {
	return _torah_portion_names_en[torahPortion()];
}

const char * HebrewDate::torahPortionName() {
	return _torah_portion_names[torahPortion()];
}

const char * HebrewDate::additionalTorahPortionNameEn() {
	return _torah_portion_names_en[torahPortion(true)];
}

const char * HebrewDate::additionalTorahPortionName() {
	return _torah_portion_names[torahPortion(true)];
}

void int2heb(uint16_t number, char * buffer, bool gershayim) {
	int offset = 0;
	uint16_t n = number;
	while (n > 0) {
		if (n > 1000) {
			int thousands = div(n, 1000).quot;
			n -= thousands * 1000;
			buffer[offset++] = 0xd7;
			buffer[offset++] = 0x90 + thousands - 1;
			if (gershayim) {
				buffer[offset++] = 0xd7;
				buffer[offset++] = 0xb3;
			}
		} else if (n >= 400) {
			buffer[offset++] = 0xd7;
			buffer[offset++] = 0xaa;
			n -= 400;
		} else if (n >= 300) { // shin
			buffer[offset++] = 0xd7;
			buffer[offset++] = 0xa9;
			n -= 300;
		} else if (n >= 200) {
			buffer[offset++] = 0xd7;
			buffer[offset++] = 0xa8;
			n -= 200;
		} else if (n >= 100) {
			buffer[offset++] = 0xd7;
			buffer[offset++] = 0xa7;
			n -= 100;
		} else if (n >= 90) {
			buffer[offset++] = 0xd7;
			buffer[offset++] = 0xa6;
			n -= 90;
		} else if (n >= 80) { // pe
			buffer[offset++] = 0xd7;
			buffer[offset++] = 0xa4;
			n -= 80;
		} else if (n >= 70) { // ain
			buffer[offset++] = 0xd7;
			buffer[offset++] = 0xa2;
			n -= 70;
		} else if (n >= 60) { // samech
			buffer[offset++] = 0xd7;
			buffer[offset++] = 0xa1;
			n -= 60;
		} else if (n >= 50) { // nun
			buffer[offset++] = 0xd7;
			buffer[offset++] = 0xa0;
			n -= 50;
		} else if (n >= 40) { // mem
			buffer[offset++] = 0xd7;
			buffer[offset++] = 0x9e;
			n -= 40;
		} else if (n >= 30) { // ל
			buffer[offset++] = 0xd7;
			buffer[offset++] = 0x9c;
			n -= 30;
		} else if (n >= 20) { // כ
			buffer[offset++] = 0xd7;
			buffer[offset++] = 0x9b;
			n -= 20;
		} else if (n >= 10) {
			if (n == 16) {
				buffer[offset++] = 0xd7;
				buffer[offset++] = 0x98;
				if (gershayim) {
					buffer[offset++] = 0xd7;
					buffer[offset++] = 0xb4;
				}
				buffer[offset++] = 0xd7;
				buffer[offset++] = 0x96;
				n -= 16;
			} else if (n == 15) {
				buffer[offset++] = 0xd7;
				buffer[offset++] = 0x98;
				if (gershayim) {
					buffer[offset++] = 0xd7;
					buffer[offset++] = 0xb4;
				}
				buffer[offset++] = 0xd7;
				buffer[offset++] = 0x95;
				n -= 15;
			} else {
				buffer[offset++] = 0xd7;
				buffer[offset++] = 0x99;
				n -= 10;
			}
		} else {
			if (gershayim && number > 10) {
				buffer[offset++] = 0xd7;
				buffer[offset++] = 0xb4;
			}
			buffer[offset++] = 0xd7;
			buffer[offset++] = 0x90 + n - 1;
			n = 0;
		}
		if (n == 0 && gershayim && offset == 2) {
			buffer[offset++] = 0xd7;
			buffer[offset++] = 0xb3;
		}
	}
	buffer[offset] = 0;
}
