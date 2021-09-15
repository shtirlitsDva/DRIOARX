// Utility.h
//
// (C) Copyright 2009 by Paul Kohut, paulkohut@hotmail.com
//
// This file is part of EPlineTut and EPlineTutDb.
// 
// EPlineTut and EPlineTutDb is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
// 
// EPlineTut and EPlineTutDb is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with EPlineTut.  If not, see <http://www.gnu.org/licenses/>.
#include "StdAfx.h"
#include <unordered_map>

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