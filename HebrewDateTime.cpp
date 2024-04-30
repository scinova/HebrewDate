#include "HebrewDateTime.h"
#include "Sun.h"

HebrewDateTime::HebrewDateTime() {
}

HebrewDateTime::HebrewDateTime(DateTime datetime, ObserverLocation location) {
	_date = HebrewDate(datetime.date().daysSinceEpoch() + 1);
	Sun sun = Sun(location);
	Date today = datetime.date();
	Time now = datetime.time();
	Date yesterday = today - DaySpan(1);
	Date tomorrow = today + DaySpan(1);

	Time yesterdaySunset = sun.sunset(yesterday).time() + HourSpan(2);
	Time todaySunrise = sun.sunrise(today).time() + HourSpan(2);
	Time todaySunset = sun.sunset(today).time() + HourSpan(2);
	Time tomorrowSunrise = sun.sunrise(tomorrow).time() + HourSpan(2);

	uint32_t daylen = 0, nightlen = 0;
	uint32_t secs;
	double hour;
	if (now > todaySunset) {
		// sunset to midnight
		nightlen = tomorrowSunrise.secondsSinceMidnight() + 86400 - todaySunset.secondsSinceMidnight();
		secs = now.secondsSinceMidnight() - todaySunset.secondsSinceMidnight();
		hour = (double)secs / nightlen * 12;
		_hour = int(hour);
		_date = _date + DaySpan(1);
	} else if (now < todaySunrise) {
		// midnight to sunrise
		nightlen = 86400 - yesterdaySunset.secondsSinceMidnight() + todaySunrise.secondsSinceMidnight();
		secs = 86400 - yesterdaySunset.secondsSinceMidnight() + now.secondsSinceMidnight();
		hour = (double)secs / nightlen * 12;
		_hour = int(hour);
	} else {
		// sunrise to sunset
		daylen = todaySunset.secondsSinceMidnight() - todaySunrise.secondsSinceMidnight();
		secs = now.secondsSinceMidnight() - todaySunrise.secondsSinceMidnight();;
		hour = (double)secs / daylen * 12;
		_hour = 12 + int(hour);
	}
	_parts = hour - int(hour);
	}
