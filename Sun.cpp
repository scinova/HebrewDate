#include <stdio.h>
#include <math.h>
#include <time.h>
#include "Sun.h"

#define PI M_PI
#define RAD PI / 180

// sun calculations are based on http://aa.quae.nl/en/reken/zonpositie.html formulas

#define J1970 2440588
#define J2000 2451545

double toJulian(time_t date) {
	return (double)date / 60 / 60 / 24 - 0.5 + J1970;
}

time_t fromJulian(double j) {
	return (j + 0.5 - J1970) * 60 * 60 * 24;
}

double toDays(time_t date) {
	return toJulian(date) - J2000;
}

// general calculations for position

double E = RAD * 23.4397; // obliquity of the Earth

double rightAscension(double l, double b) {
	return atan2(sin(l) * cos(E) - tan(b) * sin(E), cos(l));
}

double declination(double l, double b) {
	return asin(sin(b) * cos(E) + cos(b) * sin(E) * sin(l));
}

double azimuth(double H, double phi, double dec) {
	return (atan2(sin(H), cos(H) * sin(phi) - tan(dec) * cos(phi)) + M_PI / 2) / M_PI;
}

double altitude(double H, double phi, double dec) {
	return asin(sin(phi) * sin(dec) + cos(phi) * cos(dec) * cos(H)) / M_PI;
}

double siderealTime(double d, double lw) {
	return RAD * (280.16 + 360.9856235 * d) - lw;
}

double astroRefraction(double h) {
	if (h < 0) // the following formula works for positive altitudes only.
		h = 0; // if h = -0.08901179 a div/0 would occur.
	// formula 16.4 of "Astronomical Algorithms" 2nd edition by Jean Meeus (Willmann-Bell, Richmond) 1998.
	// 1.02 / tan(h + 10.26 / (h + 5.10)) h in degrees, result in arc minutes -> converted to rad:
	return 0.0002967 / tan(h + 0.00312536 / (h + 0.08901179));
}

// general sun calculations

double solarMeanAnomaly(double d) {
	return RAD * (357.5291 + 0.98560028 * d);
}

double eclipticLongitude(double M) {
	double C = RAD * (1.9148 * sin(M) + 0.02 * sin(2 * M) + 0.0003 * sin(3 * M)); // equation of center
	double P = RAD * 102.9372; // perihelion of the Earth
	return M + C + P + PI;
}

// calculations for sun times

double J0 = 0.0009;

double julianCycle(double d, double lw) {
	return round(d - J0 - lw / (2 * PI));
}

double approxTransit(double Ht, double lw, double n) {
	return J0 + (Ht + lw) / (2 * PI) + n;
}

double solarTransitJ(double ds, double M, double L) {
	return J2000 + ds + 0.0053 * sin(M) - 0.0069 * sin(2 * L);
}

double hourAngle(double h, double phi, double d) {
	return acos((sin(h) - sin(phi) * sin(d)) / (cos(phi) * cos(d)));
}

double observerAngle(double height) {
	return -2.076 * sqrt(height) / 60;
}

// returns set time for the given sun altitude
double getSetJ(double h, double lw, double phi, double dec, double n, double M, double L) {
	double w = hourAngle(h, phi, dec);
	double a = approxTransit(w, lw, n);
	return solarTransitJ(a, M, L);
}

time_t hoursLater(time_t date, double h) {
	return date + (time_t)(h * 60 * 60);
}

Sun::Sun(ObserverLocation observer) {
	_observer = observer;
}

SunCoordinates Sun::coordinates(DateTime datetime) {
	double d = toDays(datetime.unixtime());
	// calculate coordinates
	double M = solarMeanAnomaly(d);
	double L = eclipticLongitude(M);
	SunCoordinates c;
	c.declination = declination(L, 0);
	c.rightAscension = rightAscension(L, 0);
	return c;
}

SunPosition Sun::position(DateTime datetime) {
	double d = toDays(datetime.unixtime());
	SunCoordinates c = coordinates(datetime);
	// calculate position
	double lw  = RAD * -_observer.longitude;
	double phi = RAD * _observer.latitude;
	double H = siderealTime(d, lw) - c.rightAscension;
	SunPosition p;
	p.azimuth = azimuth(H, phi, c.declination);
	p.altitude = altitude(H, phi, c.declination);
	return p;
}

DateTime Sun::sunrise(Date date) {
	DateTime dt = DateTime(date, Time(0, 0, 0));
	double d = toDays(dt.unixtime());
	double lw  = RAD * - _observer.longitude;
	double phi = RAD * _observer.latitude;
	double n = julianCycle(d, lw);
	double ds = approxTransit(0, lw, n);
	double M = solarMeanAnomaly(ds);
	double L = eclipticLongitude(M);
	double dec = declination(L, 0);
	double Jnoon = solarTransitJ(ds, M, L);
	//solarNoon = fromJulian(Jnoon);
	//nadir = fromJulian(Jnoon - 0.5);
	double angle = -0.833;
	double height = 0;
	double dh = observerAngle(height);
	double h0 = (angle + dh) * RAD;
	double Jset = getSetJ(h0, lw, phi, dec, n, M, L);
	double Jrise = Jnoon - (Jset - Jnoon);
	return DateTime(fromJulian(Jrise));
	//sunset = fromJulian(Jset);
}

DateTime Sun::sunset(Date date) {
	DateTime dt = DateTime(date, Time(0, 0, 0));
	double d = toDays(dt.unixtime());
	double lw  = RAD * - _observer.longitude;
	double phi = RAD * _observer.latitude;
	double n = julianCycle(d, lw);
	double ds = approxTransit(0, lw, n);
	double M = solarMeanAnomaly(ds);
	double L = eclipticLongitude(M);
	double dec = declination(L, 0);
	//double Jnoon = solarTransitJ(ds, M, L);
	//solarNoon = fromJulian(Jnoon);
	//nadir = fromJulian(Jnoon - 0.5);
	double angle = -0.833;
	double height = 0;
	double dh = observerAngle(height);
	double h0 = (angle + dh) * RAD;
	double Jset = getSetJ(h0, lw, phi, dec, n, M, L);
	//double Jrise = Jnoon - (Jset - Jnoon);
	return DateTime(fromJulian(Jset));
	//sunset = fromJulian(Jset);
}

Moon::Moon(ObserverLocation observer) {
	_observer = observer;
}

MoonCoordinates Moon::coordinates(DateTime datetime) { // geocentric ecliptic coordinates of the moon
	double d = toDays(datetime.unixtime());
	double L = RAD * (218.316 + 13.176396 * d); // ecliptic longitude
	double M = RAD * (134.963 + 13.064993 * d); // mean anomaly
	double F = RAD * (93.272 + 13.229350 * d);  // mean distance
	double l  = L + RAD * 6.289 * sin(M); // longitude
	double b  = RAD * 5.128 * sin(F);     // latitude
	MoonCoordinates c;
	c.distance = 385001 - 20905 * cos(M);  // distance to the moon in km
	c.rightAscension = rightAscension(l, b);
	c.declination = declination(l, b);
	return c;
}

MoonPosition Moon::position(DateTime datetime) {
	double lw  = RAD * -_observer.longitude;
	double phi = RAD * _observer.latitude;
	double d = toDays(datetime.unixtime());
	MoonCoordinates c = coordinates(datetime);
	double H = siderealTime(d, lw) - c.rightAscension;
	double h = altitude(H, phi, c.declination);
	// formula 14.1 of "Astronomical Algorithms" 2nd edition by Jean Meeus (Willmann-Bell, Richmond) 1998.
	h = h + astroRefraction(h); // altitude correction for refraction
	MoonPosition p;
	p.parallacticAngle = atan2(sin(H), tan(phi) * cos(c.declination) - sin(c.declination) * cos(H));
	p.azimuth = azimuth(H, phi, c.declination);
	p.altitude = h;
	p.distance = c.distance;
	return p;
}

// calculations for illumination parameters of the moon,
// based on http://idlastro.gsfc.nasa.gov/ftp/pro/astro/mphase.pro formulas and
// Chapter 48 of "Astronomical Algorithms" 2nd edition by Jean Meeus (Willmann-Bell, Richmond) 1998.
MoonIllumination Moon::illumination(DateTime datetime) {
	//double d = toDays(datetime.unixtime());
	SunCoordinates s = Sun(_observer).coordinates(datetime);
	MoonCoordinates m = coordinates(datetime);
	double sdist = 149598000; // distance from Earth to Sun in km
	double phi = acos(sin(s.declination) * sin(m.declination) + cos(s.declination) * cos(m.declination) * cos(s.rightAscension - m.rightAscension));
	double inc = atan2(sdist * sin(phi), m.distance - sdist * cos(phi));
	MoonIllumination i;
	i.fraction = (1 + cos(inc)) / 2;
	i.angle = atan2(cos(s.declination) * sin(s.rightAscension - m.rightAscension), sin(s.declination) * cos(m.declination) -
			cos(s.declination) * sin(m.declination) * cos(s.rightAscension - m.rightAscension));
	i.phase = 0.5 + 0.5 * inc * (i.angle < 0 ? -1 : 1) / M_PI;
	return i;
}

DateTime Moon::moonrise(DateTime dt) {
	//DateTime dt = mlocaltime(DateTime(date, Time(0, 0, 0)));
	double hc = 0.133 * RAD;
	double h0 = position(dt).altitude - hc;
	double h1, h2, rise=0, set=0, a, b, xe, ye, d, roots, x1, x2, dx;
	// go in 2-hour chunks, each time seeing if a 3-point quadratic curve crosses zero (which means rise or set)
	for (int i = 1; i <= 24; i += 2) {
		h1 = position(dt + HourSpan(i)).altitude - hc;
		h2 = position(dt + HourSpan(i + 1)).altitude - hc;
//		h1 = position(DateTime(hoursLater(dt.unixtime(), i))).altitude - hc;
//		h2 = position(DateTime(hoursLater(dt.unixtime(), i + 1))).altitude - hc;
		a = (h0 + h2) / 2 - h1;
		b = (h2 - h0) / 2;
		xe = -b / (2 * a);
		ye = (a * xe + b) * xe + h1;
		d = b * b - 4 * a * h1;
		roots = 0;
		if (d >= 0) {
			dx = sqrt(d) / (abs(a) * 2);
			x1 = xe - dx;
			x2 = xe + dx;
			if (abs(x1) <= 1) roots++;
			if (abs(x2) <= 1) roots++;
			if (x1 < -1) x1 = x2;
		}
		if (roots == 1) {
			if (h0 < 0) rise = i + x1;
			else set = i + x1;
		} else if (roots == 2) {
			rise = i + (ye < 0 ? x2 : x1);
			set = i + (ye < 0 ? x1 : x2);
		}
		if (rise && set)
			break;
		h0 = h2;
	}
	//if (rise)
	return DateTime(hoursLater(dt.unixtime(), rise));
	//else
	//return NULL;
	//if (rise) result.rise = hoursLater(t, rise);
	//if (set) result.set = hoursLater(t, set);
	//if (!rise && !set) result[ye > 0 ? 'alwaysUp' : 'alwaysDown'] = true;
}

DateTime Moon::moonset(DateTime dt) {
	//DateTime dt = mlocaltime(DateTime(date, Time(0, 0, 0)));
	double hc = 0.133 * RAD;
	double h0 = position(dt).altitude - hc;
	double h1, h2, rise=0, set=0, a, b, xe, ye, d, roots, x1, x2, dx;
	// go in 2-hour chunks, each time seeing if a 3-point quadratic curve crosses zero (which means rise or set)
	for (int i = 1; i <= 24; i += 2) {
		h1 = position(dt + HourSpan(i)).altitude - hc;
		h2 = position(dt + HourSpan(i + 1)).altitude - hc;
//		h1 = position(DateTime(hoursLater(dt.unixtime(), i))).altitude - hc;
//		h2 = position(DateTime(hoursLater(dt.unixtime(), i + 1))).altitude - hc;
		a = (h0 + h2) / 2 - h1;
		b = (h2 - h0) / 2;
		xe = -b / (2 * a);
		ye = (a * xe + b) * xe + h1;
		d = b * b - 4 * a * h1;
		roots = 0;
		if (d >= 0) {
			dx = sqrt(d) / (abs(a) * 2);
			x1 = xe - dx;
			x2 = xe + dx;
			if (abs(x1) <= 1) roots++;
			if (abs(x2) <= 1) roots++;
			if (x1 < -1) x1 = x2;
		}
		if (roots == 1) {
			if (h0 < 0) rise = i + x1;
			else set = i + x1;
		} else if (roots == 2) {
			rise = i + (ye < 0 ? x2 : x1);
			set = i + (ye < 0 ? x1 : x2);
		}
		if (rise && set)
			break;
		h0 = h2;
	}
	//if (set)
	return DateTime(hoursLater(dt.unixtime(), set));
	//else
	//	return NULL;
}
