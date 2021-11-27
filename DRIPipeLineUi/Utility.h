// Utility.h
#include "StdAfx.h"
#include <unordered_map>
#include "meta_enum.hpp"

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
	meta_enum_class(
		PipeDnSizes,
		uint16_t,
		Invalid = 999,
		DN32 = 32,
		DN40 = 40,
		DN50 = 50,
		DN65 = 65,
		DN80 = 80,
		DN100 = 100,
		DN125 = 125,
		DN150 = 150,
		DN200 = 200,
		DN250 = 250,
		DN300 = 300,
		DN350 = 350,
		DN400 = 400,
		DN450 = 450,
		DN500 = 500,
		DN600 = 600
	);
}