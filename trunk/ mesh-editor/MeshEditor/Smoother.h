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
	void setPolyMesh(PolyMesh* _polyMesh);                //�������������Ϣ���ݽ�����Ȼ����ܽ���ƽ������
	void implicitMeanCurvatureFlow(float _dt);            //��һ�ε����У����в���Ϊdt��һ��ƽ��
};
#endif