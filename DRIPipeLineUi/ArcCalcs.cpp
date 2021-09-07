// ArcCalcs.cpp
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
#include "ArcCalcs.h"
#include "Utility.h"

double CalcArcEndTangent(AcGeCircArc2d & arc)
{
	double dAngle;
	dAngle = acutAngle(asDblArray(arc.center()), asDblArray(arc.endPoint()));
	if(arc.isClockWise())
		dAngle -= D90;
	else
		dAngle += D90;
	return dAngle;
}

double CalcBulgeFactor(double dTangentAngle, AcGePoint3d & fromPt, AcGePoint3d & ptArcEnd)
{
	AcGeLineSeg2d chord(asPnt2d(asDblArray(fromPt)), asPnt2d(asDblArray(ptArcEnd)));
	double dChordAngle = chord.direction().angle();
	if(dTangentAngle == dChordAngle)
		return 0.0;

	AcGeVector2d vec(fromPt.x, fromPt.y);
	AcGeLineSeg2d ln;
	AcGePoint3d lnStartPt;
	acutPolar(asDblArray(fromPt), dTangentAngle - D180, 10.0, asDblArray(lnStartPt));
	ln.set(asPnt2d(asDblArray(lnStartPt)), asPnt2d(asDblArray(fromPt)));
	return CalcBulgeFactor(ln.direction().angleTo(chord.direction()) * 2.0, dTangentAngle, dChordAngle);
}

double CalcBulgeFactor(double dDeflectionAngle, double dTangentAngle, double dChordAngle)
{
	double dStartAngle = dTangentAngle + D90;
	double dEndAngle = dStartAngle + dDeflectionAngle;
	double dIncludeAngle = ( dChordAngle > dTangentAngle ? dEndAngle - dStartAngle : dStartAngle - dEndAngle );

	if(dTangentAngle < D90) {
		if(dChordAngle <= D360 && dChordAngle >= (dTangentAngle + D180))
			dIncludeAngle *= -1.0;
	} else if (dTangentAngle < D180) {
		if(dChordAngle <= D360 && dChordAngle >= (dTangentAngle + D180))
			dIncludeAngle *= -1.0;
	} else if (dTangentAngle < D270) {
		if(dChordAngle <= (dTangentAngle - D180))
			dIncludeAngle *= -1.0;
	} else if (dTangentAngle < D360) {
		if(dChordAngle <= (dTangentAngle - D180))
			dIncludeAngle *= -1.0;
	}

	return tan(dIncludeAngle * 0.25);
}