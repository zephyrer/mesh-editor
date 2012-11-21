#ifndef HEADER_RESIZER_H
#define HEADER_RESIZER_H

#include "PolyMesh.h"
#define CELLNUM 20                             //每个方向上划分的个数
#define CELLLAYER (CELLNUM*CELLNUM)              //每一层上的cell的个数
#define ALLCELLNUM (CELLNUM*CELLNUM*CELLNUM)     //体元的总的个数


//调用taucs库，必须进行一些头文件包含等操作
#include <assert.h>
#define TAUCS_CORE_DOUBLE
extern "C"
{
#include <taucs.h>
#include <taucs_private.h>
};

//缩放类，进行一些缩放操作
class Resizer
{
private:
	PolyMesh *polyMesh;
	float minBoundingBox[3];                   //存放模型外面套的网格的最小值边界
	float maxBoundingBox[3];                   //存放模型外面套的网格的最大值边界
	float edgeLength[3];                       //计算每个cell在三个方向上的值
	double (*cellVulnerability)[3];            //记录每个网格cell的脆弱性(当然是三个方向的)
	int *cellIncludingFaceNum;				   //记录每个cell包含的经过它的face的个数
	int (*cellIncludingFace)[200];             //记录每个cell包含的经过它的face的下标(可能不止一个,一般肯定小于200)
	int *facePassthroughCellNum;               //记录每个face经过的cell的个数
	int (*facePassthroughCell)[200];           //记录每个face经过的Cell的编号(可能不止一个,最大不会超过200)
	double ***cellPhi;                         //记录k,j,i(Z,Y,X)对应的cell的phi值(注意为了计算方便，我在整个grid的外面套了一个壳子)
	double **W;                                //记录论文中提到的每个cell的W[i][0]--XY,W[i][1]--YZ,W[i][2]--ZX
	double **scaleEstimation;                  //记录论文中提到的每个cell的scaleEstimation[i][0]--X,scaleEstimation[i][1]--Y,scaleEstimation[i][2]--Z
	double **cellScale;                        //记录每个cell在三个方向上的缩放大小
	double **oldCellVertex;                    //缩放前每个cell的只要存放左下角的一个顶点的坐标(3维的)
	double **newCellVertex;                    //缩放后每个cell的左下角顶点的新坐标(3维的)
	int **vertexInCell;                        //记录每个顶点属于的cell的下标(CELLNUM和(CELLNUM+1)两种情形)

public:
	Resizer();
	~Resizer();
	//设置指针polyMesh的指向
	void setPolyMesh(PolyMesh* _polyMesh); 
	//计算minBoundingBox[]和maxBoundingBox[]和edgeLength[]的值（方便后面的计算）
	void computeBoundingBoxAndEdgeLength();
	//给定一个face的三个顶点，计算其边界
	void computeFaceBoundingBox(float _vertex[3][3],float _min[3],float _max[3]);
	//运用顺序查找
	int computeIndex(float _edgeLength,float _coordValue,float _minCoordValue);
	//根据i,j,k计算出grid中对应的cell的下标(注意：grid中cell下标范围:[0,ALLCELLNUM-1]，存放的方向是：先X，后Y，最后Z)
	int computeCellIndex(int _i,int _j,int _k);
	//根据i,j,k计算出grid中对应的cell的下标(对于在右、上、前添加一层的情况，左、下、后不算)（CELLNUM+1）
	int computeCellIndex1(int _i,int _j,int _k);
	//根据i,j,k计算出grid中对应的cell的下标(对于外面六个方向添加一层的情况)（CELLNUM+2）
	int computeCellIndex2(int _i,int _j,int _k);
	//根据给定grid中cell的下标求得其i,j,k的值(对于在右、上、前添加一层的情况，左、下、后不算)(CELLNUM+1)
	void computeNewCellIJK(int cellIndex,int *_i,int *_j,int *_k);
	//根据_indexMin[3]和_indexMax[3]的值计算出第_faceIndex面的facePassthroughCell[i][]和facePassthroughCellNum[i]的值
	void computeFacePassthroughingCell(int _faceIndex,float _tempVertex[3][3],int _indexMin[3],int _indexMax[3],float _cellHalfLength[3]);
	//根据facePassthroughCell[i][]和facePassthroughCellNum[i]的值计算出cellIncludingFace[][]和cellIncludingFaceNum[]的值
	void computeCellIncludingFace();
	//计算cell和face的相交关系(主要是四个变量：facePassthroughCell[][]和facePassthroughCellNum[]和cellIncludingFace[][]和cellIncludingFaceNum[])
	void computeCellAndFaceRelationship(); 
	//计算每个体元的脆弱度(主要是计算出变量：cellVulnerability[][])
	void computeCellVulnerability();    
	//计算每个cell的phi值（稍微浪费一点空间，但是换来整个程序可理解性）
	void computeCellPhi();
	//计算每个cell的W[][3]值
	void computeCellW(); 
	//计算每个cell的scaleEstimation[][3]值
	void computeCellScaleEstimation(float _S[3]);
	//计算每个cell的缩放的大小(主要是计算cellScale[][]的值,其中参数S[]为用户定义的模型在三个方向上整体的缩放比例)
	void computeCellScale(float _S[3]);              
	//计算缩放前每个cell的8个顶点的坐标（即求oldCellVertex[][]）
	void computeOldCellVertex();
	//计算缩放后每个cell的8个顶点的坐标（即求newCellVertex[][]）
	void computeNewCellVertex();
	//计算每个vertex属于哪个cell(即计算vertexInCell[]的值)
	void computeVertexInCell();
	//更新顶点的位置
	void updateVertex();
	//for test (输出每个cell的vulnerability值)
	void printCellVulnerability();
	//for test（输出每个cell的Phi的值）
	void printCellPhi();
	//for test（输出每个cell的cellScale的值）
	void printCellScale();
};
#endif