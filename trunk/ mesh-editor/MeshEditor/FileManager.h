#ifndef HEADER_FILEMANAGER_H
#define HEADER_FILEMANAGER_H

//����һ���ļ������࣬�������ļ��������ļ�֮��ĸ��ֲ���
#include "PolyMesh.h"
class FileManager
{
public:
	FileManager();
	~FileManager();
	void openPly2(CString _filePath,PolyMesh *_polyMesh);    //��.ply2�ļ�(������ʵ�飬����ֻʵ��.ply2�ļ��Ĵ�)
};
#endif