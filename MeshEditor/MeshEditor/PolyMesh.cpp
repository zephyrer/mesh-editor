#pragma once
#include "stdafx.h"
#include "PolyMesh.h"

PolyMesh::PolyMesh()
{
	vertexN = 0;
	faceN = 0;
	vertex = NULL;
	face = NULL;
	normal = NULL;
	normal_f = NULL;
	vertexLinkV = NULL;
	vertexLinkF = NULL;
	degreeV = NULL;
	degreeF = NULL;
	isBound = NULL;
}

PolyMesh::~PolyMesh()
{
	if(vertex != NULL)
		delete[] vertex;
	if(face != NULL)
		delete[] face;
	if(normal != NULL)
		delete[] normal;
	if(normal_f != NULL)
		delete[] normal_f;
	if(vertexLinkV != NULL)
		delete[] vertexLinkV;
	if(vertexLinkF != NULL)
		delete[] vertexLinkF;
	if(degreeV != NULL)
		delete degreeV;
	if(degreeF != NULL)
		delete degreeF;
	if(isBound != NULL)
		delete isBound;
}

int PolyMesh::getVertexN()
{
	return vertexN;
}

int PolyMesh::getFaceN()
{
	return faceN;
}

void PolyMesh::setVertexN(int _vertexN)
{
	if(vertex != NULL)
		delete[] vertex;
	vertexN = _vertexN;
	vertex = new float[_vertexN][3];
}

void PolyMesh::setFaceN(int _faceN)
{
	if(face != NULL)
		delete[] face;
	faceN = _faceN;
	face = new int[_faceN][3];
}

void PolyMesh::setIndexVertex(int _index,float _coord[3])
{
	vertex[_index][0] = _coord[0];
	vertex[_index][1] = _coord[1];
	vertex[_index][2] = _coord[2];
}

void PolyMesh::setIndexFace(int _index,int _vertexIndex[3])
{
	face[_index][0] = _vertexIndex[0];
	face[_index][1] = _vertexIndex[1];
	face[_index][2] = _vertexIndex[2];
}

//����ÿ�������Ӧ�ķ���������λ�ģ�
void PolyMesh::computeNormal()
{
	if(normal != NULL)
		delete[] normal;
	normal = new float[vertexN][3];

	double (*tmp)[3] = new double[vertexN][3];
	for(int i=0; i<vertexN; i++)
		tmp[i][0] = tmp[i][1] = tmp[i][2] = 0;

	for(int i=0; i<faceN; i++)
	{
		int *f = face[i];
		float v1[3], v2[3];
		double c[3];
		VEC(v1, vertex[f[1]], vertex[f[0]]);
		VEC(v2, vertex[f[2]], vertex[f[0]]);
		CROSS(c, v1, v2);

		tmp[f[0]][0] += c[0];
		tmp[f[0]][1] += c[1];
		tmp[f[0]][2] += c[2];

		tmp[f[1]][0] += c[0];
		tmp[f[1]][1] += c[1];
		tmp[f[1]][2] += c[2];

		tmp[f[2]][0] += c[0];
		tmp[f[2]][1] += c[1];
		tmp[f[2]][2] += c[2];
	}

	for(int i=0; i<vertexN; i++)
	{
		double length = LENGTH(tmp[i]);
		if((float)length != 0)
		{
			normal[i][0] = (float)(tmp[i][0]/length);
			normal[i][1] = (float)(tmp[i][1]/length);
			normal[i][2] = (float)(tmp[i][2]/length);
		}
		else
			normal[i][0] = normal[i][1] = normal[i][2] = 0.0f;
	}

	delete[] tmp;
}

//�����淨��������λ��
void PolyMesh::computeFaceNormal()
{
	if(normal_f != NULL)
		delete[] normal_f;
	normal_f = new float[faceN][3];
	for(int i=0; i<faceN; i++){
		float v1[3], v2[3];
		double n[3];
		double length;
		VEC(v1, vertex[face[i][0]], vertex[face[i][1]]);
		VEC(v2, vertex[face[i][0]], vertex[face[i][2]]);
		CROSS(n, v1, v2);
		length = LENGTH(n);
		if((float)length != 0){
			normal_f[i][0] = (float)(n[0]/length);
			normal_f[i][1] = (float)(n[1]/length);
			normal_f[i][2] = (float)(n[2]/length);
		}
		else
			normal_f[i][0] = normal_f[i][1] = normal_f[i][2] = 0.0f;
	}
}

void PolyMesh::computeVertexLink()
{
	int *vertexFaceN = new int[vertexN];             //�������еĶ��㣬�������ÿ���������ĸ���
	for(int i=0;i<vertexN;i++)
		vertexFaceN[i] = 0;                          //��ʼÿ��Ԫ��Ϊ0
	//For Robustness
	for(int i=0;i<faceN;i++)
	{
		for(int j=0;j<3;j++)
		{
			if(face[i][j] < 0||face[i][j] >= vertexN)
			{
				face[i][0] = face[i][1] = face[i][2] = -1;
			}
			if(face[i][j] == face[i][(j+1)%3])
			{
				face[i][0] = face[i][1] = face[i][2] = -1;
			}
		}
	}
	//���濪ʼ���֣�ÿ�����Ӧ3��������±꣬��˱������ö����±�ʱ������Ӧ�Ķ�����������������1
	for(int i=0;i<faceN;i++)
	{
		if(face[i][0] < 0)
			continue;
		vertexFaceN[face[i][0]]++;
		vertexFaceN[face[i][1]]++;
		vertexFaceN[face[i][2]]++;
	}

	int **vertexLinkVertexs=new int*[vertexN];                 //���ÿ������1-ring���ڽ�����±�
	for(int i=0;i<vertexN;i++)
	{
		vertexLinkVertexs[i] = new int[3*vertexFaceN[i]];
		vertexFaceN[i] = 0;
	}
	//���濪ʼ���֣���ÿ�����Ӧ��1��������������������Ķ�������vertexLinkVertexs�У�ͬʱ��������ڵ�����±�
	for(int i=0;i<faceN;i++)
	{
		if(face[i][0] < 0)
			continue;
		for(int j=0;j<3;j++)
		{
			int index = face[i][j];
			if(index < 0)                                     //ֻ��Ϊ�˰�ȫ����ʵ�ǲ��ᷢ����
				continue;
			vertexLinkVertexs[index][3*vertexFaceN[index]] = face[i][(j+1)%3];
			vertexLinkVertexs[index][3*vertexFaceN[index]+1] = face[i][(j+2)%3];
			vertexLinkVertexs[index][3*vertexFaceN[index]+2] = i;       //��Ӧ��i����
			vertexFaceN[index]++;
		}
	}

	//Ϊ�˰�ȫ
	if(vertexLinkV != NULL)
	{
		for(int i=0;i<vertexN;i++)
		{
			if(degreeV[i] != 0)
				delete[] vertexLinkV[i];
		}
		delete[] vertexLinkV;
		delete[] degreeV;
	}
	vertexLinkV = new int*[vertexN];
	degreeV = new int[vertexN];

	if(vertexLinkF != NULL)
	{
		for(int i=0;i<vertexN;i++)
		{
			if(degreeF[i] != 0)
				delete[] vertexLinkF[i];
		}
		delete[] vertexLinkF;
		delete degreeF;
	}
	vertexLinkF = new int*[vertexN];
	degreeF = new int[vertexN];

	if(isBound != NULL)
		delete[] isBound;
	isBound = new int[vertexN];

	//�ж϶��������
	for(int i=0;i<vertexN;i++)
	{
		isBound[i] = sortVertexLink(vertexLinkVertexs[i],vertexFaceN[i],vertexLinkV[i],vertexLinkF[i],degreeV[i],degreeF[i]);
		if(vertexFaceN[i] != 0)
			delete[] vertexLinkVertexs[i];
	}

	delete[] vertexLinkVertexs;
	delete[] vertexFaceN;
}

//�ú������ö�������ڵ㰴��洢���ڵ���������±꣬����ĸ�����������������±꣬��ĸ���
//ע��:��ҪҪ�����ĸ������໥��Ӧ
int PolyMesh::sortVertexLink(int *_linkVertexs,int _linkN,int *&_linkV,int *&_linkF,int &_vertexN,int &_faceN)
{
	if(_linkN == 0)            //�����ö���������Ϊ0���ʴ�ʱ�õ��ǹ�����
	{
		_vertexN = 0;
		_linkV = NULL;
		_linkF = NULL;  
		_faceN = 0;
		return NONMANIFOLD;
	}
	_faceN = _linkN;          //�õ�������������Ѿ��������Ϊ_linkN��ֵ
	_linkF = new int[_faceN];
	int *tempLinkVertex = new int[2*_faceN];            //���õ����ڽ��Ķ�����±��_linkVertexes��ȡ�������ø��õĽṹ�洢
	for(int i=0;i<_linkN;i++)
	{
		tempLinkVertex[2*i] = _linkVertexs[3*i];
		tempLinkVertex[2*i+1] = _linkVertexs[3*i+1];    
		_linkF[i] = _linkVertexs[3*i+2];
	}

//=====================================================================================================================
	int mIndex = -1;
	int j;
	for(int i=0;i<_faceN;i++)
	{
		int target = tempLinkVertex[2*i];
		for(j=0;j<_faceN;j++)
		{
			if(target == tempLinkVertex[2*j+1])
				break;
		}
		if(j == _faceN)       
			mIndex = i;          		
	}

	if(mIndex != -1)         
	{
		int temp = tempLinkVertex[0];
		tempLinkVertex[0] = tempLinkVertex[2*mIndex];
		tempLinkVertex[2*mIndex] = temp;

		temp = tempLinkVertex[1];
		tempLinkVertex[1] = tempLinkVertex[2*mIndex+1];
		tempLinkVertex[2*mIndex+1] = temp;

		temp = _linkF[0];
		_linkF[0] = _linkF[mIndex];
		_linkF[mIndex] = temp;
	}

	//ʹ��ð�������tempLinkVertex��������
	for(int i=0; i<_faceN-1; i++)
	{
		int target = tempLinkVertex[2*i+1];
		for(j=i+1; j<_faceN; j++)
		{
			if(target == tempLinkVertex[2*j])
			{
				int tmp = tempLinkVertex[2*i+2];
				tempLinkVertex[2*i+2] = tempLinkVertex[2*j];
				tempLinkVertex[2*j] = tmp;

				tmp = tempLinkVertex[2*i+3];
				tempLinkVertex[2*i+3] = tempLinkVertex[2*j+1];
				tempLinkVertex[2*j+1] = tmp;

				tmp = _linkF[i+1];
				_linkF[i+1] = _linkF[j];
				_linkF[j] = tmp;

				break;
			}
		}
		if(j == _faceN)
			mIndex = -2;
	}

	if(mIndex < 0 && tempLinkVertex[0] != tempLinkVertex[2*_faceN-1])
		mIndex = -2;
	if(mIndex == -1)
	{
		for(int i=0; i<_faceN; i++)
		{
			int target = tempLinkVertex[2*i];
			int consist = 0;
			for(j=0; j<_faceN; j++)
			{
				if(target == tempLinkVertex[2*j+1])
				{
					consist++;		
				}
			}
			if(consist != 1){
				mIndex = -2;
				break;
			}
		}
	}
	else if(mIndex >= 0)
	{
		for(int i=1; i<_faceN; i++)
		{
			int target = tempLinkVertex[2*i];
			int consist = 0;
			for(int j=0; j<_faceN; j++)
			{
				if(target == tempLinkVertex[2*j+1])
				{
					consist++;		
				}
			}
			if(consist != 1)
			{
				mIndex = -2;
				break;
			}
		}
	}
//=====================================================================================================================
	int type;
	if(mIndex == -1)
	{
		type = INNER;
		_vertexN = _faceN;
		_linkV = new int[_vertexN];
		for(int i=0;i<_vertexN;i++)
			_linkV[i] = tempLinkVertex[2*i];
	}
	else if(mIndex == -2)
	{
		type = NONMANIFOLD;
		_vertexN = 2*_faceN;
		_linkV = new int[_vertexN];
		for(int i=0;i<_vertexN;i++)
			_linkV[i] = tempLinkVertex[i];
	}
	else
	{
		type = BOUNDARY;
		_vertexN = _faceN+1;
		_linkV = new int[_vertexN];
		for(int i=0;i<_vertexN;i++)
			_linkV[i] = tempLinkVertex[2*i];
		_linkV[_faceN] = tempLinkVertex[2*_faceN-1];
	}
	delete[] tempLinkVertex;
	return type;
}

double PolyMesh::computeVolume()
{
	double vol = 0;
	double g[3],n[3],v1[3],v2[3];

	for(int i=0; i<faceN; i++)
	{
		g[0] = (vertex[ face[i][0] ][0] + vertex[ face[i][1] ][0] + vertex[ face[i][2] ][0])/3.0;
		g[1] = (vertex[ face[i][0] ][1] + vertex[ face[i][1] ][1] + vertex[ face[i][2] ][1])/3.0;
		g[2] = (vertex[ face[i][0] ][2] + vertex[ face[i][1] ][2] + vertex[ face[i][2] ][2])/3.0;

		v1[0] = vertex[ face[i][1] ][0] - vertex[ face[i][0] ][0];
		v1[1] = vertex[ face[i][1] ][1] - vertex[ face[i][0] ][1];
		v1[2] = vertex[ face[i][1] ][2] - vertex[ face[i][0] ][2];

		v2[0] = vertex[ face[i][2] ][0] - vertex[ face[i][0] ][0];
		v2[1] = vertex[ face[i][2] ][1] - vertex[ face[i][0] ][1];
		v2[2] = vertex[ face[i][2] ][2] - vertex[ face[i][0] ][2];

		n[0] = v1[1]*v2[2] - v1[2]*v2[1];
		n[1] = v1[2]*v2[0] - v1[0]*v2[2];
		n[2] = v1[0]*v2[1] - v1[1]*v2[0];

		vol += (g[0]*n[0] + g[1]*n[1] + g[2]*n[2])/6.0;
	}
	return vol;
}

void PolyMesh::rescale(double _rate)
{
	for(int i=0; i<vertexN; i++)
	{
		vertex[i][0] = (float)(_rate*vertex[i][0]);
		vertex[i][1] = (float)(_rate*vertex[i][1]);;
		vertex[i][2] = (float)(_rate*vertex[i][2]);
	}
}