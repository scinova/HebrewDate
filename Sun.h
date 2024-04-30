#ifndef Sun_h
#define Sun_h
#include "DateTime.h"
#include <time.h>

typedef struct {
	double latitude;
	double longitude;
} ObserverLocation;

typedef struct {
	double azimuth;
	double altitude;
} SunPosition;

typedef struct {
	double declination;
	double rightAscension;
} SunCoordinates;

class Sun {
	public:
		Sun(ObserverLocation observer);
		SunCoordinates coordinates(DateTime datetime);
		SunPosition position(DateTime datetime);
		DateTime sunrise(Date date);
		DateTime sunset(Date date);
	private:
		ObserverLocation _observer;
};

typedef struct {
	double rightAscension;
	double declination;
	double distance;
} MoonCoordinates;

typedef struct {
	double azimuth;
	double altitude;
	double distance;
	double parallacticAngle;
} MoonPosition;

typedef struct {
	double fraction;
	double phase;
	double angle;
} MoonIllumination;

class Moon {
	public:
		Moon(ObserverLocation observer);
		MoonCoordinates coordinates(DateTime datetime);
		MoonPosition position(DateTime datetime);
		MoonIllumination illumination(DateTime datetime);
		DateTime moonrise(DateTime datetime);
		DateTime moonset(DateTime datetime);
	private:
		ObserverLocation _observer;
};
#endif
