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
	float distance(float *vertex1,float *vertex2);        //计算顶点vertex1[3]到顶点vertex2[3]的距离
	void setPolyMesh(PolyMesh* _polyMesh);                //将多边形网格信息传递进来，然后才能进行平滑处理
	void implicitMeanCurvatureFlow(float _dt);            //在一次迭代中，进行步长为dt的一次平滑
	void laplacianFlow(int _weightSet1,float _dt);        //在一次迭代中，根据相邻点权重的不同选择方法进行步长为dt的一次平滑
};
#endif