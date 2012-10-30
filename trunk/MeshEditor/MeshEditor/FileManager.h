#ifndef HEADER_FILEMANAGER_H
#define HEADER_FILEMANAGER_H

//定义一个文件操作类，包括打开文件、保存文件之类的各种操作
#include "PolyMesh.h"
class FileManager
{
public:
	FileManager();
	~FileManager();
	void openPly2(CString _filePath,PolyMesh *_polyMesh);    //打开.ply2文件(由于做实验，所以只实现.ply2文件的打开)
};
#endif