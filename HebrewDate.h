#ifndef HebrewDate_h
#define HebrewDate_h

#include "Date.h"
#include <stdbool.h>
#include <stdint.h>

enum HebrewHoliday {
//	NO_HOLIDAY=0,
	PESACH=1,
	PESACH_DIASPORA,
	CHOL_HAMOED_PESACH,
	PESACH_7,
	PESACH_7_DIASPORA,
	HOLOCAUST_DAY,
	REMEMBRANCE_DAY,
	INDEPENDENCE_DAY,
	SECOND_PESACH,
	LAG_BAOMER,
	JERUSALEM_DAY,
	SHAVUOT,
	SHAVUOT_DIASPORA,
	FAST_OF_TAMUZ,
	FAST_OF_AV,
	TU_BE_AV,
	ROSH_HASHANA,
	ROSH_HASHANA_2,
	FAST_OF_GEDALIAH,
	KIPPUR,
	SUKKOT,
	SUKKOT_DIASPORA,
	CHOL_HAMOED_SUKKOT,
	HOSHAANA_RABBA,
	SHEMINI_ATZERET_SIMCHAT_TORAH,
	SHEMINI_ATZERET,
	SIMCHAT_TORAH,
	CHANUKKA_1,
	CHANUKKA_2,
	CHANUKKA_3,
	CHANUKKA_4,
	CHANUKKA_5,
	CHANUKKA_6,
	CHANUKKA_7,
	CHANUKKA_8,
	FAST_OF_TEVET,
	TU_BI_SHVAT,
	FAST_OF_ESTHER,
	PURIM,
	SHUSHAN_PURIM,
	PURIM_KATAN,
	SHUSHAN_PURIM_KATAN
};

enum YearType {
	Y2H3,
	Y7H1,
	Y3C5,
	Y5C7,
	Y2S5,
	Y5S1,
	Y7S3,
	Y2H5,
	Y5H1,
	Y7H3,
	Y3C7,
	Y2S7,
	Y5S3,
	Y7S5
};

class HebrewDate {
	public:
		static bool isLeapYear(uint16_t year);
		static uint8_t numMonthsInYear(uint16_t year);
		static uint16_t numDaysInYear(uint16_t year);
		static bool isShortKislevYear(uint16_t year);
		static bool isLongHeshvanYear(uint16_t year);
		static uint8_t numDaysInMonth(uint16_t year, uint8_t month);
		static uint8_t yearType(uint16_t year);
		static const char * yearTypeName(uint16_t year);
		static const char * yearTypeNameEn(uint16_t year);
		HebrewDate();
		HebrewDate(uint16_t year, uint8_t month, uint8_t day);
		HebrewDate(int32_t daysSinceEpoch);
		inline uint8_t day() { return _day; };
		inline uint8_t month() { return _month; };
		inline uint16_t year() { return _year; };
		bool operator < (HebrewDate& d);
		bool operator > (HebrewDate& d);
		HebrewDate operator + (DaySpan dayspan);
		HebrewDate operator - (DaySpan dayspan);
		DaySpan operator - (HebrewDate& d);
		int32_t daysSinceEpoch();
		uint8_t dayOfWeek();
		HebrewHoliday holiday(bool diaspora=false);
		void dayNumber(char * buf);
		void yearNumber(char * buf);
		const char * dayOfWeekName();
		const char * monthName();
		const char * monthNameEn();
		const char * holidayName();
		const char * holidayNameEn();
		uint8_t torahPortion(bool additional=false);
		const char * torahPortionName();
		const char * torahPortionNameEn();
		const char * additionalTorahPortionName();
		const char * additionalTorahPortionNameEn();
	private:
		static uint32_t yearElapsedDays(uint16_t year);
		uint16_t _year;
		uint8_t _month;
		uint8_t _day;
};

void int2heb(uint16_t number, char * buffer, bool gershayim=false);
#endif
