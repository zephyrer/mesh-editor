                                      #pragma once

#include "stdafx.h"
#include "Smoother.h"

Smoother::Smoother()
{

}

Smoother::~Smoother()
{

}

double Smoother::distance(float *vertex1,float *vertex2)
{
	double temp[3];
	temp[0] = (double)(vertex1[0]-vertex2[0]);
	temp[1] = (double)(vertex1[1]-vertex2[1]);
	temp[2] = (double)(vertex1[2]-vertex2[2]);
	return sqrt(temp[0]*temp[0]+temp[1]*temp[1]+temp[2]*temp[2]);
}

void Smoother::setPolyMesh(PolyMesh* _polyMesh)
{
	polyMesh = _polyMesh;
}

void Smoother::laplacianFlow(int _weightSet1,float _dt)
{
	int vertexN = polyMesh->getVertexN();             //�������
	int *isBound = polyMesh->isBound;                 //ÿ��������ͣ��ڲ����߽磬�����㣩
	float (*vertex)[3] = polyMesh->vertex;            //���ж��������
	int *degreeV = polyMesh->degreeV;                 //��¼ÿ����������ڶ���ĸ���
	int **vertexLinkV = polyMesh->vertexLinkV;        //��¼���ж�����������ڶ�����±�
           
	if(_weightSet1 == -1)            //��ʱû��ѡ�����ڵ��Ȩ��
		return;     
	else if(_weightSet1 == 0)        //��ʱѡ��wi=1
	{
		for(int i=0;i<vertexN;i++)
		{
			if(degreeV[i] != 0 && isBound[i] != BOUNDARY && isBound[i] != NONMANIFOLD)     //����ö��㲻�Ǳ߽�ͬʱ���ǹ�����
			{
				double temp[3] = {0.0,0.0,0.0};             //x,y,z��
				for(int j=0;j<degreeV[i];j++)
				{
					temp[0] = temp[0]+vertex[vertexLinkV[i][j]][0];
					temp[1] = temp[1]+vertex[vertexLinkV[i][j]][1];
					temp[2] = temp[2]+vertex[vertexLinkV[i][j]][2];
				}
				temp[0] = temp[0]/degreeV[i];
				temp[1] = temp[1]/degreeV[i];
				temp[2] = temp[2]/degreeV[i];
				//����U0
				temp[0] = temp[0] - vertex[i][0];
				temp[1] = temp[1] - vertex[i][1];
				temp[2] = temp[2] - vertex[i][2];
				//�����µĶ����λ��
				vertex[i][0] = vertex[i][0] + (float)(_dt*temp[0]);
				vertex[i][1] = vertex[i][1] + (float)(_dt*temp[1]);
				vertex[i][2] = vertex[i][2] + (float)(_dt*temp[2]);
			}
		}
	}
	else if(_weightSet1 == 1)        //��ʱѡ��wi=1/distance
	{
		for(int i=0;i<vertexN;i++)
		{
			if(degreeV[i] != 0 && isBound[i] != BOUNDARY && isBound[i] != NONMANIFOLD)     //����ö��㲻�Ǳ߽�ͬʱ���ǹ�����
			{
				double temp[3] = {0.0,0.0,0.0};             //x,y,z��
				double totalWeight = 0.0;
				for(int j=0;j<degreeV[i];j++)
				{
					//����õ��ÿ���ڽ�������Ӧ��Ȩ��
					double weight = distance(vertex[i],vertex[vertexLinkV[i][j]]);
					if(weight == 0.0)
						continue;
					else 
						weight = 1.0/weight;
					totalWeight = totalWeight+weight;
					temp[0] = temp[0]+weight*vertex[vertexLinkV[i][j]][0];
					temp[1] = temp[1]+weight*vertex[vertexLinkV[i][j]][1];
					temp[2] = temp[2]+weight*vertex[vertexLinkV[i][j]][2];
				}
				temp[0] = temp[0]/totalWeight;
				temp[1] = temp[1]/totalWeight;
				temp[2] = temp[2]/totalWeight;
				//����U0
				temp[0] = temp[0] - vertex[i][0];
				temp[1] = temp[1] - vertex[i][1];
				temp[2] = temp[2] - vertex[i][2];
				//�����µĶ����λ��
				vertex[i][0] = vertex[i][0] + (float)(_dt*temp[0]);
				vertex[i][1] = vertex[i][1] + (float)(_dt*temp[1]);
				vertex[i][2] = vertex[i][2] + (float)(_dt*temp[2]);
			}
		}		
	}
}

void Smoother::explicitMeanCurvatureFlow(float _dt)
{
	int vertexN = polyMesh->getVertexN();             //�������
	int *isBound = polyMesh->isBound;                 //ÿ��������ͣ��ڲ����߽磬�����㣩
	float (*vertex)[3] = polyMesh->vertex;            //���ж��������
	int *degreeV = polyMesh->degreeV;                 //��¼ÿ����������ڶ���ĸ���
	int **vertexLinkV = polyMesh->vertexLinkV;        //��¼���ж�����������ڶ������

	double (*temp)[3] = new double[vertexN][3];
	for(int i=0;i<vertexN;i++)
	{
		temp[i][0] = 0.0;
		temp[i][1] = 0.0;
		temp[i][2] = 0.0;
		if(degreeV[i] != 0||isBound[i] != BOUNDARY||isBound[i] != NONMANIFOLD)     //��ʱҪô�Ǳ߽�Ҫô�ǹ�����
		{
			double A = 0.0;                          //���ڴ�Ŷ������Χ������������
			for(int j=0;j<degreeV[i];j++)
			{                                        //�赱ǰ�Ķ���ΪP
				int t = vertexLinkV[i][j];                  //��������ε�һ��������±�,��ΪQ
				int s = vertexLinkV[i][(j+1)%degreeV[i]];   //��������ε���һ��������±�,��ΪR

				float PQ[3],PR[3],QR[3];
				PolyMesh::VEC(PQ,vertex[i],vertex[s]);    //����PQ
				PolyMesh::VEC(PR,vertex[i],vertex[t]);    //����PR
				PolyMesh::VEC(QR,vertex[s],vertex[t]);    //����QR

				double Ai = PolyMesh::AREA(vertex[i],vertex[s],vertex[t]);     //��ǰ�����ε����

				if(Ai > 0.0)
				{
					A = A + Ai;
					double dot1 = -PolyMesh::DOT(PQ,QR);    //����PQ�������QR
					double dot2 = PolyMesh::DOT(PR,QR);     //����PR�������QR
					double cot1 = 0.5*dot1/Ai;
					double cot2 = 0.5*dot2/Ai;
					temp[i][0] = temp[i][0] + cot1*PR[0] + cot2*PQ[0];
					temp[i][1] = temp[i][1] + cot1*PR[1] + cot2*PQ[1];
					temp[i][2] = temp[i][2] + cot1*PR[2] + cot2*PQ[2];
				}
			}
			if(A > 0.0)
			{
				A = 4*A;
				temp[i][0] = temp[i][0]/A;
				temp[i][1] = temp[i][1]/A;
				temp[i][2] = temp[i][2]/A;
			}
		}
	}
	//���vertex���µ�λ��
	for(int i=0;i<vertexN;i++)
	{	
		vertex[i][0] = (float)(vertex[i][0] + _dt*temp[i][0]);
		vertex[i][1] = (float)(vertex[i][1] + _dt*temp[i][1]);
		vertex[i][2] = (float)(vertex[i][2] + _dt*temp[i][2]);
	}
}

void Smoother::bilaplacianFlow(int _weightSet3,float _dt)
{
	int vertexN = polyMesh->getVertexN();             //�������
	int *isBound = polyMesh->isBound;                 //ÿ��������ͣ��ڲ����߽磬�����㣩
	float (*vertex)[3] = polyMesh->vertex;            //���ж��������
	int *degreeV = polyMesh->degreeV;                 //��¼ÿ����������ڶ���ĸ���
	int **vertexLinkV = polyMesh->vertexLinkV;        //��¼���ж�����������ڶ�����±�

	if(_weightSet3 == -1)            //��ʱû��ѡ�����ڵ��Ȩ��
		return;     
	else if(_weightSet3 == 0)        //��ʱѡ��wi=1
	{
		double (*U)[3] = new double[vertexN][3];             //U(P)=(1/n)*(sigma(Qi))-P,Ϊ�˾�ȷ����Ϊdouble��
		//����(*U)[3]��ֵ
		for(int i=0;i<vertexN;i++)
		{
			if(degreeV[i] != 0 && isBound[i] != BOUNDARY && isBound[i] != NONMANIFOLD)     //����ö��㲻�Ǳ߽�ͬʱ���ǹ�����
			{
				double temp[3] = {0.0,0.0,0.0};             //x,y,z��
				for(int j=0;j<degreeV[i];j++)
				{
					temp[0] = temp[0]+vertex[vertexLinkV[i][j]][0];
					temp[1] = temp[1]+vertex[vertexLinkV[i][j]][1];
					temp[2] = temp[2]+vertex[vertexLinkV[i][j]][2];
				}
				temp[0] = temp[0]/degreeV[i];
				temp[1] = temp[1]/degreeV[i];
				temp[2] = temp[2]/degreeV[i];
				//����U0
				temp[0] = temp[0] - vertex[i][0];
				temp[1] = temp[1] - vertex[i][1];
				temp[2] = temp[2] - vertex[i][2];
				//��temp[3]��ֵ��ֵ��(*U)[3]
				U[i][0] = temp[0];
				U[i][1] = temp[1];
				U[i][2] = temp[2];
			}
			else
			{
				U[i][0] = 0;
				U[i][1] = 0;
				U[i][2] = 0;
			}
		}

		//�����vertex����λ��
		for(int i=0;i<vertexN;i++)
		{
			if(degreeV[i] != 0 && isBound[i] != BOUNDARY && isBound[i] != NONMANIFOLD)     //����ö��㲻�Ǳ߽�ͬʱ���ǹ�����
			{
				double temp[3] = {0.0,0.0,0.0};
				double v = 0.0;
				for(int j=0;j<degreeV[i];j++)
				{
					temp[0] = temp[0] + U[i][0] - U[vertexLinkV[i][j]][0];
					temp[1] = temp[1] + U[i][1] - U[vertexLinkV[i][j]][1];
					temp[2] = temp[2] + U[i][2] - U[vertexLinkV[i][j]][2];		
					v = v + 1.0/degreeV[vertexLinkV[i][j]];
				}
				v = v + degreeV[i];
				vertex[i][0] = vertex[i][0] + (float)(_dt*temp[0]/v);
				vertex[i][1] = vertex[i][1] + (float)(_dt*temp[1]/v);
				vertex[i][2] = vertex[i][2] + (float)(_dt*temp[2]/v);
			}
			//����Ļ���vertex[i][3]��ֵ����Ҫ��
		}
		delete[] U;
	}
	else if(_weightSet3 == 1)        //��ʱѡ��wi=1/distance
	{
		double (*U)[3] = new double[vertexN][3];             //U(P)=(1/n)*(sigma(Qi))-P,Ϊ�˾�ȷ����Ϊdouble��
		//����(*U)[3]��ֵ
		for(int i=0;i<vertexN;i++)
		{
			if(degreeV[i] != 0 && isBound[i] != BOUNDARY && isBound[i] != NONMANIFOLD)     //����ö��㲻�Ǳ߽�ͬʱ���ǹ�����
			{
				double temp[3] = {0.0,0.0,0.0};             //x,y,z��
				double totalWeight = 0.0;
				for(int j=0;j<degreeV[i];j++)
				{
					//����õ��ÿ���ڽ�������Ӧ��Ȩ��
					double weight = distance(vertex[i],vertex[vertexLinkV[i][j]]);
					if(weight == 0.0)
						continue;
					else 
						weight = 1.0/weight;
					temp[0] = temp[0]+weight*vertex[vertexLinkV[i][j]][0];
					temp[1] = temp[1]+weight*vertex[vertexLinkV[i][j]][1];
					temp[2] = temp[2]+weight*vertex[vertexLinkV[i][j]][2];
					totalWeight = totalWeight + weight;
				}
				temp[0] = temp[0]/totalWeight;
				temp[1] = temp[1]/totalWeight;
				temp[2] = temp[2]/totalWeight;
				//����U0
				temp[0] = temp[0] - vertex[i][0];
				temp[1] = temp[1] - vertex[i][1];
				temp[2] = temp[2] - vertex[i][2];
				//��temp[3]��ֵ��ֵ��(*U)[3]
				U[i][0] = temp[0];
				U[i][1] = temp[1];
				U[i][2] = temp[2];
			}
			else
			{
				U[i][0] = 0;
				U[i][1] = 0;
				U[i][2] = 0;
			}
		}

		//�����vertex����λ��
		for(int i=0;i<vertexN;i++)
		{
			if(degreeV[i] != 0 && isBound[i] != BOUNDARY && isBound[i] != NONMANIFOLD)     //����ö��㲻�Ǳ߽�ͬʱ���ǹ�����
			{
				double temp[3] = {0.0,0.0,0.0};
				double v = 0.0;
				double totalWeight = 0.0;
				for(int j=0;j<degreeV[i];j++)
				{
					//����õ��ÿ���ڽ�������Ӧ��Ȩ��
					double weight = distance(vertex[i],vertex[vertexLinkV[i][j]]);
					if(weight == 0.0)
						continue;
					else 
						weight = 1.0/weight;
					temp[0] = temp[0] + weight*U[vertexLinkV[i][j]][0];
					temp[1] = temp[1] + weight*U[vertexLinkV[i][j]][1];
					temp[2] = temp[2] + weight*U[vertexLinkV[i][j]][2];	
					totalWeight = totalWeight + weight;
					v = v + 1.0/degreeV[vertexLinkV[i][j]];
				}
				v = v/degreeV[i]+1;
				temp[0] = temp[0]/totalWeight - U[i][0];
				temp[1] = temp[1]/totalWeight - U[i][1];
				temp[2] = temp[2]/totalWeight - U[i][2];               //UU�����
				vertex[i][0] = vertex[i][0] - (float)(_dt*temp[0]/v);
				vertex[i][1] = vertex[i][1] - (float)(_dt*temp[1]/v);
				vertex[i][2] = vertex[i][2] - (float)(_dt*temp[2]/v);
			}
			//����Ļ���vertex[i][3]��ֵ����Ҫ��
		}
		delete[] U;		
	}
}

void Smoother::taubinSmoothing(int _weightSet4,float _lambda,float _mu)
{
	int vertexN = polyMesh->getVertexN();             //�������
	int *isBound = polyMesh->isBound;                 //ÿ��������ͣ��ڲ����߽磬�����㣩
	float (*vertex)[3] = polyMesh->vertex;            //���ж��������
	int *degreeV = polyMesh->degreeV;                 //��¼ÿ����������ڶ���ĸ���
	int **vertexLinkV = polyMesh->vertexLinkV;        //��¼���ж�����������ڶ�����±�

	if(_weightSet4 == -1)            //��ʱû��ѡ�����ڵ��Ȩ��
		return;     
	else if(_weightSet4 == 0)        //��ʱѡ��wi=1
	{
		double (*U)[3] = new double[vertexN][3];             //U(P)=(1/n)*(sigma(Qi))-P,Ϊ�˾�ȷ����Ϊdouble��
		double (*UU)[3] = new double[vertexN][3];            //UU(P)=(1/n)*(sigma(U(Qi)))-U(P),Ϊ�˾�ȷ����Ϊdouble��
		//����(*U)[3]��ֵ
		for(int i=0;i<vertexN;i++)
		{
			if(degreeV[i] != 0 && isBound[i] != BOUNDARY && isBound[i] != NONMANIFOLD)     //����ö��㲻�Ǳ߽�ͬʱ���ǹ�����
			{
				double temp[3] = {0.0,0.0,0.0};             //x,y,z��
				for(int j=0;j<degreeV[i];j++)
				{
					temp[0] = temp[0]+vertex[vertexLinkV[i][j]][0];
					temp[1] = temp[1]+vertex[vertexLinkV[i][j]][1];
					temp[2] = temp[2]+vertex[vertexLinkV[i][j]][2];
				}
				temp[0] = temp[0]/degreeV[i];
				temp[1] = temp[1]/degreeV[i];
				temp[2] = temp[2]/degreeV[i];
				//����U0
				temp[0] = temp[0] - vertex[i][0];
				temp[1] = temp[1] - vertex[i][1];
				temp[2] = temp[2] - vertex[i][2];
				//��temp[3]��ֵ��ֵ��(*U)[3]
				U[i][0] = temp[0];
				U[i][1] = temp[1];
				U[i][2] = temp[2];
			}
			else
			{
				U[i][0] = 0.0;
				U[i][1] = 0.0;
				U[i][2] = 0.0;
			}
		}

		//�����(*UU)[3]��ֵ
		for(int i=0;i<vertexN;i++)
		{
			if(degreeV[i] != 0 && isBound[i] != BOUNDARY && isBound[i] != NONMANIFOLD)     //����ö��㲻�Ǳ߽�ͬʱ���ǹ�����
			{
				double temp[3] = {0.0,0.0,0.0};
				double v = 0.0;
				for(int j=0;j<degreeV[i];j++)
				{
					temp[0] = temp[0] + U[vertexLinkV[i][j]][0] - U[i][0];
					temp[1] = temp[1] + U[vertexLinkV[i][j]][1] - U[i][1];
					temp[2] = temp[2] + U[vertexLinkV[i][j]][2] - U[i][2];		
					v = v + 1.0/degreeV[vertexLinkV[i][j]];
				}
				v = v + degreeV[i];
				UU[i][0] = temp[0]/v;
				UU[i][1] = temp[1]/v;
				UU[i][2] = temp[2]/v;
			}
			else
			{
				UU[i][0] = 0.0;
				UU[i][1] = 0.0;
				UU[i][2] = 0.0;
			}
		}
		//����vertex���µ�λ��
		for(int i=0;i<vertexN;i++)
		{
			vertex[i][0] = (float)(vertex[i][0] -(_mu-_lambda)*U[i][0] -_mu*_lambda*UU[i][0]);
			vertex[i][1] = (float)(vertex[i][1] -(_mu-_lambda)*U[i][1] -_mu*_lambda*UU[i][1]);
			vertex[i][2] = (float)(vertex[i][2] -(_mu-_lambda)*U[i][2] -_mu*_lambda*UU[i][2]);			
		}
		delete[] U;
		delete[] UU;
	}
	else if(_weightSet4 == 1)        //��ʱѡ��wi=1/distance
	{
		double (*U)[3] = new double[vertexN][3];             
		double (*UU)[3] = new double[vertexN][3];
		//����(*U)[3]��ֵ
		for(int i=0;i<vertexN;i++)
		{
			if(degreeV[i] != 0 && isBound[i] != BOUNDARY && isBound[i] != NONMANIFOLD)     //����ö��㲻�Ǳ߽�ͬʱ���ǹ�����
			{
				double temp[3] = {0.0,0.0,0.0};             //x,y,z��
				double totalWeight = 0.0;
				for(int j=0;j<degreeV[i];j++)
				{
					//����õ��ÿ���ڽ�������Ӧ��Ȩ��
					double weight = distance(vertex[i],vertex[vertexLinkV[i][j]]);
					if(weight == 0.0)
						continue;
					else 
						weight = 1.0/weight;
					temp[0] = temp[0]+weight*vertex[vertexLinkV[i][j]][0];
					temp[1] = temp[1]+weight*vertex[vertexLinkV[i][j]][1];
					temp[2] = temp[2]+weight*vertex[vertexLinkV[i][j]][2];
					totalWeight = totalWeight + weight;
				}
				temp[0] = temp[0]/totalWeight;
				temp[1] = temp[1]/totalWeight;
				temp[2] = temp[2]/totalWeight;
				//����U0
				temp[0] = temp[0] - vertex[i][0];
				temp[1] = temp[1] - vertex[i][1];
				temp[2] = temp[2] - vertex[i][2];
				//��temp[3]��ֵ��ֵ��(*U)[3]
				U[i][0] = temp[0];
				U[i][1] = temp[1];
				U[i][2] = temp[2];
			}
			else
			{
				U[i][0] = 0.0;
				U[i][1] = 0.0;
				U[i][2] = 0.0;
			}
		}

		//�����(*UU)[3]��ֵ
		for(int i=0;i<vertexN;i++)
		{
			if(degreeV[i] != 0 && isBound[i] != BOUNDARY && isBound[i] != NONMANIFOLD)     //����ö��㲻�Ǳ߽�ͬʱ���ǹ�����
			{
				double temp[3] = {0.0,0.0,0.0};
				double v = 0.0;
				double totalWeight = 0.0;
				for(int j=0;j<degreeV[i];j++)
				{
					//����õ��ÿ���ڽ�������Ӧ��Ȩ��
					double weight = distance(vertex[i],vertex[vertexLinkV[i][j]]);
					if(weight == 0.0)
						continue;
					else 
						weight = 1.0/weight;
					temp[0] = temp[0] + weight*U[vertexLinkV[i][j]][0];
					temp[1] = temp[1] + weight*U[vertexLinkV[i][j]][1];
					temp[2] = temp[2] + weight*U[vertexLinkV[i][j]][2];	
					totalWeight = totalWeight + weight;
					v = v + 1.0/degreeV[vertexLinkV[i][j]];
				}
				v = v/degreeV[i]+1;
				temp[0] = temp[0]/totalWeight - U[i][0];
				temp[1] = temp[1]/totalWeight - U[i][1];
				temp[2] = temp[2]/totalWeight - U[i][2];               
				//UU�����
				UU[i][0] = temp[0];
				UU[i][1] = temp[1];
				UU[i][2] = temp[2];
			}
			else
			{
				UU[i][0] = 0.0;
				UU[i][1] = 0.0;
				UU[i][2] = 0.0;
			}
		}
		//����vertex���µ�λ��
		for(int i=0;i<vertexN;i++)
		{
			vertex[i][0] = (float)(vertex[i][0] -(_mu-_lambda)*U[i][0] -_mu*_lambda*UU[i][0]);
			vertex[i][1] = (float)(vertex[i][1] -(_mu-_lambda)*U[i][1] -_mu*_lambda*UU[i][1]);
			vertex[i][2] = (float)(vertex[i][2] -(_mu-_lambda)*U[i][2] -_mu*_lambda*UU[i][2]);			
		}
		delete[] U;
		delete[] UU;
	}
}

void Smoother::implicitMeanCurvatureFlow(float _dt)
{
	//����PBCG�࣬��Ҫ��Ϊ�˺���Ҫ����PBCG���ڲ�����
	PBCG* pbcg = new PBCG;
	int vertexN = polyMesh->getVertexN();        //��ö������
	int *degreeV = polyMesh->degreeV;            //degreeV���ÿ�������Ӧ�Ķ���
	int size = 0;                                //���ڼ�¼���ж���Ķ���֮�ͼ��϶���ĸ���
	for(int i=0;i<vertexN;i++)
	{
		size = size + degreeV[i];
	}
	size = size + vertexN;

	pbcg->sa = new double[size+2];
	pbcg->ija = new unsigned long[size+2];

	//����ϡ�����ĸ���λ�ô���ֵ��˳���������洢��������ϡ�����洢����
	int **vertexLinkV = polyMesh->vertexLinkV;   //vertexLinkV���ÿ������������ڽ�����±�
	float (*vertex)[3] = polyMesh->vertex;       //vertex������ж����3ά����
	int *isBound = polyMesh->isBound;            //isBoundΪÿ����������ͣ��ڲ����߽磬�����㣩
	double *sa = pbcg->sa;      
	unsigned long *ija = pbcg->ija;              //��Ҫ��Ϊ����д����

	ija[1] = vertexN+2;                          //�����洢�����ı�Ȼ
	unsigned long k = vertexN+1;
	for(int i=1;i<=vertexN;i++)
	{
		int ii = i-1;                            //ע���±��ת��
		int degree = degreeV[ii];
		if(degree == 0||isBound[ii] == BOUNDARY||isBound[ii] == NONMANIFOLD)     //��ʱҪô�Ǳ߽�Ҫô�ǹ�����
		{
			sa[i] = 1.0;
			ija[i+1] = k+1;
			continue;
		}
		int *temp1 = vertexLinkV[ii];            //��ʱ��ŵ�ǰ����������ڽ�����±�
		double *w = new double[degree];          //���ڴ�ŵ�ǰ����������ڽ����cotֵ֮��
		for(int j=0;j<degree;j++)
			w[j] = 0;                            //����ʼ��Ϊ0
		double totalCots=0;                      //���ڴ�ŵ�ǰ�����cot1+cot2ֵ֮��
		double A = 0;                            //���ڴ�Ŷ������Χ������������
		for(int j=0;j<degree;j++)
		{                                        //�赱ǰ�Ķ���ΪP
			int t = temp1[j];                    //��������ε�һ��������±�,��ΪQ
			int s = temp1[(j+1)%degree];         //��������ε���һ��������±�,��ΪR

			float PQ[3],PR[3],QR[3];
			PolyMesh::VEC(PQ,vertex[ii],vertex[s]);    //����PQ
			PolyMesh::VEC(PR,vertex[ii],vertex[t]);    //����PR
			PolyMesh::VEC(QR,vertex[s],vertex[t]);     //����QR

			double Ai = PolyMesh::AREA(vertex[ii],vertex[s],vertex[t]);     //��ǰ�����ε����

			if(Ai > 0)
			{
				A = A+Ai;
				double dot1 = -PolyMesh::DOT(PQ,QR);    //����PQ�������QR
				double dot2 = PolyMesh::DOT(PR,QR);     //����PR�������QR

				double cot1 = dot1/Ai;
				double cot2 = dot2/Ai;

				w[j] += cot1;
				w[(j+1)%degree] += cot2;
				totalCots += cot1+cot2;
			}
		}
		A = 2.0*A;
		if(A > 0)
			sa[i] = 1.0+_dt*totalCots/A;                 //���еĶԽ����ϵ�Ԫ���Ѹ�ֵ����
		else
			sa[i] = 1.0;

		int *temp2 = new int[degree];                   //��ŵ�ǰ��������ڽ�����±�+1����ΪPBCG�㷨���±궼�Ǵ�1��ʼ�ģ�
		for(int j=0;j<degree;j++)
			temp2[j] = temp1[j]+1;
		//�ڶԾ���ǰ�еķ���㣨���Խ���Ԫ�أ����и�ֵ֮ǰ��Ҫ�Ƚ�temp2�����±��С����������򣬵�Ȼ���Ӧ��w��ֵҲ��Ӧ����
		for(int pp=0;pp<degree;pp++)
		{
			for(int qq=pp;qq<degree;qq++)
			{
				if(temp2[pp] > temp2[qq])
				{
					int temp3 = temp2[pp];
					temp2[pp] = temp2[qq];
					temp2[qq] = temp3;
					double temp4 = w[pp];
					w[pp] = w[qq];
					w[qq] = temp4;
				}
			}
		}
		//�Ծ���ǰ�еķ���㣨���Խ���Ԫ�أ����и�ֵ
		for(int j=0;j<degree;j++)
		{
			if(w[j] == 0.0)
				continue;
			k++;
			sa[k] = -_dt*w[j]/A;
			ija[k] = temp2[j];
		}
		ija[i+1] = k+1;

		delete[] temp2;
		delete[] w;
	}

	int iter;                                         //���ʵ�ʽ��е����Ĵ���
	double err;                                       //���ʵ�ʲ����Ĺ������
	double *old_vertex = new double[vertexN+1];
	double *new_vertex = new double[vertexN+1];

	//X��������
	for(int i=0;i<vertexN;i++)
	{
		old_vertex[i+1] = vertex[i][0];
		new_vertex[i+1] = vertex[i][0];
	}
	//����linbcg����������������µ����е��X����
	pbcg->linbcg(vertexN,old_vertex,new_vertex,1,1e-5,100,&iter,&err);
	for(int i=0;i<vertexN;i++)
		vertex[i][0] = (float)new_vertex[i+1];

	//Y��������
	for(int i=0;i<vertexN;i++)
	{
		old_vertex[i+1] = vertex[i][1];
		new_vertex[i+1] = vertex[i][1];
	}
	//����linbcg����������������µ����е��Y����
	pbcg->linbcg(vertexN,old_vertex,new_vertex,1,1e-5,100,&iter,&err);
	for(int i=0;i<vertexN;i++)
		vertex[i][1] = (float)new_vertex[i+1];

	//Z��������
	for(int i=0;i<vertexN;i++)
	{
		old_vertex[i+1] = vertex[i][2];
		new_vertex[i+1] = vertex[i][2];
	}
	//����linbcg����������������µ����е��Z����
	pbcg->linbcg(vertexN,old_vertex,new_vertex,1,1e-5,100,&iter,&err);
	for(int i=0;i<vertexN;i++)
		vertex[i][2] = (float)new_vertex[i+1];

	//�ͷ���Դ
	delete[] old_vertex;
	delete[] new_vertex;
	delete pbcg;
} 