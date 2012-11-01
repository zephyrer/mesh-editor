
// MeshEditorDoc.h : interface of the CMeshEditorDoc class
//


#pragma once
#include "FileManager.h"
#include "Smoother.h"
#include "ParameterSetDlg1.h"
#include "ParameterSetDlg2.h"
#include "ParameterSetDlg3.h"
#include "ParameterSetDlg4.h"
#include "ParameterSetDlg5.h"
#include "ProcessViewerDlg.h"

#define FLAT_MODE 0
#define WIRE_MODE 1

class CMeshEditorDoc : public CDocument
{
protected: // create from serialization only
	CMeshEditorDoc();
	DECLARE_DYNCREATE(CMeshEditorDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CMeshEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

	//My Defined Variables
private:
	int mode;                        //显示模式
	CString title;                   //存放的是窗口标题
	PolyMesh *polyMesh;              //多边形化对象
	Smoother *smoother;              //平滑操作类
	ParameterSetDlg1 *parameterSet1;   
	ParameterSetDlg2 *parameterSet2;     
	ParameterSetDlg3 *parameterSet3;       
	ParameterSetDlg4 *parameterSet4;
	ParameterSetDlg5 *parameterSet5;       //平滑参数设置
	ProcessViewerDlg *processViewerDlg;    //迭代过程显示类 

	//My Defined Functions
public:
	void drawFun();
	void selDrawFun();
	afx_msg void OnFileOpen();
	afx_msg void OnSmoothingImplicitMeancurvatureflow();
	afx_msg void OnSmoothingLaplacianflow();
	afx_msg void OnFlatShade();
	afx_msg void OnWireFrame();
	afx_msg void OnSmoothingBilaplacianflowbykobbelt();
	afx_msg void OnSmoothingExplicitmeancurvatureflow();
	afx_msg void OnSmoothingTaubinsmoothing();
};
