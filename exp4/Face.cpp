#include "Face.h"

CFace::CFace()
{
	vI = NULL;
	vN = 0;
}

CFace::~CFace()
{
	if (vI != NULL)
	{
		delete[]vI;
		vI = NULL;
	}
}

void CFace::SetNum(int n)
{
	vN = n;
	vI = new int[n];
}