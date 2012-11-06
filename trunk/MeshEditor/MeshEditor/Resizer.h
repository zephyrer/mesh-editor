#ifndef HEADER_RESIZER_H
#define HEADER_RESIZER_H

#include "PolyMesh.h"
//缩放类，进行一些缩放操作
class Resizer
{
private:
	PolyMesh *polyMesh;
public:
	Resizer();
	~Resizer();
};
#endif