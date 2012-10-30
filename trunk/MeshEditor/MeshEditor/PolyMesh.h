#ifndef HEADER_POLYMESH_H
#define HEADER_POLYMESH_H

#include <math.h>
#define INNER 0
#define BOUNDARY 1
#define NONMANIFOLD 2            //���嶥�����������

//�����������
class PolyMesh
{
private:
	int vertexN;      //�������
	int faceN;        //��ĸ���
public:
	float (*vertex)[3];        //������ж��������
	int (*face)[3];            //����������Ӧ�����������Ӧ�Ķ�����±�
	float (*normal)[3];        //ÿ�����Ӧ�ķ�����
	float (*normal_f)[3];      //��ķ�����
	int **vertexLinkV;         //��¼���������ӵ���������������±�(�����ظ�)
	int **vertexLinkF;         //��¼���������ڵ����е���
	int *degreeV;              //�������(����ö���1-ring�Ķ���ĸ���)
	int *degreeF;              //������ǰ�������ĸ���
	int *isBound;              //�ж�ÿ���������������isBound==INNER,�����㴦���ڲ���isBound==BOUNDARY,�����㴦�ڱ߽磻isBound==ISOLATED,�������ǹ����㣩

public:
	PolyMesh();
	~PolyMesh();
	int getVertexN();
	int getFaceN();
	void setVertexN(int _vertexN);
	void setFaceN(int _faceN);
	void setIndexVertex(int _index,float _coord[3]);      //�����±�Ϊ_index���Ķ��������
	void setIndexFace(int _index,int _vertexIndex[3]);    //�����±�Ϊ_index�������������������Ӧ�ĵ���±�
	void computeNormal();                                 //����ÿ����ķ�����
	void computeFaceNormal();                             //������ķ�����
	void computeVertexLink();                             //���㶥������ӹ�ϵ 
	/*�ں���computeVertexLink()���Ѿ��ó���������vertexFaceN��vertexLinkVertexs,����vertexFaceN��¼ÿ���������ڵ���ĸ�����
	vertexLinkVertexs����һ���Ľṹ��¼ÿ���������ڵĶ�����±꼰ÿ���������ڵ�����±ꡣ���ڣ���Ҫ��������sortVertexLink()��
	���ã��ó���ǰ�����ڽ��ĵ���±꼰��������ó���ǰ��������������±꼰��������ó���ǰ��������������
	ע��:����INNER���͵ĵ㣬���ڽ���Ӧ�ð��±���ģ����Χ�Ƹõ�˳������һȦ��
	     ����BOUNDARY���͵ĵ㣬ҲӦ�ð��±�˳������һ����Ҫ��Ϊ�˺��������㣩*/
	int sortVertexLink(int *_linkVertexs,int _linkN,int *&_linkV,int *&_linkF,int &_vertexN,int &_faceN);            
	double computeVolume();                               //���㵱ǰģ�͵����
	void rescale(double _rate);                           //��ģ���е����е㰴��_rate�ı����������������С

	//VEC����P1ָ��P2������
	//CROSS����������V1��V2��ֱ������
	//LENGTH������N��ģ
	//AREA�����P1��P2��P3Χ�ɵ������ε����
	//DOT�ǵ��
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