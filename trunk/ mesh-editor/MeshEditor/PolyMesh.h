#ifndef HEADER_POLYMESH_H
#define HEADER_POLYMESH_H

#include <math.h>
#define INNER 0
#define BOUNDARY 1
#define NONMANIFOLD 2            //定义顶点所属的类别

//多边形网格类
class PolyMesh
{
private:
	int vertexN;      //顶点个数
	int faceN;        //面的个数
public:
	float (*vertex)[3];        //存放所有顶点的坐标
	int (*face)[3];            //存放所有面对应的三个顶点对应的顶点的下标
	float (*normal)[3];        //每个点对应的法向量
	float (*normal_f)[3];      //面的法向量
	int **vertexLinkV;         //记录顶点所连接的所有其它顶点的下标(可以重复)
	int **vertexLinkF;         //记录顶点所属于的所有的面
	int *degreeV;              //顶点度数(即与该顶点1-ring的顶点的个数)
	int *degreeF;              //包含当前顶点的面的个数
	int *isBound;              //判断每个顶点所属的类别（isBound==INNER,即顶点处于内部；isBound==BOUNDARY,即顶点处于边界；isBound==ISOLATED,即顶点是孤立点）

public:
	PolyMesh();
	~PolyMesh();
	int getVertexN();
	int getFaceN();
	void setVertexN(int _vertexN);
	void setFaceN(int _faceN);
	void setIndexVertex(int _index,float _coord[3]);      //设置下标为_index处的顶点的坐标
	void setIndexFace(int _index,int _vertexIndex[3]);    //设置下标为_index处的面的三个顶点所对应的点的下标
	void computeNormal();                                 //计算每个点的法向量
	void computeFaceNormal();                             //计算面的法向量
	void computeVertexLink();                             //计算顶点的连接关系 
	/*在函数computeVertexLink()中已经得出两个变量vertexFaceN和vertexLinkVertexs,其中vertexFaceN记录每个顶点属于的面的个数，
	vertexLinkVertexs按照一定的结构记录每个顶点相邻的顶点的下标及每个顶点属于的面的下标。现在，需要经过函数sortVertexLink()的
	调用，得出当前顶点邻近的点的下标及其个数，得出当前顶点所属的面的下标及其个数，得出当前顶点所属的类型
	注意:对于INNER类型的点，其邻近点应该按下标在模型上围绕该点顺序连成一圈；
	     对于BOUNDARY类型的点，也应该按下标顺序连在一起（主要是为了后续处理方便）*/
	int sortVertexLink(int *_linkVertexs,int _linkN,int *&_linkV,int *&_linkF,int &_vertexN,int &_faceN);            
	double computeVolume();                               //计算当前模型的体积
	void rescale(double _rate);                           //将模型中的所有点按照_rate的比例进行扩大或者缩小

	//VEC是求P1指向P2的向量
	//CROSS是求与向量V1和V2垂直的向量
	//LENGTH是向量N的模
	//AREA是求点P1、P2、P3围成的三角形的面积
	//DOT是点乘
	static inline double AREA(float p1[3], float p2[3], float p3[3])
	{
		double n[3];
		float v1[3], v2[3];
		VEC(v1, p2, p1);
		VEC(v2, p3, p1);
		CROSS(n, v1, v2);

		return 0.5*LENGTH(n);
	}

	static inline double AREA(double p1[3], double p2[3], double p3[3])
	{
		double n[3];
		double v1[3], v2[3];
		VEC(v1, p2, p1);
		VEC(v2, p3, p1);
		CROSS(n, v1, v2);

		return 0.5*LENGTH(n);
	}

	static inline double AREA(float p1[3], float p2[3], double p3[3])
	{
		double n[3];
		float v1[3];
		double v2[3];
		VEC(v1, p2, p1);
		VEC(v2, p3, p1);
		CROSS(n, v1, v2);

		return 0.5*LENGTH(n);
	}
	static inline void VEC(float v[3], float p1[3], float p2[3])
	{
		v[0] = p2[0]-p1[0];
		v[1] = p2[1]-p1[1];
		v[2] = p2[2]-p1[2];
	}

	static inline void VEC(double v[3], double p1[3], double p2[3])
	{
		v[0] = p2[0]-p1[0];
		v[1] = p2[1]-p1[1];
		v[2] = p2[2]-p1[2];
	}

	static inline void VEC(double v[3], double p1[3], float p2[3])
	{
		v[0] = (double)p2[0]-p1[0];
		v[1] = (double)p2[1]-p1[1];
		v[2] = (double)p2[2]-p1[2];
	}

	static inline void VEC(double v[3], float p1[3], double p2[3])
	{
		v[0] = p2[0]-(double)p1[0];
		v[1] = p2[1]-(double)p1[1];
		v[2] = p2[2]-(double)p1[2];
	}

	static inline double LENGTH(float v[3])
	{
		return sqrt((double)v[0]*(double)v[0] + (double)v[1]*(double)v[1] + (double)v[2]*(double)v[2]);
	}

	static inline double LENGTH(double v[3])
	{
		return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	}

	static inline void CROSS(float n[3], float v1[3], float v2[3])
	{
		n[0] = v1[1]*v2[2] - v1[2]*v2[1];
		n[1] = v1[2]*v2[0] - v1[0]*v2[2];
		n[2] = v1[0]*v2[1] - v1[1]*v2[0];
	}

	static inline void CROSS(double n[3], double v1[3], double v2[3])
	{
		n[0] = v1[1]*v2[2] - v1[2]*v2[1];
		n[1] = v1[2]*v2[0] - v1[0]*v2[2];
		n[2] = v1[0]*v2[1] - v1[1]*v2[0];
	}

	static inline void CROSS(double n[3], float v1[3], float v2[3])
	{
		n[0] = (double)v1[1]*(double)v2[2] - (double)v1[2]*(double)v2[1];
		n[1] = (double)v1[2]*(double)v2[0] - (double)v1[0]*(double)v2[2];
		n[2] = (double)v1[0]*(double)v2[1] - (double)v1[1]*(double)v2[0];
	}

	static inline void CROSS(double n[3], double v1[3], float v2[3])
	{
		n[0] = v1[1]*(double)v2[2] - v1[2]*(double)v2[1];
		n[1] = v1[2]*(double)v2[0] - v1[0]*(double)v2[2];
		n[2] = v1[0]*(double)v2[1] - v1[1]*(double)v2[0];
	}

	static inline void CROSS(double n[3], float v1[3], double v2[3])
	{
		n[0] = (double)v1[1]*v2[2] - (double)v1[2]*v2[1];
		n[1] = (double)v1[2]*v2[0] - (double)v1[0]*v2[2];
		n[2] = (double)v1[0]*v2[1] - (double)v1[1]*v2[0];
	}

	static inline void CROSS(float n[3], double v1[3], double v2[3])
	{
		n[0] = (float)(v1[1]*v2[2] - v1[2]*v2[1]);
		n[1] = (float)(v1[2]*v2[0] - v1[0]*v2[2]);
		n[2] = (float)(v1[0]*v2[1] - v1[1]*v2[0]);
	}

	static inline void CROSS(float n[3], double v1[3], float v2[3])
	{
		n[0] = (float)(v1[1]*v2[2] - v1[2]*v2[1]);
		n[1] = (float)(v1[2]*v2[0] - v1[0]*v2[2]);
		n[2] = (float)(v1[0]*v2[1] - v1[1]*v2[0]);
	}

	static inline void CROSS(float n[3], float v1[3], double v2[3])
	{
		n[0] = (float)(v1[1]*v2[2] - v1[2]*v2[1]);
		n[1] = (float)(v1[2]*v2[0] - v1[0]*v2[2]);
		n[2] = (float)(v1[0]*v2[1] - v1[1]*v2[0]);
	}

	static inline double DOT(float v1[3], float v2[3])
	{
		return (double)v1[0]*(double)v2[0] + (double)v1[1]*(double)v2[1] + (double)v1[2]*(double)v2[2];
	}

	static inline double DOT(double v1[3], float v2[3])
	{
		return v1[0]*(double)v2[0] + v1[1]*(double)v2[1] + v1[2]*(double)v2[2];
	}

	static inline double DOT(float v1[3], double v2[3])
	{
		return (double)v1[0]*v2[0] + (double)v1[1]*v2[1] + (double)v1[2]*v2[2];
	}

	static inline double DOT(double v1[3], double v2[3])
	{
		return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
	}
};
#endif