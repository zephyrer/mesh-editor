#ifndef HEADER_SMOOTHER_H
#define HEADER_SMOOTHER_H

//平滑类，主要进行一些平滑操作
#include "PolyMesh.h"
#include "PBCG.h"
class Smoother
{
private:
	PolyMesh *polyMesh;

public:
	Smoother();
	~Smoother();
	void setPolyMesh(PolyMesh* _polyMesh);                //将多边形网格信息传递进来，然后才能进行平滑处理
	void implicitMeanCurvatureFlow(float _dt);            //在一次迭代中，进行步长为dt的一次平滑
};
#endif