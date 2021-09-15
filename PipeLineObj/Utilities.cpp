#include "StdAfx.h"
#include "Utilities.h"
#include "meta_enum.hpp"
#include <any>
#include <functional>
#include <vector>

namespace Isoplus
{
	namespace BondedPipeSystem
	{
		namespace Common
		{
			double GetOd(int size)
			{
				std::unordered_map<int, double> Ods =
				{
					{ 10, 17.2 },
					{ 15, 21.3 },
					{ 20, 26.9 },
					{ 25, 33.7 },
					{ 32, 42.4 },
					{ 40, 48.3 },
					{ 50, 60.3 },
					{ 65, 76.1 },
					{ 80, 88.9 },
					{ 100, 114.3 },
					{ 125, 139.7 },
					{ 150, 168.3 },
					{ 200, 219.1 },
					{ 250, 273.0 },
					{ 300, 323.9 },
					{ 350, 355.6 },
					{ 400, 406.4 },
					{ 450, 457.0 },
					{ 500, 508.0 },
					{ 600, 610.0 },
				};

				return Ods[size];
			}
		}
		namespace SinglePipe {}
		namespace TwinPipe
		{
			namespace Serie1 {}
			namespace Serie2 {}
			namespace Serie3
			{
				double GetKOd(int size)
				{
					std::unordered_map<int, double> kOds =
					{
						{  20, 160.0 },
						{  25, 180.0 },
						{  32, 200.0 },
						{  40, 200.0 },
						{  50, 250.0 },
						{  65, 280.0 },
						{  80, 315.0 },
						{ 100, 400.0 },
						{ 125, 500.0 },
						{ 150, 560.0 },
						{ 200, 710.0 }
					};

					return kOds[size];
				}
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
// Supplier			PipeType				System			Series
// *1000			*100					*10				*1
// 1	Logstor		1	BondedPipeSystem	1	SinglePipe	1	Serie_1
// 2	Isoplus		2	PexFlextra			2	TwinPipe	2	Serie_2
// 3	Starpipe	3	AluFlextra			3	DoublePipe	3	Serie_3
// 					4	SteelFlex
// 					5	CuFlex
// 					6	CopperPipeSystem

namespace LookUps
{
	double GetOd(int key, int size)
	{
		std::unordered_map<int, std::any> map =
		{
			{2123, std::function<double(int)>(Isoplus::BondedPipeSystem::Common::GetOd)}
		};

		return std::any_cast<std::function<double(int)>>(map[key])(size);
	}
	double GetKOd(int key, int size)
	{
		std::unordered_map<int, std::any> map =
		{
			{2123, std::function<double(int)>(Isoplus::BondedPipeSystem::TwinPipe::Serie3::GetKOd)}
		};

		return std::any_cast<std::function<double(int)>>(map[key])(size);
	}
}
