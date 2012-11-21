#ifndef HEADER_RESIZER_H
#define HEADER_RESIZER_H

#include "PolyMesh.h"
#define CELLNUM 20                             //ÿ�������ϻ��ֵĸ���
#define CELLLAYER (CELLNUM*CELLNUM)              //ÿһ���ϵ�cell�ĸ���
#define ALLCELLNUM (CELLNUM*CELLNUM*CELLNUM)     //��Ԫ���ܵĸ���


//����taucs�⣬�������һЩͷ�ļ������Ȳ���
#include <assert.h>
#define TAUCS_CORE_DOUBLE
extern "C"
{
#include <taucs.h>
#include <taucs_private.h>
};

//�����࣬����һЩ���Ų���
class Resizer
{
private:
	PolyMesh *polyMesh;
	float minBoundingBox[3];                   //���ģ�������׵��������Сֵ�߽�
	float maxBoundingBox[3];                   //���ģ�������׵���������ֵ�߽�
	float edgeLength[3];                       //����ÿ��cell�����������ϵ�ֵ
	double (*cellVulnerability)[3];            //��¼ÿ������cell�Ĵ�����(��Ȼ�����������)
	int *cellIncludingFaceNum;				   //��¼ÿ��cell�����ľ�������face�ĸ���
	int (*cellIncludingFace)[200];             //��¼ÿ��cell�����ľ�������face���±�(���ܲ�ֹһ��,һ��϶�С��200)
	int *facePassthroughCellNum;               //��¼ÿ��face������cell�ĸ���
	int (*facePassthroughCell)[200];           //��¼ÿ��face������Cell�ı��(���ܲ�ֹһ��,��󲻻ᳬ��200)
	double ***cellPhi;                         //��¼k,j,i(Z,Y,X)��Ӧ��cell��phiֵ(ע��Ϊ�˼��㷽�㣬��������grid����������һ������)
	double **W;                                //��¼�������ᵽ��ÿ��cell��W[i][0]--XY,W[i][1]--YZ,W[i][2]--ZX
	double **scaleEstimation;                  //��¼�������ᵽ��ÿ��cell��scaleEstimation[i][0]--X,scaleEstimation[i][1]--Y,scaleEstimation[i][2]--Z
	double **cellScale;                        //��¼ÿ��cell�����������ϵ����Ŵ�С
	double **oldCellVertex;                    //����ǰÿ��cell��ֻҪ������½ǵ�һ�����������(3ά��)
	double **newCellVertex;                    //���ź�ÿ��cell�����½Ƕ����������(3ά��)
	int **vertexInCell;                        //��¼ÿ���������ڵ�cell���±�(CELLNUM��(CELLNUM+1)��������)

public:
	Resizer();
	~Resizer();
	//����ָ��polyMesh��ָ��
	void setPolyMesh(PolyMesh* _polyMesh); 
	//����minBoundingBox[]��maxBoundingBox[]��edgeLength[]��ֵ���������ļ��㣩
	void computeBoundingBoxAndEdgeLength();
	//����һ��face���������㣬������߽�
	void computeFaceBoundingBox(float _vertex[3][3],float _min[3],float _max[3]);
	//����˳�����
	int computeIndex(float _edgeLength,float _coordValue,float _minCoordValue);
	//����i,j,k�����grid�ж�Ӧ��cell���±�(ע�⣺grid��cell�±귶Χ:[0,ALLCELLNUM-1]����ŵķ����ǣ���X����Y�����Z)
	int computeCellIndex(int _i,int _j,int _k);
	//����i,j,k�����grid�ж�Ӧ��cell���±�(�������ҡ��ϡ�ǰ���һ�����������¡�����)��CELLNUM+1��
	int computeCellIndex1(int _i,int _j,int _k);
	//����i,j,k�����grid�ж�Ӧ��cell���±�(�������������������һ������)��CELLNUM+2��
	int computeCellIndex2(int _i,int _j,int _k);
	//���ݸ���grid��cell���±������i,j,k��ֵ(�������ҡ��ϡ�ǰ���һ�����������¡�����)(CELLNUM+1)
	void computeNewCellIJK(int cellIndex,int *_i,int *_j,int *_k);
	//����_indexMin[3]��_indexMax[3]��ֵ�������_faceIndex���facePassthroughCell[i][]��facePassthroughCellNum[i]��ֵ
	void computeFacePassthroughingCell(int _faceIndex,float _tempVertex[3][3],int _indexMin[3],int _indexMax[3],float _cellHalfLength[3]);
	//����facePassthroughCell[i][]��facePassthroughCellNum[i]��ֵ�����cellIncludingFace[][]��cellIncludingFaceNum[]��ֵ
	void computeCellIncludingFace();
	//����cell��face���ཻ��ϵ(��Ҫ���ĸ�������facePassthroughCell[][]��facePassthroughCellNum[]��cellIncludingFace[][]��cellIncludingFaceNum[])
	void computeCellAndFaceRelationship(); 
	//����ÿ����Ԫ�Ĵ�����(��Ҫ�Ǽ����������cellVulnerability[][])
	void computeCellVulnerability();    
	//����ÿ��cell��phiֵ����΢�˷�һ��ռ䣬���ǻ����������������ԣ�
	void computeCellPhi();
	//����ÿ��cell��W[][3]ֵ
	void computeCellW(); 
	//����ÿ��cell��scaleEstimation[][3]ֵ
	void computeCellScaleEstimation(float _S[3]);
	//����ÿ��cell�����ŵĴ�С(��Ҫ�Ǽ���cellScale[][]��ֵ,���в���S[]Ϊ�û������ģ����������������������ű���)
	void computeCellScale(float _S[3]);              
	//��������ǰÿ��cell��8����������꣨����oldCellVertex[][]��
	void computeOldCellVertex();
	//�������ź�ÿ��cell��8����������꣨����newCellVertex[][]��
	void computeNewCellVertex();
	//����ÿ��vertex�����ĸ�cell(������vertexInCell[]��ֵ)
	void computeVertexInCell();
	//���¶����λ��
	void updateVertex();
	//for test (���ÿ��cell��vulnerabilityֵ)
	void printCellVulnerability();
	//for test�����ÿ��cell��Phi��ֵ��
	void printCellPhi();
	//for test�����ÿ��cell��cellScale��ֵ��
	void printCellScale();
};
#endif