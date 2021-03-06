// Quadtree.cpp: 定义应用程序的入口点。
//

#include "Quadtree.h"

const dir_table dir_table::table[9][4] = {
	{},
{/*R*/{ R,1 },{ HALT,0 },{ R,3 },{ HALT,2 } },
{/*L*/{ HALT,1 },{ L,0 },{ HALT,3 },{ L,2 } },
{/*D*/{ HALT,2 },{ HALT,3 },{ D,0 },{ D,1 } },
{/*U*/{ U,2 },{ U,3 },{ HALT,0 },{ HALT,1 } },
{/*RU*/{ RU,3 },{ U,2 },{ R,1 },{ HALT,0 } },
{/*LU*/{ U,3 },{ LU,2 },{ HALT,1 },{ L,0 } },
{/*RD*/{ R,3 },{ HALT,2 },{ RD,1 },{ D,0 } },
{/*LD*/{ HALT,3 },{ L,2 },{ D,1 },{ LD,0 } }
};  

