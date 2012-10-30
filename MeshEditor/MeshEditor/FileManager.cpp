#pragma once
#include "stdafx.h"
#include "FileManager.h"

//���򿪵��ļ����ݴ���������polyMesh��
FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

void FileManager::openPly2(CString _filePath,PolyMesh *_polyMesh)  
{
	FILE *file = fopen(_filePath,"r");       //��ֻ����ʽ��ָ���ļ�
	int vertexN,faceN;
	fscanf(file,"%d",&vertexN);
	_polyMesh->setVertexN(vertexN);
	fscanf(file,"%d",&faceN);
	_polyMesh->setFaceN(faceN);
	float vertex[3];
	int face[3];
	for(int i=0;i<vertexN;i++)
	{
		fscanf(file,"%f",&vertex[0]);
		fscanf(file,"%f",&vertex[1]);
		fscanf(file,"%f",&vertex[2]);
		_polyMesh->setIndexVertex(i,vertex);
	}
	int temp;
	for(int i=0;i<faceN;i++)
	{
		fscanf(file,"%d",&temp);
		fscanf(file,"%d",&face[0]);
		fscanf(file,"%d",&face[1]);
		fscanf(file,"%d",&face[2]);
		_polyMesh->setIndexFace(i,face);
	}
	fclose(file);
}