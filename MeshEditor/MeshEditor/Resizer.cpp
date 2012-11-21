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
		delete[] cellVulnerability;     //貌似数组形式直接用delete[]释放即可
	printf("cellVulnerability释放资源没问题.\n");
	if(cellIncludingFaceNum != NULL)
		delete[] cellIncludingFaceNum;
	printf("cellIncludingFaceNum释放资源没问题.\n");
	if(cellIncludingFace != NULL)
		delete[] cellIncludingFace;
	printf("cellIncludingFace释放资源没问题.\n");
	if(facePassthroughCellNum != NULL)
		delete[] facePassthroughCellNum;
	printf("facePassthroughCellNum释放资源没问题.\n");
	if(facePassthroughCell != NULL)
		delete[] facePassthroughCell;
	printf("facePassthroughCell释放资源没问题.\n");
	if(cellPhi != NULL)
	{
		for(int i=0;i<CELLNUM+1;i++)
			for(int j=0;j<CELLNUM+1;j++)
				delete[] cellPhi[i][j];
		for(int i=0;i<CELLNUM+1;i++)
			delete[] cellPhi[i];
		delete[] cellPhi;
	}
	printf("cellPhi释放资源没问题.\n");
	if(cellScale != NULL)
	{
		for(int i=0;i<ALLCELLNUM;i++)
		{
			delete[] cellScale[i];
		}
 		delete[] cellScale;
	}
	printf("cellScale释放资源没问题.\n");
	if(oldCellVertex != NULL)
	{
		for(int i=0;i<(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i++)
		{
			delete[] oldCellVertex[i];
		}
		delete[] oldCellVertex;
	}
	printf("oldCellVertex释放资源没问题.\n");
	if(newCellVertex != NULL)
	{
		for(int i=0;i<(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i++)
		{
			delete[] newCellVertex[i];
		}
		delete[] newCellVertex;
	}
	printf("newCellVertex释放资源没问题.\n");
	if(vertexInCell != NULL)
	{
		int vertexN = polyMesh->getVertexN();
		for(int i=0;i<vertexN;i++)
		{
			delete[] vertexInCell[i];
		}
		delete[] vertexInCell;
	}
	printf("vertexInCell释放资源没问题.\n");
}

void Resizer::setPolyMesh(PolyMesh* _polyMesh)
{
	polyMesh = _polyMesh;
}

void Resizer::computeBoundingBoxAndEdgeLength()
{
	//首先计算出整个模型的边界
	polyMesh->computeBoundingBox();        //计算出整个模型的边界
	//注意可以将模型的边界往外扩大一些作为grid的边界
	//计算三个方向上扩大的幅度（可以自定义）
	float temp[3];
	for(int i=0;i<3;i++)
	{
		temp[i] = (polyMesh->maxBoundingBox[i]-polyMesh->minBoundingBox[i])/CELLNUM;
	}
	//计算出grid的边界
	for(int i=0;i<3;i++)
	{
		minBoundingBox[i] = polyMesh->minBoundingBox[i]-temp[i];
		maxBoundingBox[i] = polyMesh->maxBoundingBox[i]+temp[i];
	}
	//计算出edgeLength[]的值
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

	//遍历所有的顶点一遍
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
	//排除错误情况
	if(_coordValue < _minCoordValue)    //其实不可能发生，这样写只是为了健壮性
	{
		printf("_coordValue is wrong.\n");
		return -2;
	}
	//顺序查找
	int i;
	bool flag = false;
	for(i=0;i<CELLNUM;i++)
	{
		if((_coordValue >= _minCoordValue+i*_edgeLength)&&(_coordValue < _minCoordValue+(i+1)*_edgeLength))   //找到所在方向上coordValue所在的index
		{
			flag = true;
			break;         //结束循环
		}
	}
	if(flag == true)
		return i;
	else
		return -1;
}

int Resizer::computeCellIndex(int _i,int _j,int _k)    //i-->k维数升高
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


void Resizer::computeNewCellIJK(int cellIndex,int *_i,int *_j,int *_k)   //i-->k维数升高
{
	//先求*_k的值
	*_k = cellIndex/((CELLNUM+1)*(CELLNUM+1));
	*_j = (cellIndex-(*_k)*((CELLNUM+1)*(CELLNUM+1)))/(CELLNUM+1);
	*_i = cellIndex%(CELLNUM+1);
}

void Resizer::computeFacePassthroughingCell(int _faceIndex,float _tempVertex[3][3],int _indexMin[3],int _indexMax[3],float _cellHalfLength[3])
{
	int num = 0;                        //记录该face穿过的cell的个数
	float cellCenter[3];                //记录当前cell的中心点
	//仔细考虑八种情况
	if(_indexMax[0] == _indexMin[0])         
	{
		if(_indexMax[1] == _indexMin[1])    
		{
			if(_indexMax[2] == _indexMin[2])    //如果X,Y,Z方向都相同（肯定是相交的）
			{
				facePassthroughCell[_faceIndex][num] = computeCellIndex(_indexMin[0],_indexMin[1],_indexMin[2]);
			//	printf("X，Y，Z方向都相同:%d--%d\n",num,computeCellIndex(_indexMin[0],_indexMin[1],_indexMin[2]));
				num++; 
				facePassthroughCellNum[_faceIndex] = num; 
			//	Sleep(500);
			}
			else             //如果X,Y方向相同,Z方向不同（需要判断是否相交）      
			{
				cellCenter[0] = minBoundingBox[0]+_indexMin[0]*edgeLength[0]+_cellHalfLength[0];
				cellCenter[1] = minBoundingBox[1]+_indexMin[1]*edgeLength[1]+_cellHalfLength[1];
				for(int j=_indexMin[2];j<=_indexMax[2];j++)    //Z轴
				{
					cellCenter[2] = minBoundingBox[2]+j*edgeLength[2]+_cellHalfLength[2];
					if(triBoxOverlap(cellCenter,_cellHalfLength,_tempVertex) == 1)      //如果face和cell相交
					{
						facePassthroughCell[_faceIndex][num] = computeCellIndex(_indexMin[0],_indexMin[1],j);
					//	printf("X，Y方向相同,Z方向不同:%d--%d\n",num,computeCellIndex(_indexMin[0],_indexMin[1],j));
						num++;
					//	Sleep(500);
					}
					/*else
					{
						printf("X，Y方向相同,Z方向不同：face和cell不相交.\n");
						Sleep(500);
					}*/
				}
				facePassthroughCellNum[_faceIndex] = num;
			}
		}
		else                 //Y方向不相同
		{
			if(_indexMin[2] == _indexMax[2])    //X,Z方向相同,Y方向不同
			{
				cellCenter[0] = minBoundingBox[0]+_indexMin[0]*edgeLength[0]+_cellHalfLength[0];
				cellCenter[2] = minBoundingBox[2]+_indexMin[2]*edgeLength[2]+_cellHalfLength[2];
				for(int j=_indexMin[1];j<=_indexMax[1];j++)    //Y轴
				{
					cellCenter[1] = minBoundingBox[1]+j*edgeLength[1]+_cellHalfLength[1];
					if(triBoxOverlap(cellCenter,_cellHalfLength,_tempVertex) == 1)      //如果face和cell相交
					{
						facePassthroughCell[_faceIndex][num] = computeCellIndex(_indexMin[0],j,_indexMin[2]);
					//	printf("X，Z方向相同,Y方向不同:%d--%d\n",num,computeCellIndex(_indexMin[0],j,_indexMin[2]));
						num++;
					//	Sleep(500);
					}
					/*else
					{
						printf("X，Z方向相同,Y方向不同：face和cell不相交.\n");
						Sleep(500);
					}*/
					facePassthroughCellNum[_faceIndex] = num;
				}
			}
			else           //X方向相同,Y,Z方向不同
			{
				cellCenter[0] = minBoundingBox[0]+_indexMin[0]*edgeLength[0]+_cellHalfLength[0];
				for(int j=_indexMin[1];j<=_indexMax[1];j++)           //Y方向
				{
					for(int k=_indexMin[2];k<=_indexMax[2];k++)       //Z方向
					{
						cellCenter[1] = minBoundingBox[1]+j*edgeLength[1]+_cellHalfLength[1];
						cellCenter[2] = minBoundingBox[2]+k*edgeLength[2]+_cellHalfLength[2];
						if(triBoxOverlap(cellCenter,_cellHalfLength,_tempVertex) == 1)      //如果face和cell相交
						{
							facePassthroughCell[_faceIndex][num] = computeCellIndex(_indexMin[0],j,k);
						//	printf("X方向相同,Y,Z方向不同:%d--%d\n",num,computeCellIndex(_indexMin[0],j,k));
							num++;
						//	Sleep(500);
						}
						/*else
						{
							printf("X方向相同,Y,Z方向不同：face和cell不相交.\n");
							Sleep(500);
						}*/
						facePassthroughCellNum[_faceIndex] = num;
					}
				}
			}
		}
	}
	else           //X方向不同
	{
		if(_indexMin[1] == _indexMax[1])     //Y方向相同
		{
			if(_indexMin[2] == _indexMax[2])    //Y,Z方向相同,X方向不同
			{
				cellCenter[1] = minBoundingBox[1]+_indexMin[1]*edgeLength[1]+_cellHalfLength[1];
				cellCenter[2] = minBoundingBox[2]+_indexMin[2]*edgeLength[2]+_cellHalfLength[2];
				for(int j=_indexMin[0];j<=_indexMax[0];j++)    //X方向
				{
					cellCenter[0] = minBoundingBox[0]+j*edgeLength[0]+_cellHalfLength[0];
					if(triBoxOverlap(cellCenter,_cellHalfLength,_tempVertex) == 1)      //如果face和cell相交
					{
						facePassthroughCell[_faceIndex][num] = computeCellIndex(j,_indexMin[1],_indexMin[2]);
					//	printf("Y,Z方向相同,X方向不同:%d--%d\n",num,computeCellIndex(j,_indexMin[1],_indexMin[2]));
						num++;
					//	Sleep(500);
					}
					/*else
					{
						printf("Y,Z方向相同,X方向不同：face和cell不相交.\n");
						Sleep(500);
					}*/
				}
				facePassthroughCellNum[_faceIndex] = num;
			}
			else     //Y方向相同,X,Z方向不同
			{
				cellCenter[1] = minBoundingBox[1]+_indexMin[1]*edgeLength[1]+_cellHalfLength[1];
				for(int j=_indexMin[0];j<=_indexMax[0];j++)           //X方向
				{
					for(int k=_indexMin[2];k<=_indexMax[2];k++)       //Z方向
					{
						cellCenter[0] = minBoundingBox[0]+j*edgeLength[0]+_cellHalfLength[0];
						cellCenter[2] = minBoundingBox[2]+k*edgeLength[2]+_cellHalfLength[2];
						if(triBoxOverlap(cellCenter,_cellHalfLength,_tempVertex) == 1)      //如果face和cell相交
						{
							facePassthroughCell[_faceIndex][num] = computeCellIndex(j,_indexMin[1],k);
						//	printf("Y方向相同,X,Z方向不同:%d--%d\n",num,computeCellIndex(j,_indexMin[1],k));
							num++;
						//	Sleep(500);
						}
						/*else
						{
							printf("Y方向相同,X,Z方向不同：face和cell不相交.\n");
							Sleep(500);
						}*/
						facePassthroughCellNum[_faceIndex] = num;
					}
				}
			}
		}
		else      //Y方向不同
		{
			if(_indexMin[2] == _indexMax[2])        //Z方向相同,X,Y方向不同
			{
				cellCenter[2] = minBoundingBox[2]+_indexMin[2]*edgeLength[2]+_cellHalfLength[2];
				for(int j=_indexMin[0];j<=_indexMax[0];j++)           //X方向
				{
					for(int k=_indexMin[1];k<=_indexMax[1];k++)       //Y方向
					{
						cellCenter[0] = minBoundingBox[0]+j*edgeLength[0]+_cellHalfLength[0];
						cellCenter[1] = minBoundingBox[1]+k*edgeLength[1]+_cellHalfLength[1];
						if(triBoxOverlap(cellCenter,_cellHalfLength,_tempVertex) == 1)      //如果face和cell相交
						{
							facePassthroughCell[_faceIndex][num] = computeCellIndex(j,k,_indexMin[2]);
						//	printf("Z方向相同,X,Y方向不同:%d--%d\n",num,computeCellIndex(j,k,_indexMin[2]));
							num++;
						//	Sleep(500);
						}
						/*else
						{
							printf("Z方向相同,X,Y方向不同：face和cell不相交.\n");
							Sleep(500);
						}*/
						facePassthroughCellNum[_faceIndex] = num;
					}
				}
			} 
			else        //X,Y,Z方向都不同
			{
				for(int j=_indexMin[0];j<=_indexMax[0];j++)   //X轴
				{
					for(int k=_indexMin[1];k<=_indexMax[1];k++)   //Y轴
					{
						for(int p=_indexMin[2];p<=_indexMax[2];p++)   //Z轴
						{
							cellCenter[0] = minBoundingBox[0]+j*edgeLength[0]+_cellHalfLength[0];
							cellCenter[1] = minBoundingBox[1]+k*edgeLength[1]+_cellHalfLength[1];
							cellCenter[2] = minBoundingBox[2]+p*edgeLength[2]+_cellHalfLength[2];
							if(triBoxOverlap(cellCenter,_cellHalfLength,_tempVertex) == 1)      //如果face和cell相交
							{
								facePassthroughCell[_faceIndex][num] = computeCellIndex(j,k,p);
							/*	printf("X,Y,Z方向不同:%d--%d\n",num,computeCellIndex(j,k,p));
								printf("(%f,%f,%f),(%f,%f,%f),(%f,%f,%f)\n",_tempVertex[0][0],_tempVertex[0][1],_tempVertex[0][2],
																			_tempVertex[1][0],_tempVertex[1][1],_tempVertex[1][2],
																			_tempVertex[2][0],_tempVertex[2][1],_tempVertex[2][2]);*/
								num++;
							//	Sleep(1500);
							}
							/*else
							{
								printf("X,Y,Z方向不同：face和cell不相交.\n");
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
	//初始化cellIncludingFaceNum[]所有元素为0
	for(int i=0;i<ALLCELLNUM;i++)
		cellIncludingFaceNum[i] = 0;
	int faceN = polyMesh->getFaceN();
	//以每个面为考虑对象
	for(int i=0;i<faceN;i++)
	{
		for(int j=0;j<facePassthroughCellNum[i];j++)
		{
			int index = facePassthroughCell[i][j];       //该face的穿过所有的cell的编号
			cellIncludingFace[index][cellIncludingFaceNum[index]] = i;
			cellIncludingFaceNum[index]++;
		}
	}
}

void Resizer::computeCellAndFaceRelationship()
{
	float (*vertex)[3] = polyMesh->vertex;     //获取多边形网格的所有顶点三维坐标
	int (*face)[3] = polyMesh->face;           //获取多边形网格中face的具体三个顶点的坐标
	int faceN = polyMesh->getFaceN();          //获取多边形网格中face的个数
	facePassthroughCell = new int[faceN][200];
	facePassthroughCellNum = new int[faceN];
	cellIncludingFace = new int[ALLCELLNUM][200];
	cellIncludingFaceNum = new int[ALLCELLNUM];
	//计算出grid的边界和每个cell的三个方向上棱长
	this->computeBoundingBoxAndEdgeLength();
	float cellHalfSize[3];                 //记录所有cell在三个方向上的边长的一半
	for(int i=0;i<3;i++)
		cellHalfSize[i] = edgeLength[i]/2.0f;
	//以face为考虑基准
	for(int i=0;i<faceN;i++)
	{
		//每个face有3个顶点
		float tempVertex[3][3];         //临时存放当前face的3个顶点
		for(int j=0;j<3;j++)
		{
			tempVertex[j][0] = vertex[face[i][j]][0];
			tempVertex[j][1] = vertex[face[i][j]][1];
			tempVertex[j][2] = vertex[face[i][j]][2];
		}
		float max[3],min[3];            //记录tempVertex[][]在三个方向的最大值和最小值
		//计算出当前face的边界
		computeFaceBoundingBox(tempVertex,min,max);
		//计算min[]和max[]在grid中对应的cell的三个方向的下标
		int indexMin[3];                //记录min[]在grid中对应的cell的三个方向的下标
		int indexMax[3];                //记录max[]在grid中对应的cell的三个方向的下标
		for(int j=0;j<3;j++)            //分别计算三个方向
		{
			indexMin[j] = computeIndex(edgeLength[j],min[j],minBoundingBox[j]);
			indexMax[j] = computeIndex(edgeLength[j],max[j],minBoundingBox[j]);
		}
		//对indexMin[3]和indexMax[3]进行全方位的考虑,从而计算出第i个面的facePassthroughCell[i][]和facePassthroughCellNum[i]的值
		computeFacePassthroughingCell(i,tempVertex,indexMin,indexMax,cellHalfSize);
	}
	//根据facePassthroughCell[i][]和facePassthroughCellNum[i]的值计算出cellIncludingFace[][]和cellIncludingFaceNum[]的值
	computeCellIncludingFace();
}


void Resizer::computeCellVulnerability()
{
	//为变量cellVulnerability[][]申请空间
	cellVulnerability = new double[ALLCELLNUM][3];
	//首先调用polyMesh中的computeVulnerability()函数，计算出faceVulnerability[][3]的值
	polyMesh->computeVulnerability();
	//得到所有面的faceVulnerability的值
	double (*faceVulnerability)[3] = polyMesh->faceVulnerability;
	//如果有face与该cell相交，则求出所有与该cell相交的face的vulnerability中最大的一个作为该cell的vulnerability的值
	//如果没有face与该cell相交，则该cell的vulnerability赋值为0
	for(int i=0;i<ALLCELLNUM;i++)
	{			
		if(cellIncludingFaceNum[i] == 0)
		{
			cellVulnerability[i][0] = cellVulnerability[i][1] = cellVulnerability[i][2] = 0.0f;
		}
		else
		{
			double maxVulnerability[3];            //记录三个方向上所有相交的face中vulnerability的最大值
			maxVulnerability[0] = faceVulnerability[cellIncludingFace[i][0]][0];
			maxVulnerability[1] = faceVulnerability[cellIncludingFace[i][0]][1];
			maxVulnerability[2] = faceVulnerability[cellIncludingFace[i][0]][2];
			for(int j=1;j<cellIncludingFaceNum[i];j++)            //与该cell相交face的总个数为cellIncludingFaceNum[i]
			{
				for(int k=0;k<3;k++)            //三个方向上
				{
					if(faceVulnerability[cellIncludingFace[i][j]][k] > maxVulnerability[k])
					{
						maxVulnerability[k] = faceVulnerability[cellIncludingFace[i][j]][k];
					}
				}
			}
			//得出该cell的vulnerability[i][3]的值
			cellVulnerability[i][0] = maxVulnerability[0];
			cellVulnerability[i][1] = maxVulnerability[1];
			cellVulnerability[i][2] = maxVulnerability[2];
		}
	}
}

//计算每个cell的phi值，即计算数组phi中每个元素的值(cellPhi[Z][Y][X])  
//在原来grid的外面的右边、上边、前边各加一层(方便计算)
void Resizer::computeCellPhi()
{ 
	//为变量cellPhi[][][]申请空间
	cellPhi = new double **[(CELLNUM+1)];
	for(int i=0;i<(CELLNUM+1);i++)
		cellPhi[i] = new double *[(CELLNUM+1)];
	for(int i=0;i<(CELLNUM+1);i++)
		for(int j=0;j<(CELLNUM+1);j++)
			cellPhi[i][j] = new double[(CELLNUM+1)];
	//如果该cell与模型中的face相交，则cellPhi[i]值为1；
	//如果该cell与模型中的face不相交，则cellPhi[i]值为0.1
	for(int k=0;k<CELLNUM+1;k++)    //高Z
	{
		for(int j=0;j<CELLNUM+1;j++)     //列Y
		{
			for(int i=0;i<CELLNUM+1;i++)      //行X                              
			{
				if(i==CELLNUM||j==CELLNUM||k==CELLNUM)       //此时为右边、上边、前边的一层
				{
					cellPhi[k][j][i] = 0.0;   
				}
				else
				{
					int cellIndex = computeCellIndex(i,j,k);     //得出i,j,k对应的cell的下标
					if(cellIncludingFaceNum[cellIndex] == 0)     //不相交
					{
						cellPhi[k][j][i] = 0.1;
					}
					else                              //相交
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
	//为变量W[][]申请空间
	W = new double *[ALLCELLNUM];
	for(int i=0;i<ALLCELLNUM;i++)
		W[i] = new double[3];
    //W[0]---XY,W[1]---YZ,W[2]---ZX
	for(int i=0;i<ALLCELLNUM;i++)
	{
		for(int j=0;j<3;j++)          //3个方向
		{
			W[i][j] = max(min(cellVulnerability[i][j],cellVulnerability[i][(j+1)%3]),1e-4);
		}
	}
}

void Resizer::computeCellScaleEstimation(float _S[3])
{
	//为变量scaleEstimation[][]申请空间
	scaleEstimation = new double *[ALLCELLNUM];
	for(int i=0;i<ALLCELLNUM;i++)
		scaleEstimation[i] = new double[3];
	for(int i=0;i<ALLCELLNUM;i++)
	{
		for(int j=0;j<3;j++)          //3个方向
		{
			scaleEstimation[i][j] = cellVulnerability[i][j]+(1.0-cellVulnerability[i][j])*_S[j];
		}
	}
}

//计算每个cell的缩放的大小(主要是计算cellScale[][]的值)
void Resizer::computeCellScale(float _S[3])
{
	//计算每个cell的Phi值
	computeCellPhi();
	//计算每个cell的W[][3]和scaleEstimation[][3]值
	computeCellW();
	computeCellScaleEstimation(_S);
	//先定义临时变量存放每个cell的9个变量
	double **temp = new double *[(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)];
	for(int i=0;i<(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i++)
		temp[i] = new double[9];
	//对temp[][]赋初值为0.0
	for(int i=0;i<(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i++)
		for(int j=0;j<9;j++)
			temp[i][j] = 0.0;
	//对temp[][]重新计算
	for(int k=0;k<CELLNUM;k++)
	{
		for(int j=0;j<CELLNUM;j++)
		{
			for(int i=0;i<CELLNUM;i++)
			{
				int cellIndex = computeCellIndex1(i,j,k);
				temp[cellIndex][0] += cellPhi[k][j][i]*cellPhi[k][j+1][i]+cellPhi[k][j][i]*cellPhi[k+1][j][i];         //ScX的平方
				temp[cellIndex][1] += cellPhi[k][j][i]*cellPhi[k][j][i+1]+cellPhi[k][j][i]*cellPhi[k+1][j][i];         //ScY的平方
				temp[cellIndex][2] += cellPhi[k][j][i]*cellPhi[k][j][i+1]+cellPhi[k][j][i]*cellPhi[k][j+1][i];         //ScZ的平方
				temp[cellIndex][3] += -cellPhi[k][j][i]*cellPhi[k][j+1][i];        //ScX*Sd2X
				temp[cellIndex][4] += -cellPhi[k][j][i]*cellPhi[k+1][j][i];        //ScX*Sd3X
				temp[cellIndex][5] += -cellPhi[k][j][i]*cellPhi[k][j][i+1];        //ScY*Sd1Y
				temp[cellIndex][6] += -cellPhi[k][j][i]*cellPhi[k+1][j][i];        //ScY*Sd3Y
				temp[cellIndex][7] += -cellPhi[k][j][i]*cellPhi[k][j][i+1];        //ScZ*Sd1Z
				temp[cellIndex][8] += -cellPhi[k][j][i]*cellPhi[k][j+1][i];        //ScZ*Sd2Z

				//对Sd1所在的位置加上两项
				cellIndex = computeCellIndex1(i+1,j,k);
				temp[cellIndex][1] += cellPhi[k][j][i]*cellPhi[k][j][i+1];
				temp[cellIndex][2] += cellPhi[k][j][i]*cellPhi[k][j][i+1];

				//对Sd2所在的位置加上两项
				cellIndex = computeCellIndex1(i,j+1,k);
				temp[cellIndex][0] += cellPhi[k][j][i]*cellPhi[k][j+1][i];
				temp[cellIndex][2] += cellPhi[k][j][i]*cellPhi[k][j+1][i];

				//对Sd3所在的位置加上两项
				cellIndex = computeCellIndex1(i,j,k+1);
				temp[cellIndex][0] += cellPhi[k][j][i]*cellPhi[k+1][j][i];
				temp[cellIndex][1] += cellPhi[k][j][i]*cellPhi[k+1][j][i];
 			}
		}
	}

	//首先定义整个矩阵(注意在grid的右侧、上侧、前侧各加了一层，主要是为了方便计算)
	int m = 3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+3*CELLLAYER;    //矩阵的行数
	int n = 3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+3*CELLLAYER;    //矩阵的列数
	//矩阵中非0元的个数(注意计算过程:整个矩阵的0---3*ALLCELLNUM-1行,每行有最多8个非0元素；3*ALLCELLNUM---ALLCELLNUM+CELLLAYER*3-1行，每行有CELLNUM个非0元素)
	//注意要考虑对角线的元素
	int nnz = 6*ALLCELLNUM+5*ALLCELLNUM+4*ALLCELLNUM;             //稀疏矩阵中非0元素的个数
	//存放整个矩阵的变量
	taucs_ccs_matrix *pMatrix;            
	//为pMatrix申请空间
	pMatrix = taucs_ccs_create(m,n,nnz,TAUCS_DOUBLE);
	//设置pMatrix的一些属性(对称和下三角)
	pMatrix->flags += TAUCS_SYMMETRIC;
	pMatrix->flags += TAUCS_LOWER;

	int num1=0;            //计数器，主要是为记录colptr[]位置下标
	int num2=0;            //计数器，主要是为记录rowind[]和values.d[]位置下标
	int count=0;           //主要是为记录colptr[]存放的值       
	//下面开始存放矩阵(注意：调用taucs库稀疏矩阵必须按列进行存储)
	//首先考虑X方向
	for(int k=0;k<CELLNUM+1;k++)       //高
	{
		for(int j=0;j<CELLNUM+1;j++)        //列
		{
			for(int i=0;i<CELLNUM+1;i++)        //行
			{
				if(i==CELLNUM||j==CELLNUM||k==CELLNUM)    //壳上的点
				{
					//此时该行的元素都为0
					pMatrix->colptr[num1] = count;
					num1++;
				}
				else     //不是壳上的点
				{
					//colptr[]存储每一列开始元素对应的下标
					//rowind[]存储每个元素在相应的列中的下标
					//value.d[]存储每个元素的元素值，注意要与rowind[]相对应
					pMatrix->colptr[num1] = count;
					num1++;
					int cellIndex1 = computeCellIndex(i,j,k);           //此下标用于取W和scaleEstimation的值
					int cellIndex2 = computeCellIndex1(i,j,k);          //此下标用于取temp的值
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
	//处理Y方向（num1,num2和count应该接上面的值）
	for(int k=0;k<CELLNUM+1;k++)       //高
	{
		for(int j=0;j<CELLNUM+1;j++)        //列
		{
			for(int i=0;i<CELLNUM+1;i++)        //行
			{
				if(i==CELLNUM||j==CELLNUM||k==CELLNUM)    //壳上的点
				{
					//此时该行的元素都为0
					pMatrix->colptr[num1] = count;
					num1++;
				}
				else     //不是壳上的点
				{
					//colptr[]存储每一列开始元素对应的下标	
					//rowind[]存储每个元素在相应的列中的下标
					//value.d[]存储每个元素的元素值，注意要与rowind[]相对应
					pMatrix->colptr[num1] = count;
					num1++;
					int cellIndex1 = computeCellIndex(i,j,k);     //i,j,k对应的不带壳的grid中cell的下标
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
	//处理Z方向（num1,num2和count应该接上面的值）
	for(int k=0;k<CELLNUM+1;k++)       //高
	{
		for(int j=0;j<CELLNUM+1;j++)        //列
		{
			for(int i=0;i<CELLNUM+1;i++)        //行
			{
				if(i==CELLNUM||j==CELLNUM||k==CELLNUM)    //壳上的点
				{
					//此时该行的元素都为0
					pMatrix->colptr[num1] = count;
					num1++;
				}
				else     //不是壳上的点
				{
					//colptr[]存储每一列开始元素对应的下标	
					//rowind[]存储每个元素在相应的列中的下标
					//value.d[]存储每个元素的元素值，注意要与rowind[]相对应
					pMatrix->colptr[num1] = count;
					num1++;
					int cellIndex1 = computeCellIndex(i,j,k);        //i,j,k对应的不带壳的grid中cell的下标
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

	//注意：pMatrix->colptr[]共有n+1个元素，即使为0，也应存储
	for(int i=num1;i<=n;i++)                  //此处n应能取到
		pMatrix->colptr[i] = count;

	taucs_double *x = new taucs_double[3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+3*CELLLAYER];     //存放运算得到的未知数结果              
	//对矩阵x赋初值(加快收敛速度)
	for(int k=0;k<CELLNUM+1;k++)       //高
	{
		for(int j=0;j<CELLNUM+1;j++)        //列
		{
			for(int i=0;i<CELLNUM+1;i++)        //行
			{
				if(i==CELLNUM||j==CELLNUM||k==CELLNUM)     //壳上的点,其对应的x[i]值赋值为0
				{
					int index = computeCellIndex1(i,j,k);        
					x[index] = 0.0;
				}
				else     //不是壳上的点
				{
					int index = computeCellIndex1(i,j,k);
					x[index] = _S[0]; 
					x[(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+index] = _S[1];
					x[2*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+index] = _S[2];
				}
			}
		}
	}
	//其余x[]值赋值为0
	for(int i=3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i<3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+3*CELLLAYER;i++)
		x[i] = 0.0;

	taucs_double *b = new taucs_double[3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+3*CELLLAYER];           //存放右边的列矩阵
	//对矩阵b赋值
	for(int i=0;i<3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i++)
		b[i] = 0.0;
	for(int i=3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i<3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+CELLLAYER;i++)
		b[i] = CELLNUM*_S[0];
	for(int i=3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+CELLLAYER;i<3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+2*CELLLAYER;i++)
		b[i] = CELLNUM*_S[1];
	for(int i=3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+2*CELLLAYER;i<3*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+3*CELLLAYER;i++)
		b[i] = CELLNUM*_S[2];

	//调用函数解决问题
	//注意：原论文其实是要计算预处理矩阵，主要是为了早点收敛
	//我没有计算基于两点：1)时间紧迫，计算预处理矩阵又得花不少时间 2)直接计算收敛速度也很快的
	int result = taucs_minres(pMatrix,NULL,NULL,x,b,1000,1e-4);
	//判断结果
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
		//为cellScale[][3]申请空间
		cellScale = new double *[ALLCELLNUM];
		for(int i=0;i<ALLCELLNUM;i++)
			cellScale[i] = new double[3];
		//正确求得解，将其赋值给cellScale[][3]
		for(int k=0;k<CELLNUM+1;k++)       //高
		{
			for(int j=0;j<CELLNUM+1;j++)        //列
			{
				for(int i=0;i<CELLNUM+1;i++)        //行
				{
					if(i==CELLNUM||j==CELLNUM||k==CELLNUM)    //壳上的点,其对应的x[i]值是不需要的
					{
					}
					else     //不是壳上的点
					{
						int cellIndex1 = computeCellIndex(i,j,k);         //i,j,k对应的不带壳的grid中cell的下标
						int cellIndex2 = computeCellIndex1(i,j,k);        //i,j,k对应的带壳的grid中cell的下标
						cellScale[cellIndex1][0] = x[cellIndex2];
						cellScale[cellIndex1][1] = x[(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+cellIndex2];
						cellScale[cellIndex1][2] = x[2*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)+cellIndex2];
					}
				}
			}
		}
	}

	taucs_ccs_free(pMatrix);

	//此处可以释放W[][]和scaleEstimation[][]的资源了
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
	printf("W和scaleEstimation释放资源没问题.\n");
	//释放x[]和b[]的资源
	delete[] x;
	delete[] b;
	printf("x和b释放资源没问题.\n");
}

//可以假设在原来的grid的右、上、前三个方向各加上一层cell（为了方便计算）
void Resizer::computeOldCellVertex()
{
	//为变量oldCellVertex[][3]申请空间
	oldCellVertex = new double *[(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)];
	for(int i=0;i<(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i++)
		oldCellVertex[i] = new double[3];
	//开始计算oldCellVertex[][3]的值
	for(int i=0;i<(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i++)
	{
		int xx,yy,zz;
		computeNewCellIJK(i,&xx,&yy,&zz);      //(CELLNUM+1)
		//X方向
		oldCellVertex[i][0] = minBoundingBox[0] + xx*edgeLength[0];
		//Y方向
		oldCellVertex[i][1] = minBoundingBox[1] + yy*edgeLength[1];
		//Z方向
		oldCellVertex[i][2] = minBoundingBox[2] + zz*edgeLength[2];
	}
}
            
//这个函数功能比较难
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

	//为变量newCellVertex[][]申请空间
	newCellVertex = new double *[(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1)];
	for(int i=0;i<(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);i++)
		newCellVertex[i] = new double[3];

	//初始化所有元素为0
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<(CELLNUM+2)*(CELLNUM+2)*(CELLNUM+2);j++)
		{
			temp2[i][j] = 0.0;
			for(int k=0;k<4;k++)
				temp1[i][j][k] = 0.0;
		}
	}
	//对每个cell进行遍历，每次考虑8个顶点
	for(int p=0;p<3;p++)
	{
		for(int k=0;k<CELLNUM;k++)            //高
		{
			for(int j=0;j<CELLNUM;j++)          //列
			{ 
				for(int i=0;i<CELLNUM;i++)        //行
				{
					int cellIndex = computeCellIndex(i,j,k);         //(CELLNUM)
					//当前cell沿X方向的脆弱性
					double tempVulnerability = cellVulnerability[cellIndex][p];
					//当前cell的的Phi值
					double tempPhi = cellPhi[k][j][i];
					//当前cell的X方向的缩放大小
					double tempScale = cellScale[cellIndex][p];
					//当前cell的X方向缩放后的棱长
					double t = tempScale*edgeLength[p];
					//对于每个cell进行处理
					//对第0个顶点
					cellIndex = computeCellIndex2(i,j,k);
					temp1[p][cellIndex][0] += 2.0*tempPhi+tempVulnerability;
					temp1[p][cellIndex][1] += -tempVulnerability;
					temp1[p][cellIndex][2] += -tempPhi;
					temp1[p][cellIndex][3] += -tempPhi;
					temp2[p][cellIndex] += -2.0*tempVulnerability*t;
					//对第1个顶点
					cellIndex = computeCellIndex2(i+1,j,k);
					temp1[p][cellIndex][0] += 2.0*tempPhi+tempVulnerability;
					temp1[p][cellIndex][2] += -tempPhi;
					temp1[p][cellIndex][3] += -tempPhi;
					temp2[p][cellIndex] += 2.0*tempVulnerability*t;
					//对第2个顶点
					cellIndex = computeCellIndex2(i,j+1,k);
					temp1[p][cellIndex][0] += 2.0*tempPhi+tempVulnerability;
					temp1[p][cellIndex][1] += -tempVulnerability;
					temp1[p][cellIndex][3] += -tempPhi;		
					temp2[p][cellIndex] += -2.0*tempVulnerability*t;
					//对第3个顶点
					cellIndex = computeCellIndex2(i+1,j+1,k);
					temp1[p][cellIndex][0] += 2.0*tempPhi+tempVulnerability;
					temp1[p][cellIndex][3] += -tempPhi;
					temp2[p][cellIndex] += 2.0*tempVulnerability*t;
					//对第4个顶点
					cellIndex = computeCellIndex2(i,j,k+1);
					temp1[p][cellIndex][0] += 2.0*tempPhi+tempVulnerability;
					temp1[p][cellIndex][1] += -tempVulnerability;
					temp1[p][cellIndex][2] += -tempPhi;
					temp2[p][cellIndex] += -2.0*tempVulnerability*t;
					//对第5个顶点
					cellIndex = computeCellIndex2(i+1,j,k+1);
					temp1[p][cellIndex][0] += 2.0*tempPhi+tempVulnerability;
					temp1[p][cellIndex][2] += -tempPhi;
					temp2[p][cellIndex] += 2.0*tempVulnerability*t;
					//对第6个顶点
					cellIndex = computeCellIndex2(i,j+1,k+1);
					temp1[p][cellIndex][0] += 2.0*tempPhi+tempVulnerability;
					temp1[p][cellIndex][1] += -tempVulnerability;
					temp2[p][cellIndex] += -2.0*tempVulnerability*t;
					//对第7个顶点
					cellIndex = computeCellIndex2(i+1,j+1,k+1);
					temp1[p][cellIndex][0] += 2.0*tempPhi+tempVulnerability;
					temp2[p][cellIndex] += 2.0*tempVulnerability*t;
				}
			}
		}
	}

	//首先定义整个稀疏对称矩阵
	int m = (CELLNUM+2)*(CELLNUM+2)*(CELLNUM+2);                    //矩阵的行数
	int n = (CELLNUM+2)*(CELLNUM+2)*(CELLNUM+2);                    //矩阵的列数
	//稀疏矩阵中所有的非0元素的总个数
	int nnz = 4*(CELLNUM+1)*(CELLNUM+1)*(CELLNUM+1);
	//存放整个矩阵的变量
	taucs_ccs_matrix *pMatrix[3];            
	//为pMatrix[]申请空间
	for(int i=0;i<3;i++)
		pMatrix[i] = taucs_ccs_create(m,n,nnz,TAUCS_DOUBLE);
	//设置pMatrix的一些属性(对称和下三角)
	for(int i=0;i<3;i++)
	{
		pMatrix[i]->flags += TAUCS_SYMMETRIC;
		pMatrix[i]->flags += TAUCS_LOWER;
	}

	//X,Y,Z方向综合考虑
	for(int p=0;p<3;p++)
	{
		int num=0;                 //计数器，主要是为记录rowind[]和values.d[]位置下标
		int count=0;               //主要是为记录colptr[]存放的值 
		for(int k=0;k<(CELLNUM+2);k++)       //高
		{
			for(int j=0;j<(CELLNUM+2);j++)        //列
			{
				for(int i=0;i<(CELLNUM+2);i++)        //行
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

	taucs_double **x = new taucs_double *[3];                //存放运算得到的未知数结果
	for(int i=0;i<3;i++)
		x[i] = new taucs_double[(CELLNUM+2)*(CELLNUM+2)*(CELLNUM+2)];
	//对矩阵x[]进行赋值(以便快速收敛)
	for(int p=0;p<3;p++)
	{
		for(int k=0;k<(CELLNUM+2);k++)            //高
		{
			for(int j=0;j<(CELLNUM+2);j++)          //列
			{ 
				for(int i=0;i<(CELLNUM+2);i++)        //行
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
							int cellIndex = computeCellIndex2(j,i,k);    //此处注意
							x[p][cellIndex] = oldCellVertex[computeCellIndex1(i,j,k)][p];
						}
						else     //p==2
						{
							int cellIndex = computeCellIndex2(k,i,j);    //此处注意
							x[p][cellIndex] = oldCellVertex[computeCellIndex1(i,j,k)][p];
						}
					}
				}
			}
		}
	}

	taucs_double **b = new taucs_double *[3];                //存放右边的列矩阵
	for(int i=0;i<3;i++)
		b[i] = new taucs_double[(CELLNUM+2)*(CELLNUM+2)*(CELLNUM+2)];
	//对矩阵b[]进行赋值
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<(CELLNUM+2)*(CELLNUM+2)*(CELLNUM+2);j++)
		{
			b[i][j] = temp2[i][j];
		}
	}
	//先对X方向调用函数解决问题 
	for(int p=0;p<3;p++)
	{
		int result = taucs_minres(pMatrix[p],NULL,NULL,x[p],b[p],1000,0.0001);
		//判断结果
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
			//正确求得解，将其赋值给newCellVertex[][]
			for(int k=0;k<(CELLNUM+2);k++)            //高
			{
				for(int j=0;j<(CELLNUM+2);j++)          //列
				{ 
					for(int i=0;i<(CELLNUM+2);i++)        //行
					{
						if(i>CELLNUM||j>CELLNUM||k>CELLNUM)    
						{
							//这不是我们要的
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
								int cellIndex = computeCellIndex2(j,i,k);    //此处注意
								newCellVertex[computeCellIndex1(i,j,k)][p] = x[p][cellIndex];
							}
							else     //p==2
							{
								int cellIndex = computeCellIndex2(k,i,j);    //此处注意
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
	//释放资源
	//释放temp1[][][]
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
	printf("temp1释放资源没问题.\n");
	//释放temp2[][]
	for(int i=0;i<3;i++)
	{
		delete[] temp2[i];
	}
	delete[] temp2;
	printf("temp2释放资源没问题.\n");
	//释放x[][]
	for(int i=0;i<3;i++)
	{
		delete[] x[i];
	}
	delete[] x;
	printf("x释放资源没问题.\n");
	//释放b[][]
	for(int i=0;i<3;i++)
	{
		delete[] b[i];
	}
	delete[] b;
	printf("b释放资源没问题.\n");
	for(int i=0;i<3;i++)
		taucs_ccs_free(pMatrix[i]);
	printf("pMatrix释放资源没问题.\n");
}


void Resizer::computeVertexInCell()
{
	int vertexN = polyMesh->getVertexN();
	vertexInCell = new int *[vertexN];
	for(int i=0;i<vertexN;i++)
		vertexInCell[i] = new int[2];
	//得到每个顶点的3维坐标
	float (*vertex)[3] = polyMesh->vertex;
	for(int i=0;i<vertexN;i++)
	{
		//X方向的下标
		int xx = computeIndex(edgeLength[0],vertex[i][0],minBoundingBox[0]);
		int yy = computeIndex(edgeLength[1],vertex[i][1],minBoundingBox[1]);
		int zz = computeIndex(edgeLength[2],vertex[i][2],minBoundingBox[2]);
		vertexInCell[i][0] = computeCellIndex(xx,yy,zz);        //CELLNUM
		vertexInCell[i][1] = computeCellIndex1(xx,yy,zz);       //CELLNUM+1
		if(xx < 0 || yy < 0 || zz < 0)
		{
			printf("Something is error！\n");
			return;
		}
	}
}

//计算每个vertex的新的三维位置newVertex
void Resizer::updateVertex()
{
	int vertexN = polyMesh->getVertexN();
	for(int i=0;i<vertexN;i++)            //对每个cell
	{
		int index1 = vertexInCell[i][0];
		int index2 = vertexInCell[i][1];
		//计算出旧的顶点到cell的各个面的距离
		double distance[3];
		for(int j=0;j<3;j++)
		{
			distance[j] = polyMesh->vertex[i][j]-oldCellVertex[index2][j];
		}
		//计算出缩放后的顶点到cell的各个面的新的距离
		for(int j=0;j<3;j++)
		{
			distance[j] = distance[j]*cellScale[index1][j];
		}
		//计算出新的顶点坐标
		for(int j=0;j<3;j++)
		{
			polyMesh->vertex[i][j] = (float)(newCellVertex[index2][j]+distance[j]);
		}
	}
}

void Resizer::printCellVulnerability()
{
	printf("每个cell的vulnerability的值为:\n");
	for(int i=0;i<ALLCELLNUM;i++)
	{
		printf("%f  %f  %f\n",cellVulnerability[i][0],cellVulnerability[i][1],cellVulnerability[i][2]);
		Sleep(200);
	}
}

void Resizer::printCellPhi()
{
	printf("每个cell的Phi的值为:\n");
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
