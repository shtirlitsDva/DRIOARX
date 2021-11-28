// Utility.cpp
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
#include "Utility.h"

namespace Schedule
{
	uint16_t Schedule::resolveDn(AcString layerName)
	{
		static std::map<AcString, uint16_t> dnMap
		{
			{ _T("FJV-TWIN-DN32"), 32 },
			{ _T("FJV-FREM-DN32"), 32 },
			{ _T("FJV-RETUR-DN32"), 32 },
			{ _T("FJV-TWIN-DN40"), 40 },
			{ _T("FJV-FREM-DN40"), 40 },
			{ _T("FJV-RETUR-DN40"), 40 },
			{ _T("FJV-TWIN-DN50"), 50 },
			{ _T("FJV-FREM-DN50"), 50 },
			{ _T("FJV-RETUR-DN50"), 50 },
			{ _T("FJV-TWIN-DN65"), 65 },
			{ _T("FJV-FREM-DN65"), 65 },
			{ _T("FJV-RETUR-DN65"), 65 },
			{ _T("FJV-TWIN-DN80"), 80 },
			{ _T("FJV-FREM-DN80"), 80 },
			{ _T("FJV-RETUR-DN80"), 80 },
			{ _T("FJV-TWIN-DN100"), 100 },
			{ _T("FJV-FREM-DN100"), 100 },
			{ _T("FJV-RETUR-DN100"), 100 },
			{ _T("FJV-TWIN-DN125"), 125 },
			{ _T("FJV-FREM-DN125"), 125 },
			{ _T("FJV-RETUR-DN125"), 125 },
			{ _T("FJV-TWIN-DN150"), 150 },
			{ _T("FJV-FREM-DN150"), 150 },
			{ _T("FJV-RETUR-DN150"), 150 },
			{ _T("FJV-TWIN-DN200"), 200 },
			{ _T("FJV-FREM-DN200"), 200 },
			{ _T("FJV-RETUR-DN200"), 200 },
			{ _T("FJV-FREM-DN250"), 250 },
			{ _T("FJV-RETUR-DN250"), 250 },
			{ _T("FJV-FREM-DN300"), 300 },
			{ _T("FJV-RETUR-DN300"), 300 },
			{ _T("FJV-FREM-DN350"), 350 },
			{ _T("FJV-RETUR-DN350"), 350 },
			{ _T("FJV-FREM-DN400"), 400 },
			{ _T("FJV-RETUR-DN400"), 400 },
			{ _T("FJV-FREM-DN450"), 450 },
			{ _T("FJV-RETUR-DN450"), 450 },
			{ _T("FJV-FREM-DN500"), 500 },
			{ _T("FJV-RETUR-DN500"), 500 },
			{ _T("FJV-FREM-DN600"), 600 },
			{ _T("FJV-RETUR-DN600"), 600 }
		};

		if (dnMap.count(layerName) == 0) return 999;
		return (uint16_t)dnMap[layerName];
	}
}
