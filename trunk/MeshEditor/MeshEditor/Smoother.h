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
	double distance(float *vertex1,float *vertex2);       //计算顶点vertex1[3]到顶点vertex2[3]的距离
	void setPolyMesh(PolyMesh* _polyMesh);                //将多边形网格信息传递进来，然后才能进行平滑处理
	void laplacianFlow(int _weightSet1,float _dt);        //经典拉普拉斯平滑
	void explicitMeanCurvatureFlow(float _dt);            //显式曲率流平滑
	void bilaplacianFlow(int _weightSet3,float _dt);      //二次拉普拉斯平滑
	void taubinSmoothing(int _weightSet4,float _lambda,float _mu);     //Taubin平滑
	void implicitMeanCurvatureFlow(float _dt);            //隐式曲率流平滑
};
#endif