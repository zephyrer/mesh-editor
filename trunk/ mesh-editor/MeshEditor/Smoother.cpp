#pragma once

#include "stdafx.h"
#include "Smoother.h"

Smoother::Smoother()
{

}

Smoother::~Smoother()
{

}

void Smoother::setPolyMesh(PolyMesh* _polyMesh)
{
	polyMesh = _polyMesh;
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