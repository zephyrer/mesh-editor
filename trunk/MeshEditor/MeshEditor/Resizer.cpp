#pragma once
#include "stdafx.h"
#include "Resizer.h"
/********************************************************/
/* AABB-triangle overlap test code                      */
/* by Tomas Akenine-Moller                              */
/* Function: int triBoxOverlap(float boxcenter[3],      */
/*          float boxhalfsize[3],float triverts[3][3]); */
/* History:                                             */
/*   2001-03-05: released the code in its first version */
/*   2001-06-18: changed the order of the tests, faster */
/*                                                      */
/* Acknowledgement: Many thanks to Pierre Terdiman for  */
/* suggestions and discussions on how to optimize code. */
/* Thanks to David Hunt for finding a ">="-bug!         */
/********************************************************/
#define X 0
#define Y 1
#define Z 2

#define CROSS(dest,v1,v2) \
	dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
	dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
	dest[2]=v1[0]*v2[1]-v1[1]*v2[0]; 

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2) \
	dest[0]=v1[0]-v2[0]; \
	dest[1]=v1[1]-v2[1]; \
	dest[2]=v1[2]-v2[2]; 

#define FINDMINMAX(x0,x,x2,min,max) \
	min = max = x0;   \
	if(x<min) min=x;\
	if(x>max) max=x;\
	if(x2<min) min=x2;\
	if(x2>max) max=x2;

int planeBoxOverlap(float normal[3], float vert[3], float maxbox[3])	// -NJMP-
{
	int q;
	float vmin[3],vmax[3],v;
	for(q=X;q<=Z;q++)
	{
		v=vert[q];					// -NJMP-
		if(normal[q]>0.0f)
		{
			vmin[q]=-maxbox[q] - v;	// -NJMP-
			vmax[q]= maxbox[q] - v;	// -NJMP-
		}
		else
		{
			vmin[q]= maxbox[q] - v;	// -NJMP-
			vmax[q]=-maxbox[q] - v;	// -NJMP-
		}
	}
	if(DOT(normal,vmin)>0.0f) return 0;	// -NJMP-
	if(DOT(normal,vmax)>=0.0f) return 1;	// -NJMP-

	return 0;
}


/*======================== X-tests ========================*/
#define AXISTEST_X01(a, b, fa, fb)			   \
	p0 = a*v0[Y] - b*v0[Z];			       	   \
	p2 = a*v2[Y] - b*v2[Z];			       	   \
	if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
	rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
	if(min>rad || max<-rad) return 0;

#define AXISTEST_X2(a, b, fa, fb)			   \
	p0 = a*v0[Y] - b*v0[Z];			           \
	p1 = a*v1[Y] - b*v1[Z];			       	   \
	if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
	if(min>rad || max<-rad) return 0;

/*======================== Y-tests ========================*/
#define AXISTEST_Y02(a, b, fa, fb)			   \
	p0 = -a*v0[X] + b*v0[Z];		      	   \
	p2 = -a*v2[X] + b*v2[Z];	       	       	   \
	if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
	rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
	if(min>rad || max<-rad) return 0;

#define AXISTEST_Y1(a, b, fa, fb)			   \
	p0 = -a*v0[X] + b*v0[Z];		      	   \
	p1 = -a*v1[X] + b*v1[Z];	     	       	   \
	if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
	if(min>rad || max<-rad) return 0;

/*======================== Z-tests ========================*/

#define AXISTEST_Z12(a, b, fa, fb)			   \
	p1 = a*v1[X] - b*v1[Y];			           \
	p2 = a*v2[X] - b*v2[Y];			       	   \
	if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
	rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
	if(min>rad || max<-rad) return 0;

#define AXISTEST_Z0(a, b, fa, fb)			   \
	p0 = a*v0[X] - b*v0[Y];				   \
	p1 = a*v1[X] - b*v1[Y];			           \
	if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
	if(min>rad || max<-rad) return 0;

int triBoxOverlap(float boxcenter[3],float boxhalfsize[3],float triverts[3][3])
{

	/*    use separating axis theorem to test overlap between triangle and box */
	/*    need to test for overlap in these directions: */
	/*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
	/*       we do not even need to test these) */
	/*    2) normal of the triangle */
	/*    3) crossproduct(edge from tri, {x,y,z}-directin) */
	/*       this gives 3x3=9 more tests */
	float v0[3],v1[3],v2[3];
	//   float axis[3];
	float min,max,p0,p1,p2,rad,fex,fey,fez;		// -NJMP- "d" local variable removed
	float normal[3],e0[3],e1[3],e2[3];

	/* This is the fastest branch on Sun */
	/* move everything so that the boxcenter is in (0,0,0) */
	SUB(v0,triverts[0],boxcenter);
	SUB(v1,triverts[1],boxcenter);
	SUB(v2,triverts[2],boxcenter);

	/* compute triangle edges */
	SUB(e0,v1,v0);      /* tri edge 0 */
	SUB(e1,v2,v1);      /* tri edge 1 */
	SUB(e2,v0,v2);      /* tri edge 2 */

	/* Bullet 3:  */
	/*  test the 9 tests first (this was faster) */
	fex = fabsf(e0[X]);
	fey = fabsf(e0[Y]);
	fez = fabsf(e0[Z]);
	AXISTEST_X01(e0[Z], e0[Y], fez, fey);
	AXISTEST_Y02(e0[Z], e0[X], fez, fex);
	AXISTEST_Z12(e0[Y], e0[X], fey, fex);

	fex = fabsf(e1[X]);
	fey = fabsf(e1[Y]);
	fez = fabsf(e1[Z]);
	AXISTEST_X01(e1[Z], e1[Y], fez, fey);
	AXISTEST_Y02(e1[Z], e1[X], fez, fex);
	AXISTEST_Z0(e1[Y], e1[X], fey, fex);

	fex = fabsf(e2[X]);
	fey = fabsf(e2[Y]);
	fez = fabsf(e2[Z]);
	AXISTEST_X2(e2[Z], e2[Y], fez, fey);
	AXISTEST_Y1(e2[Z], e2[X], fez, fex);
	AXISTEST_Z12(e2[Y], e2[X], fey, fex);

	/* Bullet 1: */
	/*  first test overlap in the {x,y,z}-directions */
	/*  find min, max of the triangle each direction, and test for overlap in */
	/*  that direction -- this is equivalent to testing a minimal AABB around */
	/*  the triangle against the AABB */

	/* test in X-direction */
	FINDMINMAX(v0[X],v1[X],v2[X],min,max);
	if(min>boxhalfsize[X] || max<-boxhalfsize[X]) return 0;

	/* test in Y-direction */
	FINDMINMAX(v0[Y],v1[Y],v2[Y],min,max);
	if(min>boxhalfsize[Y] || max<-boxhalfsize[Y]) return 0;

	/* test in Z-direction */
	FINDMINMAX(v0[Z],v1[Z],v2[Z],min,max);
	if(min>boxhalfsize[Z] || max<-boxhalfsize[Z]) return 0;

	/* Bullet 2: */
	/*  test if the box intersects the plane of the triangle */
	/*  compute plane equation of triangle: normal*x+d=0 */
	CROSS(normal,e0,e1);
	// -NJMP- (line removed here)
	if(!planeBoxOverlap(normal,v0,boxhalfsize)) return 0;	// -NJMP-

	return 1;   /* box and triangle overlaps */
}


Resizer::Resizer()
{
	cellVulnerability = NULL;
	cellIncludingFaceNum = NULL;
	cellIncludingFace = NULL;
	facePassthroughCellNum = NULL;
	facePassthroughCell = NULL;
	cellPhi = NULL;
	W = NULL;
	scaleEstimation = NULL;
	cellScale = NULL;
	oldCellVertex = NULL;
	newCellVertex = NULL;
	vertexInCell = NULL;
}

Resizer::~Resizer()
{
	if(cellVulnerability != NULL)
		delete[] cellVulnerability;     //ò��������ʽֱ����delete[]�ͷż���
	printf("cellVulnerability�ͷ���Դû����.\n");
	if(cellIncludingFaceNum != NULL)
		delete[] cellIncludingFaceNum;
	printf("cellIncludingFaceNum�ͷ���Դû����.\n");
	if(cellIncludingFace != NULL)
		delete[] cellIncludingFace;
	printf("cellIncludingFace�ͷ���Դû����.\n");
	if(facePassthroughCellNum != NULL)
		delete[] facePassthroughCellNum;
	printf("facePassthroughCellNum�ͷ���Դû����.\n");
	if(facePassthroughCell != NULL)
		delete[] facePassthroughCell;
	printf("facePassthroughCell�ͷ���Դû����.\n");
	if(cellPhi != NULL)
	{
		for(int i=0;i<CELLNUM+1;i++)
			for(int j=0;j<CELLNUM+1;j++)
				delete[] cellPhi[i][j];
		for(int i=0;i<CELLNUM+1;i++)
			delete[] cellPhi[i];
		delete[] cellPhi;
	}
	printf("cellPhi�ͷ���Դû����.\n");
	if(cellScale != NULL)
	{
		for(int i=0;i<ALLCELLNUM;i++)
		{
			delete[] cellScale[i];
		}
 		delete[] cellScale;
	}
	printf("cellScale�ͷ���Դû����.\n");
	if(oldCellVertex != NULL)
	{
		for(int i=0;i<(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i++)
		{
			delete[] oldCellVertex[i];
		}
		delete[] oldCellVertex;
	}
	printf("oldCellVertex�ͷ���Դû����.\n");
	if(newCellVertex != NULL)
	{
		for(int i=0;i<(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i++)
		{
			delete[] newCellVertex[i];
		}
		delete[] newCellVertex;
	}
	printf("newCellVertex�ͷ���Դû����.\n");
	if(vertexInCell != NULL)
	{
		int vertexN = polyMesh->getVertexN();
		for(int i=0;i<vertexN;i++)
		{
			delete[] vertexInCell[i];
		}
		delete[] vertexInCell;
	}
	printf("vertexInCell�ͷ���Դû����.\n");
}

void Resizer::setPolyMesh(PolyMesh* _polyMesh)
{
	polyMesh = _polyMesh;
}

void Resizer::computeBoundingBoxAndEdgeLength()
{
	//���ȼ��������ģ�͵ı߽�
	polyMesh->computeBoundingBox();        //���������ģ�͵ı߽�
	//ע����Խ�ģ�͵ı߽���������һЩ��Ϊgrid�ı߽�
	//������������������ķ��ȣ������Զ��壩
	float temp[3];
	for(int i=0;i<3;i++)
	{
		temp[i] = (polyMesh->maxBoundingBox[i]-polyMesh->minBoundingBox[i])/CELLNUM;
	}
	//�����grid�ı߽�
	for(int i=0;i<3;i++)
	{
		minBoundingBox[i] = polyMesh->minBoundingBox[i]-temp[i];
		maxBoundingBox[i] = polyMesh->maxBoundingBox[i]+temp[i];
	}
	//�����edgeLength[]��ֵ
	for(int i=0;i<3;i++)
	{
		edgeLength[i] = (maxBoundingBox[i]-minBoundingBox[i])/CELLNUM;
	}
}

void Resizer::computeFaceBoundingBox(float _vertex[3][3],float _min[3],float _max[3])
{
	_max[0] = _min[0] = _vertex[0][0];
	_max[1] = _min[1] = _vertex[0][1];
	_max[2] = _min[2] = _vertex[0][2];

	//�������еĶ���һ��
	for(int i=1;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			if(_max[j] < _vertex[i][j])
				_max[j] = _vertex[i][j];
			if(_min[j] > _vertex[i][j])
				_min[j] = _vertex[i][j];
		}
	}
}

int Resizer::computeIndex(float _edgeLength,float _coordValue,float _minCoordValue)
{
	//�ų��������
	if(_coordValue < _minCoordValue)    //��ʵ�����ܷ���������дֻ��Ϊ�˽�׳��
	{
		printf("_coordValue is wrong.\n");
		return -2;
	}
	//˳�����
	int i;
	bool flag = false;
	for(i=0;i<CELLNUM;i++)
	{
		if((_coordValue >= _minCoordValue+i*_edgeLength)&&(_coordValue < _minCoordValue+(i+1)*_edgeLength))   //�ҵ����ڷ�����coordValue���ڵ�index
		{
			flag = true;
			break;         //����ѭ��
		}
	}
	if(flag == true)
		return i;
	else
		return -1;
}

int Resizer::computeCellIndex(int _i,int _j,int _k)    //i-->kά������
{
	int index;
	index = _k*CELLLAYER+_j*CELLNUM+_i;
	return index; 
}

int Resizer::computeCellIndex1(int _i,int _j,int _k)
{
	int index;
	index = _k*(CELLNUM+1)*(CELLNUM+1)+_j*(CELLNUM+1)+_i;
	return index; 
}

int Resizer::computeCellIndex2(int _i,int _j,int _k)
{
	int index;
	index = _k*(CELLNUM+2)*(CELLNUM+2)+_j*(CELLNUM+2)+_i;
	return index; 
}


void Resizer::computeNewCellIJK(int cellIndex,int *_i,int *_j,int *_k)   //i-->kά������
{
	//����*_k��ֵ
	*_k = cellIndex/((CELLNUM+1)*(CELLNUM+1));
	*_j = (cellIndex-(*_k)*((CELLNUM+1)*(CELLNUM+1)))/(CELLNUM+1);
	*_i = cellIndex%(CELLNUM+1);
}

void Resizer::computeFacePassthroughingCell(int _faceIndex,float _tempVertex[3][3],int _indexMin[3],int _indexMax[3],float _cellHalfLength[3])
{
	int num = 0;                        //��¼��face������cell�ĸ���
	float cellCenter[3];                //��¼��ǰcell�����ĵ�
	//��ϸ���ǰ������
	if(_indexMax[0] == _indexMin[0])         
	{
		if(_indexMax[1] == _indexMin[1])    
		{
			if(_indexMax[2] == _indexMin[2])    //���X,Y,Z������ͬ���϶����ཻ�ģ�
			{
				facePassthroughCell[_faceIndex][num] = computeCellIndex(_indexMin[0],_indexMin[1],_indexMin[2]);
			//	printf("X��Y��Z������ͬ:%d--%d\n",num,computeCellIndex(_indexMin[0],_indexMin[1],_indexMin[2]));
				num++; 
				facePassthroughCellNum[_faceIndex] = num; 
			//	Sleep(500);
			}
			else             //���X,Y������ͬ,Z����ͬ����Ҫ�ж��Ƿ��ཻ��      
			{
				cellCenter[0] = minBoundingBox[0]+_indexMin[0]*edgeLength[0]+_cellHalfLength[0];
				cellCenter[1] = minBoundingBox[1]+_indexMin[1]*edgeLength[1]+_cellHalfLength[1];
				for(int j=_indexMin[2];j<=_indexMax[2];j++)    //Z��
				{
					cellCenter[2] = minBoundingBox[2]+j*edgeLength[2]+_cellHalfLength[2];
					if(triBoxOverlap(cellCenter,_cellHalfLength,_tempVertex) == 1)      //���face��cell�ཻ
					{
						facePassthroughCell[_faceIndex][num] = computeCellIndex(_indexMin[0],_indexMin[1],j);
					//	printf("X��Y������ͬ,Z����ͬ:%d--%d\n",num,computeCellIndex(_indexMin[0],_indexMin[1],j));
						num++;
					//	Sleep(500);
					}
					/*else
					{
						printf("X��Y������ͬ,Z����ͬ��face��cell���ཻ.\n");
						Sleep(500);
					}*/
				}
				facePassthroughCellNum[_faceIndex] = num;
			}
		}
		else                 //Y������ͬ
		{
			if(_indexMin[2] == _indexMax[2])    //X,Z������ͬ,Y����ͬ
			{
				cellCenter[0] = minBoundingBox[0]+_indexMin[0]*edgeLength[0]+_cellHalfLength[0];
				cellCenter[2] = minBoundingBox[2]+_indexMin[2]*edgeLength[2]+_cellHalfLength[2];
				for(int j=_indexMin[1];j<=_indexMax[1];j++)    //Y��
				{
					cellCenter[1] = minBoundingBox[1]+j*edgeLength[1]+_cellHalfLength[1];
					if(triBoxOverlap(cellCenter,_cellHalfLength,_tempVertex) == 1)      //���face��cell�ཻ
					{
						facePassthroughCell[_faceIndex][num] = computeCellIndex(_indexMin[0],j,_indexMin[2]);
					//	printf("X��Z������ͬ,Y����ͬ:%d--%d\n",num,computeCellIndex(_indexMin[0],j,_indexMin[2]));
						num++;
					//	Sleep(500);
					}
					/*else
					{
						printf("X��Z������ͬ,Y����ͬ��face��cell���ཻ.\n");
						Sleep(500);
					}*/
					facePassthroughCellNum[_faceIndex] = num;
				}
			}
			else           //X������ͬ,Y,Z����ͬ
			{
				cellCenter[0] = minBoundingBox[0]+_indexMin[0]*edgeLength[0]+_cellHalfLength[0];
				for(int j=_indexMin[1];j<=_indexMax[1];j++)           //Y����
				{
					for(int k=_indexMin[2];k<=_indexMax[2];k++)       //Z����
					{
						cellCenter[1] = minBoundingBox[1]+j*edgeLength[1]+_cellHalfLength[1];
						cellCenter[2] = minBoundingBox[2]+k*edgeLength[2]+_cellHalfLength[2];
						if(triBoxOverlap(cellCenter,_cellHalfLength,_tempVertex) == 1)      //���face��cell�ཻ
						{
							facePassthroughCell[_faceIndex][num] = computeCellIndex(_indexMin[0],j,k);
						//	printf("X������ͬ,Y,Z����ͬ:%d--%d\n",num,computeCellIndex(_indexMin[0],j,k));
							num++;
						//	Sleep(500);
						}
						/*else
						{
							printf("X������ͬ,Y,Z����ͬ��face��cell���ཻ.\n");
							Sleep(500);
						}*/
						facePassthroughCellNum[_faceIndex] = num;
					}
				}
			}
		}
	}
	else           //X����ͬ
	{
		if(_indexMin[1] == _indexMax[1])     //Y������ͬ
		{
			if(_indexMin[2] == _indexMax[2])    //Y,Z������ͬ,X����ͬ
			{
				cellCenter[1] = minBoundingBox[1]+_indexMin[1]*edgeLength[1]+_cellHalfLength[1];
				cellCenter[2] = minBoundingBox[2]+_indexMin[2]*edgeLength[2]+_cellHalfLength[2];
				for(int j=_indexMin[0];j<=_indexMax[0];j++)    //X����
				{
					cellCenter[0] = minBoundingBox[0]+j*edgeLength[0]+_cellHalfLength[0];
					if(triBoxOverlap(cellCenter,_cellHalfLength,_tempVertex) == 1)      //���face��cell�ཻ
					{
						facePassthroughCell[_faceIndex][num] = computeCellIndex(j,_indexMin[1],_indexMin[2]);
					//	printf("Y,Z������ͬ,X����ͬ:%d--%d\n",num,computeCellIndex(j,_indexMin[1],_indexMin[2]));
						num++;
					//	Sleep(500);
					}
					/*else
					{
						printf("Y,Z������ͬ,X����ͬ��face��cell���ཻ.\n");
						Sleep(500);
					}*/
				}
				facePassthroughCellNum[_faceIndex] = num;
			}
			else     //Y������ͬ,X,Z����ͬ
			{
				cellCenter[1] = minBoundingBox[1]+_indexMin[1]*edgeLength[1]+_cellHalfLength[1];
				for(int j=_indexMin[0];j<=_indexMax[0];j++)           //X����
				{
					for(int k=_indexMin[2];k<=_indexMax[2];k++)       //Z����
					{
						cellCenter[0] = minBoundingBox[0]+j*edgeLength[0]+_cellHalfLength[0];
						cellCenter[2] = minBoundingBox[2]+k*edgeLength[2]+_cellHalfLength[2];
						if(triBoxOverlap(cellCenter,_cellHalfLength,_tempVertex) == 1)      //���face��cell�ཻ
						{
							facePassthroughCell[_faceIndex][num] = computeCellIndex(j,_indexMin[1],k);
						//	printf("Y������ͬ,X,Z����ͬ:%d--%d\n",num,computeCellIndex(j,_indexMin[1],k));
							num++;
						//	Sleep(500);
						}
						/*else
						{
							printf("Y������ͬ,X,Z����ͬ��face��cell���ཻ.\n");
							Sleep(500);
						}*/
						facePassthroughCellNum[_faceIndex] = num;
					}
				}
			}
		}
		else      //Y����ͬ
		{
			if(_indexMin[2] == _indexMax[2])        //Z������ͬ,X,Y����ͬ
			{
				cellCenter[2] = minBoundingBox[2]+_indexMin[2]*edgeLength[2]+_cellHalfLength[2];
				for(int j=_indexMin[0];j<=_indexMax[0];j++)           //X����
				{
					for(int k=_indexMin[1];k<=_indexMax[1];k++)       //Y����
					{
						cellCenter[0] = minBoundingBox[0]+j*edgeLength[0]+_cellHalfLength[0];
						cellCenter[1] = minBoundingBox[1]+k*edgeLength[1]+_cellHalfLength[1];
						if(triBoxOverlap(cellCenter,_cellHalfLength,_tempVertex) == 1)      //���face��cell�ཻ
						{
							facePassthroughCell[_faceIndex][num] = computeCellIndex(j,k,_indexMin[2]);
						//	printf("Z������ͬ,X,Y����ͬ:%d--%d\n",num,computeCellIndex(j,k,_indexMin[2]));
							num++;
						//	Sleep(500);
						}
						/*else
						{
							printf("Z������ͬ,X,Y����ͬ��face��cell���ཻ.\n");
							Sleep(500);
						}*/
						facePassthroughCellNum[_faceIndex] = num;
					}
				}
			} 
			else        //X,Y,Z���򶼲�ͬ
			{
				for(int j=_indexMin[0];j<=_indexMax[0];j++)   //X��
				{
					for(int k=_indexMin[1];k<=_indexMax[1];k++)   //Y��
					{
						for(int p=_indexMin[2];p<=_indexMax[2];p++)   //Z��
						{
							cellCenter[0] = minBoundingBox[0]+j*edgeLength[0]+_cellHalfLength[0];
							cellCenter[1] = minBoundingBox[1]+k*edgeLength[1]+_cellHalfLength[1];
							cellCenter[2] = minBoundingBox[2]+p*edgeLength[2]+_cellHalfLength[2];
							if(triBoxOverlap(cellCenter,_cellHalfLength,_tempVertex) == 1)      //���face��cell�ཻ
							{
								facePassthroughCell[_faceIndex][num] = computeCellIndex(j,k,p);
							/*	printf("X,Y,Z����ͬ:%d--%d\n",num,computeCellIndex(j,k,p));
								printf("(%f,%f,%f),(%f,%f,%f),(%f,%f,%f)\n",_tempVertex[0][0],_tempVertex[0][1],_tempVertex[0][2],
																			_tempVertex[1][0],_tempVertex[1][1],_tempVertex[1][2],
																			_tempVertex[2][0],_tempVertex[2][1],_tempVertex[2][2]);*/
								num++;
							//	Sleep(1500);
							}
							/*else
							{
								printf("X,Y,Z����ͬ��face��cell���ཻ.\n");
								printf("(%f,%f,%f),(%f,%f,%f),(%f,%f,%f)\n",_tempVertex[0][0],_tempVertex[0][1],_tempVertex[0][2],
								                                            _tempVertex[1][0],_tempVertex[1][1],_tempVertex[1][2],
																			_tempVertex[2][0],_tempVertex[2][1],_tempVertex[2][2]);
								Sleep(1500);
							}*/
							facePassthroughCellNum[_faceIndex] = num;
						}
					}
				}
			}
		}
	}
}

void Resizer::computeCellIncludingFace()
{
	//��ʼ��cellIncludingFaceNum[]����Ԫ��Ϊ0
	for(int i=0;i<ALLCELLNUM;i++)
		cellIncludingFaceNum[i] = 0;
	int faceN = polyMesh->getFaceN();
	//��ÿ����Ϊ���Ƕ���
	for(int i=0;i<faceN;i++)
	{
		for(int j=0;j<facePassthroughCellNum[i];j++)
		{
			int index = facePassthroughCell[i][j];       //��face�Ĵ������е�cell�ı��
			cellIncludingFace[index][cellIncludingFaceNum[index]] = i;
			cellIncludingFaceNum[index]++;
		}
	}
}

void Resizer::computeCellAndFaceRelationship()
{
	float (*vertex)[3] = polyMesh->vertex;     //��ȡ�������������ж�����ά����
	int (*face)[3] = polyMesh->face;           //��ȡ�����������face�ľ����������������
	int faceN = polyMesh->getFaceN();          //��ȡ�����������face�ĸ���
	facePassthroughCell = new int[faceN][200];
	facePassthroughCellNum = new int[faceN];
	cellIncludingFace = new int[ALLCELLNUM][200];
	cellIncludingFaceNum = new int[ALLCELLNUM];
	//�����grid�ı߽��ÿ��cell�������������ⳤ
	this->computeBoundingBoxAndEdgeLength();
	float cellHalfSize[3];                 //��¼����cell�����������ϵı߳���һ��
	for(int i=0;i<3;i++)
		cellHalfSize[i] = edgeLength[i]/2.0f;
	//��faceΪ���ǻ�׼
	for(int i=0;i<faceN;i++)
	{
		//ÿ��face��3������
		float tempVertex[3][3];         //��ʱ��ŵ�ǰface��3������
		for(int j=0;j<3;j++)
		{
			tempVertex[j][0] = vertex[face[i][j]][0];
			tempVertex[j][1] = vertex[face[i][j]][1];
			tempVertex[j][2] = vertex[face[i][j]][2];
		}
		float max[3],min[3];            //��¼tempVertex[][]��������������ֵ����Сֵ
		//�������ǰface�ı߽�
		computeFaceBoundingBox(tempVertex,min,max);
		//����min[]��max[]��grid�ж�Ӧ��cell������������±�
		int indexMin[3];                //��¼min[]��grid�ж�Ӧ��cell������������±�
		int indexMax[3];                //��¼max[]��grid�ж�Ӧ��cell������������±�
		for(int j=0;j<3;j++)            //�ֱ������������
		{
			indexMin[j] = computeIndex(edgeLength[j],min[j],minBoundingBox[j]);
			indexMax[j] = computeIndex(edgeLength[j],max[j],minBoundingBox[j]);
		}
		//��indexMin[3]��indexMax[3]����ȫ��λ�Ŀ���,�Ӷ��������i�����facePassthroughCell[i][]��facePassthroughCellNum[i]��ֵ
		computeFacePassthroughingCell(i,tempVertex,indexMin,indexMax,cellHalfSize);
	}
	//����facePassthroughCell[i][]��facePassthroughCellNum[i]��ֵ�����cellIncludingFace[][]��cellIncludingFaceNum[]��ֵ
	computeCellIncludingFace();
}


void Resizer::computeCellVulnerability()
{
	//Ϊ����cellVulnerability[][]����ռ�
	cellVulnerability = new double[ALLCELLNUM][3];
	//���ȵ���polyMesh�е�computeVulnerability()�����������faceVulnerability[][3]��ֵ
	polyMesh->computeVulnerability();
	//�õ��������faceVulnerability��ֵ
	double (*faceVulnerability)[3] = polyMesh->faceVulnerability;
	//�����face���cell�ཻ��������������cell�ཻ��face��vulnerability������һ����Ϊ��cell��vulnerability��ֵ
	//���û��face���cell�ཻ�����cell��vulnerability��ֵΪ0
	for(int i=0;i<ALLCELLNUM;i++)
	{			
		if(cellIncludingFaceNum[i] == 0)
		{
			cellVulnerability[i][0] = cellVulnerability[i][1] = cellVulnerability[i][2] = 0.0f;
		}
		else
		{
			double maxVulnerability[3];            //��¼���������������ཻ��face��vulnerability�����ֵ
			maxVulnerability[0] = faceVulnerability[cellIncludingFace[i][0]][0];
			maxVulnerability[1] = faceVulnerability[cellIncludingFace[i][0]][1];
			maxVulnerability[2] = faceVulnerability[cellIncludingFace[i][0]][2];
			for(int j=1;j<cellIncludingFaceNum[i];j++)            //���cell�ཻface���ܸ���ΪcellIncludingFaceNum[i]
			{
				for(int k=0;k<3;k++)            //����������
				{
					if(faceVulnerability[cellIncludingFace[i][j]][k] > maxVulnerability[k])
					{
						maxVulnerability[k] = faceVulnerability[cellIncludingFace[i][j]][k];
					}
				}
			}
			//�ó���cell��vulnerability[i][3]��ֵ
			cellVulnerability[i][0] = maxVulnerability[0];
			cellVulnerability[i][1] = maxVulnerability[1];
			cellVulnerability[i][2] = maxVulnerability[2];
		}
	}
}

//����ÿ��cell��phiֵ������������phi��ÿ��Ԫ�ص�ֵ(cellPhi[Z][Y][X])  
//��ԭ��grid��������ұߡ��ϱߡ�ǰ�߸���һ��(�������)
void Resizer::computeCellPhi()
{ 
	//Ϊ����cellPhi[][][]����ռ�
	cellPhi = new double **[(CELLNUM+1)];
	for(int i=0;i<(CELLNUM+1);i++)
		cellPhi[i] = new double *[(CELLNUM+1)];
	for(int i=0;i<(CELLNUM+1);i++)
		for(int j=0;j<(CELLNUM+1);j++)
			cellPhi[i][j] = new double[(CELLNUM+1)];
	//�����cell��ģ���е�face�ཻ����cellPhi[i]ֵΪ1��
	//�����cell��ģ���е�face���ཻ����cellPhi[i]ֵΪ0.1
	for(int k=0;k<CELLNUM+1;k++)    //��Z
	{
		for(int j=0;j<CELLNUM+1;j++)     //��Y
		{
			for(int i=0;i<CELLNUM+1;i++)      //��X                              
			{
				if(i==CELLNUM||j==CELLNUM||k==CELLNUM)       //��ʱΪ�ұߡ��ϱߡ�ǰ�ߵ�һ��
				{
					cellPhi[k][j][i] = 0.0;   
				}
				else
				{
					int cellIndex = computeCellIndex(i,j,k);     //�ó�i,j,k��Ӧ��cell���±�
					if(cellIncludingFaceNum[cellIndex] == 0)     //���ཻ
					{
						cellPhi[k][j][i] = 0.1;
					}
					else                              //�ཻ
					{
						cellPhi[k][j][i] = 1.0;
					}
				}
			}
		}
	}
}

void Resizer::computeCellW()
{      
	//Ϊ����W[][]����ռ�
	W = new double *[ALLCELLNUM];
	for(int i=0;i<ALLCELLNUM;i++)
		W[i] = new double[3];
    //W[0]---XY,W[1]---YZ,W[2]---ZX
	for(int i=0;i<ALLCELLNUM;i++)
	{
		for(int j=0;j<3;j++)          //3������
		{
			W[i][j] = max(min(cellVulnerability[i][j],cellVulnerability[i][(j+1)%3]),1e-4);
		}
	}
}

void Resizer::computeCellScaleEstimation(float _S[3])
{
	//Ϊ����scaleEstimation[][]����ռ�
	scaleEstimation = new double *[ALLCELLNUM];
	for(int i=0;i<ALLCELLNUM;i++)
		scaleEstimation[i] = new double[3];
	for(int i=0;i<ALLCELLNUM;i++)
	{
		for(int j=0;j<3;j++)          //3������
		{
			scaleEstimation[i][j] = cellVulnerability[i][j]+(1.0-cellVulnerability[i][j])*_S[j];
		}
	}
}

//����ÿ��cell�����ŵĴ�С(��Ҫ�Ǽ���cellScale[][]��ֵ)
void Resizer::computeCellScale(float _S[3])
{
	//����ÿ��cell��Phiֵ
	computeCellPhi();
	//����ÿ��cell��W[][3]��scaleEstimation[][3]ֵ
	computeCellW();
	computeCellScaleEstimation(_S);
	//�ȶ�����ʱ�������ÿ��cell��9������
	double **temp = new double *[(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)];
	for(int i=0;i<(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i++)
		temp[i] = new double[9];
	//��temp[][]����ֵΪ0.0
	for(int i=0;i<(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i++)
		for(int j=0;j<9;j++)
			temp[i][j] = 0.0;
	//��temp[][]���¼���
	for(int k=0;k<CELLNUM;k++)
	{
		for(int j=0;j<CELLNUM;j++)
		{
			for(int i=0;i<CELLNUM;i++)
			{
				int cellIndex = computeCellIndex1(i,j,k);
				temp[cellIndex][0] += cellPhi[k][j][i]*cellPhi[k][j+1][i]+cellPhi[k][j][i]*cellPhi[k+1][j][i];         //ScX��ƽ��
				temp[cellIndex][1] += cellPhi[k][j][i]*cellPhi[k][j][i+1]+cellPhi[k][j][i]*cellPhi[k+1][j][i];         //ScY��ƽ��
				temp[cellIndex][2] += cellPhi[k][j][i]*cellPhi[k][j][i+1]+cellPhi[k][j][i]*cellPhi[k][j+1][i];         //ScZ��ƽ��
				temp[cellIndex][3] += -cellPhi[k][j][i]*cellPhi[k][j+1][i];        //ScX*Sd2X
				temp[cellIndex][4] += -cellPhi[k][j][i]*cellPhi[k+1][j][i];        //ScX*Sd3X
				temp[cellIndex][5] += -cellPhi[k][j][i]*cellPhi[k][j][i+1];        //ScY*Sd1Y
				temp[cellIndex][6] += -cellPhi[k][j][i]*cellPhi[k+1][j][i];        //ScY*Sd3Y
				temp[cellIndex][7] += -cellPhi[k][j][i]*cellPhi[k][j][i+1];        //ScZ*Sd1Z
				temp[cellIndex][8] += -cellPhi[k][j][i]*cellPhi[k][j+1][i];        //ScZ*Sd2Z

				//��Sd1���ڵ�λ�ü�������
				cellIndex = computeCellIndex1(i+1,j,k);
				temp[cellIndex][1] += cellPhi[k][j][i]*cellPhi[k][j][i+1];
				temp[cellIndex][2] += cellPhi[k][j][i]*cellPhi[k][j][i+1];

				//��Sd2���ڵ�λ�ü�������
				cellIndex = computeCellIndex1(i,j+1,k);
				temp[cellIndex][0] += cellPhi[k][j][i]*cellPhi[k][j+1][i];
				temp[cellIndex][2] += cellPhi[k][j][i]*cellPhi[k][j+1][i];

				//��Sd3���ڵ�λ�ü�������
				cellIndex = computeCellIndex1(i,j,k+1);
				temp[cellIndex][0] += cellPhi[k][j][i]*cellPhi[k+1][j][i];
				temp[cellIndex][1] += cellPhi[k][j][i]*cellPhi[k+1][j][i];
 			}
		}
	}

	//���ȶ�����������(ע����grid���Ҳࡢ�ϲࡢǰ�������һ�㣬��Ҫ��Ϊ�˷������)
	int m = 3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+3*CELLLAYER;    //���������
	int n = 3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+3*CELLLAYER;    //���������
	//�����з�0Ԫ�ĸ���(ע��������:���������0---3*ALLCELLNUM-1��,ÿ�������8����0Ԫ�أ�3*ALLCELLNUM---ALLCELLNUM+CELLLAYER*3-1�У�ÿ����CELLNUM����0Ԫ��)
	//ע��Ҫ���ǶԽ��ߵ�Ԫ��
	int nnz = 6*ALLCELLNUM+5*ALLCELLNUM+4*ALLCELLNUM;             //ϡ������з�0Ԫ�صĸ���
	//�����������ı���
	taucs_ccs_matrix *pMatrix;            
	//ΪpMatrix����ռ�
	pMatrix = taucs_ccs_create(m,n,nnz,TAUCS_DOUBLE);
	//����pMatrix��һЩ����(�Գƺ�������)
	pMatrix->flags += TAUCS_SYMMETRIC;
	pMatrix->flags += TAUCS_LOWER;

	int num1=0;            //����������Ҫ��Ϊ��¼colptr[]λ���±�
	int num2=0;            //����������Ҫ��Ϊ��¼rowind[]��values.d[]λ���±�
	int count=0;           //��Ҫ��Ϊ��¼colptr[]��ŵ�ֵ       
	//���濪ʼ��ž���(ע�⣺����taucs��ϡ�������밴�н��д洢)
	//���ȿ���X����
	for(int k=0;k<CELLNUM+1;k++)       //��
	{
		for(int j=0;j<CELLNUM+1;j++)        //��
		{
			for(int i=0;i<CELLNUM+1;i++)        //��
			{
				if(i==CELLNUM||j==CELLNUM||k==CELLNUM)    //���ϵĵ�
				{
					//��ʱ���е�Ԫ�ض�Ϊ0
					pMatrix->colptr[num1] = count;
					num1++;
				}
				else     //���ǿ��ϵĵ�
				{
					//colptr[]�洢ÿһ�п�ʼԪ�ض�Ӧ���±�
					//rowind[]�洢ÿ��Ԫ������Ӧ�����е��±�
					//value.d[]�洢ÿ��Ԫ�ص�Ԫ��ֵ��ע��Ҫ��rowind[]���Ӧ
					pMatrix->colptr[num1] = count;
					num1++;
					int cellIndex1 = computeCellIndex(i,j,k);           //���±�����ȡW��scaleEstimation��ֵ
					int cellIndex2 = computeCellIndex1(i,j,k);          //���±�����ȡtemp��ֵ
					pMatrix->rowind[num2] = computeCellIndex1(i,j,k);                                                                                                                                 
					pMatrix->values.d[num2] = 2.0*(W[cellIndex1][0]+W[cellIndex1][2])/(scaleEstimation[cellIndex1][0]*scaleEstimation[cellIndex1][0])+(4.0/3.0)*2.0*temp[cellIndex2][0];
					num2++;
					pMatrix->rowind[num2] = computeCellIndex1(i,j+1,k);
					pMatrix->values.d[num2] = (4.0/3.0)*2.0*temp[cellIndex2][3];
					num2++;
					pMatrix->rowind[num2] = computeCellIndex1(i,j,k+1);
					pMatrix->values.d[num2] = (4.0/3.0)*2.0*temp[cellIndex2][4];
					num2++;
					pMatrix->rowind[num2] = (CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+computeCellIndex1(i,j,k);
					pMatrix->values.d[num2] = -2.0*W[cellIndex1][0]/(scaleEstimation[cellIndex1][0]*scaleEstimation[cellIndex1][1]);
					num2++;
					pMatrix->rowind[num2] = 2*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+computeCellIndex1(i,j,k);
					pMatrix->values.d[num2] = -2.0*W[cellIndex1][2]/(scaleEstimation[cellIndex1][0]*scaleEstimation[cellIndex1][2]);
					num2++;
					pMatrix->rowind[num2] = 3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+k*CELLNUM+j;
					pMatrix->values.d[num2] = 1.0; 
					num2++;
					count = count+6;
				}
			}
		}
	}
	//����Y����num1,num2��countӦ�ý������ֵ��
	for(int k=0;k<CELLNUM+1;k++)       //��
	{
		for(int j=0;j<CELLNUM+1;j++)        //��
		{
			for(int i=0;i<CELLNUM+1;i++)        //��
			{
				if(i==CELLNUM||j==CELLNUM||k==CELLNUM)    //���ϵĵ�
				{
					//��ʱ���е�Ԫ�ض�Ϊ0
					pMatrix->colptr[num1] = count;
					num1++;
				}
				else     //���ǿ��ϵĵ�
				{
					//colptr[]�洢ÿһ�п�ʼԪ�ض�Ӧ���±�	
					//rowind[]�洢ÿ��Ԫ������Ӧ�����е��±�
					//value.d[]�洢ÿ��Ԫ�ص�Ԫ��ֵ��ע��Ҫ��rowind[]���Ӧ
					pMatrix->colptr[num1] = count;
					num1++;
					int cellIndex1 = computeCellIndex(i,j,k);     //i,j,k��Ӧ�Ĳ����ǵ�grid��cell���±�
					int cellIndex2 = computeCellIndex1(i,j,k);
					pMatrix->rowind[num2] = (CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+computeCellIndex1(i,j,k); 
					pMatrix->values.d[num2] = 2.0*(W[cellIndex1][0]+W[cellIndex1][1])/(scaleEstimation[cellIndex1][1]*scaleEstimation[cellIndex1][1])+(4.0/3.0)*2.0*temp[cellIndex2][1];
					num2++;
					pMatrix->rowind[num2] = (CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+computeCellIndex1(i+1,j,k);
					pMatrix->values.d[num2] = (4.0/3.0)*2.0*temp[cellIndex2][5];
					num2++;
					pMatrix->rowind[num2] = (CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+computeCellIndex1(i,j,k+1);
					pMatrix->values.d[num2] = (4.0/3.0)*2.0*temp[cellIndex2][6];
					num2++;
					pMatrix->rowind[num2] = 2*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+computeCellIndex1(i,j,k);
					pMatrix->values.d[num2] = -2.0*W[cellIndex1][1]/(scaleEstimation[cellIndex1][1]*scaleEstimation[cellIndex1][2]);
					num2++;
					pMatrix->rowind[num2] = 3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+CELLLAYER+k*CELLNUM+i;
					pMatrix->values.d[num2] = 1.0;
					num2++;
					count = count+5;
				}
			}
		}
	}
	//����Z����num1,num2��countӦ�ý������ֵ��
	for(int k=0;k<CELLNUM+1;k++)       //��
	{
		for(int j=0;j<CELLNUM+1;j++)        //��
		{
			for(int i=0;i<CELLNUM+1;i++)        //��
			{
				if(i==CELLNUM||j==CELLNUM||k==CELLNUM)    //���ϵĵ�
				{
					//��ʱ���е�Ԫ�ض�Ϊ0
					pMatrix->colptr[num1] = count;
					num1++;
				}
				else     //���ǿ��ϵĵ�
				{
					//colptr[]�洢ÿһ�п�ʼԪ�ض�Ӧ���±�	
					//rowind[]�洢ÿ��Ԫ������Ӧ�����е��±�
					//value.d[]�洢ÿ��Ԫ�ص�Ԫ��ֵ��ע��Ҫ��rowind[]���Ӧ
					pMatrix->colptr[num1] = count;
					num1++;
					int cellIndex1 = computeCellIndex(i,j,k);        //i,j,k��Ӧ�Ĳ����ǵ�grid��cell���±�
					int cellIndex2 = computeCellIndex1(i,j,k);
					pMatrix->rowind[num2] = 2*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+computeCellIndex1(i,j,k);                                                                                                                                 
					pMatrix->values.d[num2] = 2.0*(W[cellIndex1][1]+W[cellIndex1][2])/(scaleEstimation[cellIndex1][2]*scaleEstimation[cellIndex1][2])+(4.0/3.0)*2.0*temp[cellIndex2][2];
					num2++;
					pMatrix->rowind[num2] = 2*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+computeCellIndex1(i+1,j,k);
					pMatrix->values.d[num2] = (4.0/3.0)*2.0*temp[cellIndex2][7];
					num2++;
					pMatrix->rowind[num2] = 2*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+computeCellIndex1(i,j+1,k);
					pMatrix->values.d[num2] = (4.0/3.0)*2.0*temp[cellIndex2][8];
					num2++;
					pMatrix->rowind[num2] = 3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+2*CELLLAYER+j*CELLNUM+i;
					pMatrix->values.d[num2] = 1.0;
					num2++;
					count = count+4;
				}
			}
		}
	}

	//ע�⣺pMatrix->colptr[]����n+1��Ԫ�أ���ʹΪ0��ҲӦ�洢
	for(int i=num1;i<=n;i++)                  //�˴�nӦ��ȡ��
		pMatrix->colptr[i] = count;

	taucs_double *x = new taucs_double[3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+3*CELLLAYER];     //�������õ���δ֪�����              
	//�Ծ���x����ֵ(�ӿ������ٶ�)
	for(int k=0;k<CELLNUM+1;k++)       //��
	{
		for(int j=0;j<CELLNUM+1;j++)        //��
		{
			for(int i=0;i<CELLNUM+1;i++)        //��
			{
				if(i==CELLNUM||j==CELLNUM||k==CELLNUM)     //���ϵĵ�,���Ӧ��x[i]ֵ��ֵΪ0
				{
					int index = computeCellIndex1(i,j,k);        
					x[index] = 0.0;
				}
				else     //���ǿ��ϵĵ�
				{
					int index = computeCellIndex1(i,j,k);
					x[index] = _S[0]; 
					x[(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+index] = _S[1];
					x[2*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+index] = _S[2];
				}
			}
		}
	}
	//����x[]ֵ��ֵΪ0
	for(int i=3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i<3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+3*CELLLAYER;i++)
		x[i] = 0.0;

	taucs_double *b = new taucs_double[3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+3*CELLLAYER];           //����ұߵ��о���
	//�Ծ���b��ֵ
	for(int i=0;i<3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i++)
		b[i] = 0.0;
	for(int i=3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i<3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+CELLLAYER;i++)
		b[i] = CELLNUM*_S[0];
	for(int i=3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+CELLLAYER;i<3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+2*CELLLAYER;i++)
		b[i] = CELLNUM*_S[1];
	for(int i=3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+2*CELLLAYER;i<3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+3*CELLLAYER;i++)
		b[i] = CELLNUM*_S[2];

	//���ú����������
	//ע�⣺ԭ������ʵ��Ҫ����Ԥ���������Ҫ��Ϊ���������
	//��û�м���������㣺1)ʱ����ȣ�����Ԥ��������ֵû�����ʱ�� 2)ֱ�Ӽ��������ٶ�Ҳ�ܿ��
	int result = taucs_minres(pMatrix,NULL,NULL,x,b,1000,1e-4);
	//�жϽ��
	if (result != TAUCS_SUCCESS)
	{
		printf ("Solution error.\n");
		if (result==TAUCS_ERROR)
			printf ("Generic error.");
		if (result==TAUCS_ERROR_NOMEM)
			printf ("NOMEM error.");   
		if (result==TAUCS_ERROR_BADARGS)
			printf ("BADARGS error.");  
		if (result==TAUCS_ERROR_MAXDEPTH)
			printf ("MAXDEPTH error.");   
		if (result==TAUCS_ERROR_INDEFINITE)
			printf ("NOT POSITIVE DEFINITE error.");        
	}
	else
	{
		printf ("Solution success.\n");   
		//ΪcellScale[][3]����ռ�
		cellScale = new double *[ALLCELLNUM];
		for(int i=0;i<ALLCELLNUM;i++)
			cellScale[i] = new double[3];
		//��ȷ��ý⣬���丳ֵ��cellScale[][3]
		for(int k=0;k<CELLNUM+1;k++)       //��
		{
			for(int j=0;j<CELLNUM+1;j++)        //��
			{
				for(int i=0;i<CELLNUM+1;i++)        //��
				{
					if(i==CELLNUM||j==CELLNUM||k==CELLNUM)    //���ϵĵ�,���Ӧ��x[i]ֵ�ǲ���Ҫ��
					{
					}
					else     //���ǿ��ϵĵ�
					{
						int cellIndex1 = computeCellIndex(i,j,k);         //i,j,k��Ӧ�Ĳ����ǵ�grid��cell���±�
						int cellIndex2 = computeCellIndex1(i,j,k);        //i,j,k��Ӧ�Ĵ��ǵ�grid��cell���±�
						cellScale[cellIndex1][0] = x[cellIndex2];
						cellScale[cellIndex1][1] = x[(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+cellIndex2];
						cellScale[cellIndex1][2] = x[2*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+cellIndex2];
					}
				}
			}
		}
	}

	taucs_ccs_free(pMatrix);

	//�˴������ͷ�W[][]��scaleEstimation[][]����Դ��
	for(int i=0;i<ALLCELLNUM;i++)
	{
		delete[] W[i];
	}
	delete[] W;
	for(int i=0;i<ALLCELLNUM;i++)
	{
		delete[] scaleEstimation[i];
	}
	delete[] scaleEstimation;
	printf("W��scaleEstimation�ͷ���Դû����.\n");
	//�ͷ�x[]��b[]����Դ
	delete[] x;
	delete[] b;
	printf("x��b�ͷ���Դû����.\n");
}

//���Լ�����ԭ����grid���ҡ��ϡ�ǰ�������������һ��cell��Ϊ�˷�����㣩
void Resizer::computeOldCellVertex()
{
	//Ϊ����oldCellVertex[][3]����ռ�
	oldCellVertex = new double *[(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)];
	for(int i=0;i<(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i++)
		oldCellVertex[i] = new double[3];
	//��ʼ����oldCellVertex[][3]��ֵ
	for(int i=0;i<(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i++)
	{
		int xx,yy,zz;
		computeNewCellIJK(i,&xx,&yy,&zz);      //(CELLNUM+1)
		//X����
		oldCellVertex[i][0] = minBoundingBox[0] + xx*edgeLength[0];
		//Y����
		oldCellVertex[i][1] = minBoundingBox[1] + yy*edgeLength[1];
		//Z����
		oldCellVertex[i][2] = minBoundingBox[2] + zz*edgeLength[2];
	}
}
            
//����������ܱȽ���
void Resizer::computeNewCellVertex()
{
	double ***temp1 = new double **[3]; 
	for(int i=0;i<3;i++)
		temp1[i] = new double *[(CELLNUM+2)*(CELLNUM+2)*(CELLNUM+2)];
	for(int i=0;i<3;i++)
		for(int j=0;j<(CELLNUM+2)*(CELLNUM+2)*(CELLNUM+2);j++)
			temp1[i][j] = new double[4];
	double **temp2 = new double *[3];
	for(int i=0;i<3;i++)
		temp2[i] = new double[(CELLNUM+2)*(CELLNUM+2)*(CELLNUM+2)];

	//Ϊ����newCellVertex[][]����ռ�
	newCellVertex = new double *[(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)];
	for(int i=0;i<(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i++)
		newCellVertex[i] = new double[3];

	//��ʼ������Ԫ��Ϊ0
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<(CELLNUM+2)*(CELLNUM+2)*(CELLNUM+2);j++)
		{
			temp2[i][j] = 0.0;
			for(int k=0;k<4;k++)
				temp1[i][j][k] = 0.0;
		}
	}
	//��ÿ��cell���б�����ÿ�ο���8������
	for(int p=0;p<3;p++)
	{
		for(int k=0;k<CELLNUM;k++)            //��
		{
			for(int j=0;j<CELLNUM;j++)          //��
			{ 
				for(int i=0;i<CELLNUM;i++)        //��
				{
					int cellIndex = computeCellIndex(i,j,k);         //(CELLNUM)
					//��ǰcell��X����Ĵ�����
					double tempVulnerability = cellVulnerability[cellIndex][p];
					//��ǰcell�ĵ�Phiֵ
					double tempPhi = cellPhi[k][j][i];
					//��ǰcell��X��������Ŵ�С
					double tempScale = cellScale[cellIndex][p];
					//��ǰcell��X�������ź���ⳤ
					double t = tempScale*edgeLength[p];
					//����ÿ��cell���д���
					//�Ե�0������
					cellIndex = computeCellIndex2(i,j,k);
					temp1[p][cellIndex][0] += 2.0*tempPhi+tempVulnerability;
					temp1[p][cellIndex][1] += -tempVulnerability;
					temp1[p][cellIndex][2] += -tempPhi;
					temp1[p][cellIndex][3] += -tempPhi;
					temp2[p][cellIndex] += -2.0*tempVulnerability*t;
					//�Ե�1������
					cellIndex = computeCellIndex2(i+1,j,k);
					temp1[p][cellIndex][0] += 2.0*tempPhi+tempVulnerability;
					temp1[p][cellIndex][2] += -tempPhi;
					temp1[p][cellIndex][3] += -tempPhi;
					temp2[p][cellIndex] += 2.0*tempVulnerability*t;
					//�Ե�2������
					cellIndex = computeCellIndex2(i,j+1,k);
					temp1[p][cellIndex][0] += 2.0*tempPhi+tempVulnerability;
					temp1[p][cellIndex][1] += -tempVulnerability;
					temp1[p][cellIndex][3] += -tempPhi;		
					temp2[p][cellIndex] += -2.0*tempVulnerability*t;
					//�Ե�3������
					cellIndex = computeCellIndex2(i+1,j+1,k);
					temp1[p][cellIndex][0] += 2.0*tempPhi+tempVulnerability;
					temp1[p][cellIndex][3] += -tempPhi;
					temp2[p][cellIndex] += 2.0*tempVulnerability*t;
					//�Ե�4������
					cellIndex = computeCellIndex2(i,j,k+1);
					temp1[p][cellIndex][0] += 2.0*tempPhi+tempVulnerability;
					temp1[p][cellIndex][1] += -tempVulnerability;
					temp1[p][cellIndex][2] += -tempPhi;
					temp2[p][cellIndex] += -2.0*tempVulnerability*t;
					//�Ե�5������
					cellIndex = computeCellIndex2(i+1,j,k+1);
					temp1[p][cellIndex][0] += 2.0*tempPhi+tempVulnerability;
					temp1[p][cellIndex][2] += -tempPhi;
					temp2[p][cellIndex] += 2.0*tempVulnerability*t;
					//�Ե�6������
					cellIndex = computeCellIndex2(i,j+1,k+1);
					temp1[p][cellIndex][0] += 2.0*tempPhi+tempVulnerability;
					temp1[p][cellIndex][1] += -tempVulnerability;
					temp2[p][cellIndex] += -2.0*tempVulnerability*t;
					//�Ե�7������
					cellIndex = computeCellIndex2(i+1,j+1,k+1);
					temp1[p][cellIndex][0] += 2.0*tempPhi+tempVulnerability;
					temp2[p][cellIndex] += 2.0*tempVulnerability*t;
				}
			}
		}
	}

	//���ȶ�������ϡ��Գƾ���
	int m = (CELLNUM+2)*(CELLNUM+2)*(CELLNUM+2);                    //���������
	int n = (CELLNUM+2)*(CELLNUM+2)*(CELLNUM+2);                    //���������
	//ϡ����������еķ�0Ԫ�ص��ܸ���
	int nnz = 4*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);
	//�����������ı���
	taucs_ccs_matrix *pMatrix[3];            
	//ΪpMatrix[]����ռ�
	for(int i=0;i<3;i++)
		pMatrix[i] = taucs_ccs_create(m,n,nnz,TAUCS_DOUBLE);
	//����pMatrix��һЩ����(�Գƺ�������)
	for(int i=0;i<3;i++)
	{
		pMatrix[i]->flags += TAUCS_SYMMETRIC;
		pMatrix[i]->flags += TAUCS_LOWER;
	}

	//X,Y,Z�����ۺϿ���
	for(int p=0;p<3;p++)
	{
		int num=0;                 //����������Ҫ��Ϊ��¼rowind[]��values.d[]λ���±�
		int count=0;               //��Ҫ��Ϊ��¼colptr[]��ŵ�ֵ 
		for(int k=0;k<(CELLNUM+2);k++)       //��
		{
			for(int j=0;j<(CELLNUM+2);j++)        //��
			{
				for(int i=0;i<(CELLNUM+2);i++)        //��
				{
					if(i==CELLNUM+1||j==CELLNUM+1||k==CELLNUM+1)
					{
						int cellIndex = computeCellIndex2(i,j,k);     
						pMatrix[p]->colptr[cellIndex] = count;
					}
					else     
					{
						int cellIndex = computeCellIndex2(i,j,k);     
						pMatrix[p]->colptr[cellIndex] = count;
						pMatrix[p]->rowind[num] = cellIndex;
						pMatrix[p]->values.d[num] = 2.0*temp1[p][cellIndex][0];
						num++;
						pMatrix[p]->rowind[num] = computeCellIndex2(i+1,j,k);
						pMatrix[p]->values.d[num] = 2.0*temp1[p][cellIndex][1];
						num++;
						pMatrix[p]->rowind[num] = computeCellIndex2(i,j+1,k);
						pMatrix[p]->values.d[num] = 2.0*temp1[p][cellIndex][2];
						num++;
						pMatrix[p]->rowind[num] = computeCellIndex2(i,j,k+1);
						pMatrix[p]->values.d[num] = 2.0*temp1[p][cellIndex][3];
						num++;
						count = count+4;
					}
				}
			}
		}
		pMatrix[p]->colptr[(CELLNUM+2)*(CELLNUM+2)*(CELLNUM+2)] = count;
	}

	taucs_double **x = new taucs_double *[3];                //�������õ���δ֪�����
	for(int i=0;i<3;i++)
		x[i] = new taucs_double[(CELLNUM+2)*(CELLNUM+2)*(CELLNUM+2)];
	//�Ծ���x[]���и�ֵ(�Ա��������)
	for(int p=0;p<3;p++)
	{
		for(int k=0;k<(CELLNUM+2);k++)            //��
		{
			for(int j=0;j<(CELLNUM+2);j++)          //��
			{ 
				for(int i=0;i<(CELLNUM+2);i++)        //��
				{
					if(i>CELLNUM||j>CELLNUM||k>CELLNUM)    
					{
						int cellIndex = computeCellIndex2(i,j,k);
						x[p][cellIndex] = 0.0;
					}
					else
					{
						if(p==0)
						{
							int cellIndex = computeCellIndex2(i,j,k);
							x[p][cellIndex] = oldCellVertex[computeCellIndex1(i,j,k)][p];
						}
						else if(p==1)
						{
							int cellIndex = computeCellIndex2(j,i,k);    //�˴�ע��
							x[p][cellIndex] = oldCellVertex[computeCellIndex1(i,j,k)][p];
						}
						else     //p==2
						{
							int cellIndex = computeCellIndex2(k,i,j);    //�˴�ע��
							x[p][cellIndex] = oldCellVertex[computeCellIndex1(i,j,k)][p];
						}
					}
				}
			}
		}
	}

	taucs_double **b = new taucs_double *[3];                //����ұߵ��о���
	for(int i=0;i<3;i++)
		b[i] = new taucs_double[(CELLNUM+2)*(CELLNUM+2)*(CELLNUM+2)];
	//�Ծ���b[]���и�ֵ
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<(CELLNUM+2)*(CELLNUM+2)*(CELLNUM+2);j++)
		{
			b[i][j] = temp2[i][j];
		}
	}
	//�ȶ�X������ú���������� 
	for(int p=0;p<3;p++)
	{
		int result = taucs_minres(pMatrix[p],NULL,NULL,x[p],b[p],1000,0.0001);
		//�жϽ��
		if (result != TAUCS_SUCCESS)
		{  
			printf ("Solution error.\n");
			if (result==TAUCS_ERROR)
				printf ("Generic error.");
			if (result==TAUCS_ERROR_NOMEM)
				printf ("NOMEM error.");   
			if (result==TAUCS_ERROR_BADARGS)
				printf ("BADARGS error.");  
			if (result==TAUCS_ERROR_MAXDEPTH)
				printf ("MAXDEPTH error.");   
			if (result==TAUCS_ERROR_INDEFINITE)
				printf ("NOT POSITIVE DEFINITE error.");        
		}
		else
		{
			printf ("Solution success.\n");   
			//��ȷ��ý⣬���丳ֵ��newCellVertex[][]
			for(int k=0;k<(CELLNUM+2);k++)            //��
			{
				for(int j=0;j<(CELLNUM+2);j++)          //��
				{ 
					for(int i=0;i<(CELLNUM+2);i++)        //��
					{
						if(i>CELLNUM||j>CELLNUM||k>CELLNUM)    
						{
							//�ⲻ������Ҫ��
						}
						else
						{
							if(p==0)
							{
								int cellIndex = computeCellIndex2(i,j,k);
								newCellVertex[computeCellIndex1(i,j,k)][p] = x[p][cellIndex];
							}
							else if(p==1)
							{
								int cellIndex = computeCellIndex2(j,i,k);    //�˴�ע��
								newCellVertex[computeCellIndex1(i,j,k)][p] = x[p][cellIndex];
							}
							else     //p==2
							{
								int cellIndex = computeCellIndex2(k,i,j);    //�˴�ע��
								newCellVertex[computeCellIndex1(i,j,k)][p] = x[p][cellIndex];
							}
						}
					}
				}
			}
		}
	}

	for(int i=0;i<200;i++)
	{
		printf("(%f,%f,%f)<---->(%f,%f,%f)\n",oldCellVertex[i][0],oldCellVertex[i][1],oldCellVertex[i][2],newCellVertex[i][0],newCellVertex[i][1],newCellVertex[i][2]);
	}
	//�ͷ���Դ
	//�ͷ�temp1[][][]
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<(CELLNUM+2)*(CELLNUM+2)*(CELLNUM+2);j++)
		{
			delete[] temp1[i][j];
		}
	}
	for(int i=0;i<3;i++)
	{
		delete[] temp1[i];
	}
	delete[] temp1;
	printf("temp1�ͷ���Դû����.\n");
	//�ͷ�temp2[][]
	for(int i=0;i<3;i++)
	{
		delete[] temp2[i];
	}
	delete[] temp2;
	printf("temp2�ͷ���Դû����.\n");
	//�ͷ�x[][]
	for(int i=0;i<3;i++)
	{
		delete[] x[i];
	}
	delete[] x;
	printf("x�ͷ���Դû����.\n");
	//�ͷ�b[][]
	for(int i=0;i<3;i++)
	{
		delete[] b[i];
	}
	delete[] b;
	printf("b�ͷ���Դû����.\n");
	for(int i=0;i<3;i++)
		taucs_ccs_free(pMatrix[i]);
	printf("pMatrix�ͷ���Դû����.\n");
}


void Resizer::computeVertexInCell()
{
	int vertexN = polyMesh->getVertexN();
	vertexInCell = new int *[vertexN];
	for(int i=0;i<vertexN;i++)
		vertexInCell[i] = new int[2];
	//�õ�ÿ�������3ά����
	float (*vertex)[3] = polyMesh->vertex;
	for(int i=0;i<vertexN;i++)
	{
		//X������±�
		int xx = computeIndex(edgeLength[0],vertex[i][0],minBoundingBox[0]);
		int yy = computeIndex(edgeLength[1],vertex[i][1],minBoundingBox[1]);
		int zz = computeIndex(edgeLength[2],vertex[i][2],minBoundingBox[2]);
		vertexInCell[i][0] = computeCellIndex(xx,yy,zz);        //CELLNUM
		vertexInCell[i][1] = computeCellIndex1(xx,yy,zz);       //CELLNUM+1
		if(xx < 0 || yy < 0 || zz < 0)
		{
			printf("Something is error��\n");
			return;
		}
	}
}

//����ÿ��vertex���µ���άλ��newVertex
void Resizer::updateVertex()
{
	int vertexN = polyMesh->getVertexN();
	for(int i=0;i<vertexN;i++)            //��ÿ��cell
	{
		int index1 = vertexInCell[i][0];
		int index2 = vertexInCell[i][1];
		//������ɵĶ��㵽cell�ĸ�����ľ���
		double distance[3];
		for(int j=0;j<3;j++)
		{
			distance[j] = polyMesh->vertex[i][j]-oldCellVertex[index2][j];
		}
		//��������ź�Ķ��㵽cell�ĸ�������µľ���
		for(int j=0;j<3;j++)
		{
			distance[j] = distance[j]*cellScale[index1][j];
		}
		//������µĶ�������
		for(int j=0;j<3;j++)
		{
			polyMesh->vertex[i][j] = (float)(newCellVertex[index2][j]+distance[j]);
		}
	}
}

void Resizer::printCellVulnerability()
{
	printf("ÿ��cell��vulnerability��ֵΪ:\n");
	for(int i=0;i<ALLCELLNUM;i++)
	{
		printf("%f  %f  %f\n",cellVulnerability[i][0],cellVulnerability[i][1],cellVulnerability[i][2]);
		Sleep(200);
	}
}

void Resizer::printCellPhi()
{
	printf("ÿ��cell��Phi��ֵΪ:\n");
	for(int i=0;i<CELLNUM+1;i++)
	{
		for(int j=0;j<CELLNUM+1;j++)
		{
			for(int k=0;k<CELLNUM+1;k++)
			{
				printf("%f\n",cellPhi[i][j][k]);
			}
		}
	}
}

void Resizer::printCellScale()
{
	for(int i=0;i<ALLCELLNUM;i++)
	{
		printf("%d  %f  %f  %f\n",i,cellScale[i][0],cellScale[i][1],cellScale[i][2]);
		Sleep(200);
	}
}
