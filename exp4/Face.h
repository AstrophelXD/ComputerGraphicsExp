#pragma once
#include "P3.h"

class CFace
{
public:
	CFace();
	virtual ~CFace();
	void SetNum(int);
public:
	int vN;          //小面的顶点数
	int* vI;         //小面的顶点索引
};

