#pragma once

#include "stdafx.h"
#include "Smoother.h"

Smoother::Smoother()
{

}

Smoother::~Smoother()
{

}

float Smoother::distance(float *vertex1,float *vertex2)
{
	float temp[3];
	temp[0] = vertex1[0]-vertex2[0];
	temp[1] = vertex1[1]-vertex2[1];
	temp[2] = vertex1[2]-vertex2[2];
	return sqrt(temp[0]*temp[0]+temp[1]*temp[1]+temp[2]*temp[2]);
}

void Smoother::setPolyMesh(PolyMesh* _polyMesh)
{
	polyMesh = _polyMesh;
}

void Smoother::implicitMeanCurvatureFlow(float _dt)
{
	//定义PBCG类，主要是为了后续要调用PBCG的内部函数
	PBCG* pbcg = new PBCG;
	int vertexN = polyMesh->getVertexN();        //获得顶点个数
	int *degreeV = polyMesh->degreeV;            //degreeV存放每个顶点对应的度数
	int size = 0;                                //用于记录所有顶点的度数之和加上顶点的个数
	for(int i=0;i<vertexN;i++)
	{
		size = size + degreeV[i];
	}
	size = size + vertexN;

	pbcg->sa = new double[size+2];
	pbcg->ija = new unsigned long[size+2];

	//计算稀疏矩阵的各个位置处的值，顺便用索引存储方案将该稀疏矩阵存储起来
	int **vertexLinkV = polyMesh->vertexLinkV;   //vertexLinkV存放每个顶点的所有邻近点的下标
	float (*vertex)[3] = polyMesh->vertex;       //vertex存放所有顶点的3维坐标
	int *isBound = polyMesh->isBound;            //isBound为每个顶点的类型（内部，边界，孤立点）
	double *sa = pbcg->sa;      
	unsigned long *ija = pbcg->ija;              //主要是为了书写方便

	ija[1] = vertexN+2;                          //索引存储方案的必然
	unsigned long k = vertexN+1;
	for(int i=1;i<=vertexN;i++)
	{
		int ii = i-1;                            //注意下标的转换
		int degree = degreeV[ii];
		if(degree == 0||isBound[ii] == BOUNDARY||isBound[ii] == NONMANIFOLD)     //此时要么是边界要么是孤立点
		{
			sa[i] = 1.0;
			ija[i+1] = k+1;
			continue;
		}
		int *temp1 = vertexLinkV[ii];            //临时存放当前顶点的所有邻近点的下标
		double *w = new double[degree];          //用于存放当前顶点的所有邻近点的cot值之和
		for(int j=0;j<degree;j++)
			w[j] = 0;                            //都初始化为0
		double totalCots=0;                      //用于存放当前顶点的cot1+cot2值之和
		double A = 0;                            //用于存放顶点的周围的作用域的面积
		for(int j=0;j<degree;j++)
		{                                        //设当前的顶点为P
			int t = temp1[j];                    //获得三角形的一个顶点的下标,记为Q
			int s = temp1[(j+1)%degree];         //获得三角形的另一个顶点的下标,记为R
			
			float PQ[3],PR[3],QR[3];
			PolyMesh::VEC(PQ,vertex[ii],vertex[s]);    //向量PQ
			PolyMesh::VEC(PR,vertex[ii],vertex[t]);    //向量PR
			PolyMesh::VEC(QR,vertex[s],vertex[t]);     //向量QR

			double Ai = PolyMesh::AREA(vertex[ii],vertex[s],vertex[t]);     //当前三角形的面积

			if(Ai > 0)
			{
				A = A+Ai;
				double dot1 = -PolyMesh::DOT(PQ,QR);    //向量PQ点乘向量QR
				double dot2 = PolyMesh::DOT(PR,QR);     //向量PR点乘向量QR

				double cot1 = dot1/Ai;
				double cot2 = dot2/Ai;

				w[j] += cot1;
				w[(j+1)%degree] += cot2;
				totalCots += cot1+cot2;
			}
		}
		A = 2.0*A;
		if(A > 0)
			sa[i] = 1.0+_dt*totalCots/A;                 //该行的对角线上的元素已赋值结束
		else
			sa[i] = 1.0;

		int *temp2 = new int[degree];                   //存放当前点的所有邻近点的下标+1（因为PBCG算法中下标都是从1开始的）
		for(int j=0;j<degree;j++)
			temp2[j] = temp1[j]+1;
		//在对矩阵当前行的非零点（除对角线元素）进行赋值之前需要先将temp2按照下标从小到大进行排序，当然其对应的w的值也相应排序
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
		//对矩阵当前行的非零点（除对角线元素）进行赋值
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

	int iter;                                         //存放实际进行迭代的次数
	double err;                                       //存放实际产生的估计误差
	double *old_vertex = new double[vertexN+1];
	double *new_vertex = new double[vertexN+1];

	//X坐标轴向
	for(int i=0;i<vertexN;i++)
	{
		old_vertex[i+1] = vertex[i][0];
		new_vertex[i+1] = vertex[i][0];
	}
	//调用linbcg函数经过迭代算出新的所有点的X坐标
	pbcg->linbcg(vertexN,old_vertex,new_vertex,1,1e-5,100,&iter,&err);
	for(int i=0;i<vertexN;i++)
		vertex[i][0] = (float)new_vertex[i+1];

	//Y坐标轴向
	for(int i=0;i<vertexN;i++)
	{
		old_vertex[i+1] = vertex[i][1];
		new_vertex[i+1] = vertex[i][1];
	}
	//调用linbcg函数经过迭代算出新的所有点的Y坐标
	pbcg->linbcg(vertexN,old_vertex,new_vertex,1,1e-5,100,&iter,&err);
	for(int i=0;i<vertexN;i++)
		vertex[i][1] = (float)new_vertex[i+1];

	//Z坐标轴向
	for(int i=0;i<vertexN;i++)
	{
		old_vertex[i+1] = vertex[i][2];
		new_vertex[i+1] = vertex[i][2];
	}
	//调用linbcg函数经过迭代算出新的所有点的Z坐标
	pbcg->linbcg(vertexN,old_vertex,new_vertex,1,1e-5,100,&iter,&err);
	for(int i=0;i<vertexN;i++)
		vertex[i][2] = (float)new_vertex[i+1];

	//释放资源
	delete[] old_vertex;
	delete[] new_vertex;
	delete pbcg;
} 

void Smoother::laplacianFlow(int _weightSet1,float _dt)
{
	int vertexN = polyMesh->getVertexN();             //顶点个数
	int *isBound = polyMesh->isBound;                 //每个点的类型（内部，边界，孤立点）
	float (*vertex)[3] = polyMesh->vertex;            //所有顶点的坐标
	int *degreeV = polyMesh->degreeV;                 //记录每个顶点的相邻顶点的个数
	int **vertexLinkV = polyMesh->vertexLinkV;        //记录所有顶点的所有相邻顶点的下标
           
	if(_weightSet1 == -1)            //此时没有选择相邻点的权重
		return;     
	else if(_weightSet1 == 0)        //此时选中wi=1
	{
		for(int i=0;i<vertexN;i++)
		{
			if(degreeV[i] != 0 && isBound[i] != BOUNDARY && isBound[i] != NONMANIFOLD)     //如果该顶点不是边界同时不是孤立点
			{
				float temp[3] = {0.0,0.0,0.0};             //x,y,z轴
				for(int j=0;j<degreeV[i];j++)
				{
					temp[0] = temp[0]+vertex[vertexLinkV[i][j]][0];
					temp[1] = temp[1]+vertex[vertexLinkV[i][j]][1];
					temp[2] = temp[2]+vertex[vertexLinkV[i][j]][2];
				}
				temp[0] = temp[0]/degreeV[i];
				temp[1] = temp[1]/degreeV[i];
				temp[2] = temp[2]/degreeV[i];
				//计算U0
				temp[0] = temp[0] - vertex[i][0];
				temp[1] = temp[1] - vertex[i][1];
				temp[2] = temp[2] - vertex[i][2];
				//计算新的顶点的位置
				vertex[i][0] = vertex[i][0] + _dt*temp[0];
				vertex[i][1] = vertex[i][1] + _dt*temp[1];
				vertex[i][2] = vertex[i][2] + _dt*temp[2];
			}
		}
	}
	else if(_weightSet1 == 1)        //此时选中wi=1/distance
	{
		for(int i=0;i<vertexN;i++)
		{
			if(degreeV[i] != 0 && isBound[i] != BOUNDARY && isBound[i] != NONMANIFOLD)     //如果该顶点不是边界同时不是孤立点
			{
				float temp[3] = {0.0,0.0,0.0};             //x,y,z轴
				float totalWeight = 0.0;
				for(int j=0;j<degreeV[i];j++)
				{
					//计算该点的每个邻近点所对应的权重
					float weight = distance(vertex[i],vertex[vertexLinkV[i][j]]);
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
				//计算U0
				temp[0] = temp[0] - vertex[i][0];
				temp[1] = temp[1] - vertex[i][1];
				temp[2] = temp[2] - vertex[i][2];
				//计算新的顶点的位置
				vertex[i][0] = vertex[i][0] + _dt*temp[0];
				vertex[i][1] = vertex[i][1] + _dt*temp[1];
				vertex[i][2] = vertex[i][2] + _dt*temp[2];
			}
		}		
	}
}