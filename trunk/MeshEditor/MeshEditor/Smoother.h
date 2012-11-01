#ifndef HEADER_SMOOTHER_H
#define HEADER_SMOOTHER_H

//ƽ���࣬��Ҫ����һЩƽ������
#include "PolyMesh.h"
#include "PBCG.h"
class Smoother
{
private:
	PolyMesh *polyMesh;

public:
	Smoother();
	~Smoother();
	double distance(float *vertex1,float *vertex2);       //���㶥��vertex1[3]������vertex2[3]�ľ���
	void setPolyMesh(PolyMesh* _polyMesh);                //�������������Ϣ���ݽ�����Ȼ����ܽ���ƽ������
	void laplacianFlow(int _weightSet1,float _dt);        //����������˹ƽ��
	void explicitMeanCurvatureFlow(float _dt);            //��ʽ������ƽ��
	void bilaplacianFlow(int _weightSet3,float _dt);      //����������˹ƽ��
	void taubinSmoothing(int _weightSet4,float _lambda,float _mu);     //Taubinƽ��
	void implicitMeanCurvatureFlow(float _dt);            //��ʽ������ƽ��
};
#endif