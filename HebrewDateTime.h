#ifndef HebrewdDateTime_h
#define HebrewDateTime_h
#include "Sun.h"
#include "HebrewDate.h"

class HebrewDateTime {
	public:
		inline HebrewDate date() { return _date; };
		inline uint8_t hour() { return _hour; };
		inline float parts() { return _parts; };
		HebrewDateTime();
		HebrewDateTime(DateTime datetime, ObserverLocation location);
	private:
		HebrewDate _date;
		uint8_t _hour;
		float _parts;
};
#endif
