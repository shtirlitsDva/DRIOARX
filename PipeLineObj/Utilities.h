#include "StdAfx.h"
#include <unordered_map>
#include "meta_enum.hpp"
#pragma once

namespace Logstor
{
	namespace BondedPipeSystem
	{
		namespace Common
		{
			double GetOd(int size);
		}
		namespace SinglePipe{}
		namespace TwinPipe
		{
			namespace Serie1 {}
			namespace Serie2 {}
			namespace Serie3
			{
				double GetLineWidth(int size);
			}
		}
	}
	namespace PexFlextra
	{
		namespace SinglePipe {}
		namespace TwinPipe {}
	}
	namespace AluFlextra
	{
		namespace SinglePipe {}
		namespace TwinPipe {}
		namespace DoublePipe {}
	}
	namespace SteelFlex
	{
		namespace SinglePipe {}
	}
	namespace CuFlex
	{
		namespace SinglePipe {}
		namespace TwinPipe {}
	}
	namespace CopperPipeSystem
	{
		namespace SinglePipe {}
		namespace TwinPipe {}
	}
}

namespace Enums
{
	meta_enum_class(Supplier, int, None, Logstor, Isoplus, Starpipe);
	meta_enum_class(Type, int, None, BondedPipeSystem, PexFlextra, AluFlextra, SteelFlex, CuFlex, CopperPipeSystem);
	meta_enum_class(System, int, None, SinglePipe, TwinPipe, DoublePipe);
	meta_enum_class(Series, int, None, Serie_1, Serie_2, Serie_3);
}
namespace LookUps
{
	double GetOd(int key, int size);
	double GetKOd(int key, int size);
}