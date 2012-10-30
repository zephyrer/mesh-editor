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
	float distance(float *vertex1,float *vertex2);        //���㶥��vertex1[3]������vertex2[3]�ľ���
	void setPolyMesh(PolyMesh* _polyMesh);                //�������������Ϣ���ݽ�����Ȼ����ܽ���ƽ������
	void implicitMeanCurvatureFlow(float _dt);            //��һ�ε����У����в���Ϊdt��һ��ƽ��
	void laplacianFlow(int _weightSet1,float _dt);        //��һ�ε����У��������ڵ�Ȩ�صĲ�ͬѡ�񷽷����в���Ϊdt��һ��ƽ��
};
#endif