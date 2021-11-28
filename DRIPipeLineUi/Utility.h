// Utility.h
#include "StdAfx.h"

#pragma once

#define PI 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899
#define D360 PI * 2
#define D180 PI
#define D90 D180 / 2
#define D270 D90 * 3
#define D1 PI / 180

#define DTR_RATIO PI / 180.0
#define RTD_RATIO 180.0 / PI
#define DTR(d) d * DTR_RATIO
#define RTD(d) d * RTD_RATIO

namespace Schedule
{
	uint16_t resolveDn(AcString layerName);
}