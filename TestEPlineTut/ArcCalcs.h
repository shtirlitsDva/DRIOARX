// ArcCalcs.h
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

#pragma once

double CalcArcEndTangent(AcGeCircArc2d & arc);
double CalcBulgeFactor(double dTangentAngle, AcGePoint3d & fromPt, AcGePoint3d & ptArcEnd);
double CalcBulgeFactor(double dDeflectionAngle, double dTangentAngle, double dChordAngle);