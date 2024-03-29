
// MeshEditorDoc.cpp : implementation of the CMeshEditorDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MeshEditor.h"
#endif

#include "MeshEditorDoc.h"
#include "MeshEditorView.h"
#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMeshEditorDoc

IMPLEMENT_DYNCREATE(CMeshEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CMeshEditorDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, &CMeshEditorDoc::OnFileOpen)
	ON_COMMAND(ID_SMOOTHING_MEANCURVATUREFLOW, &CMeshEditorDoc::OnSmoothingImplicitMeancurvatureflow)
	ON_COMMAND(ID_SMOOTHING_LAPLACIANFLOW, &CMeshEditorDoc::OnSmoothingLaplacianflow)
	ON_COMMAND(ID_VISUALIZATION_FLATSHADING, &CMeshEditorDoc::OnFlatShade)
	ON_COMMAND(ID_VISUALIZATION_WIREFRAME, &CMeshEditorDoc::OnWireFrame)
	ON_COMMAND(ID_SMOOTHING_BILAPLACIANFLOWBYKOBBELT, &CMeshEditorDoc::OnSmoothingBilaplacianflowbykobbelt)
	ON_COMMAND(ID_SMOOTHING_EXPLICITMEANCURVATUREFLOW, &CMeshEditorDoc::OnSmoothingExplicitmeancurvatureflow)
	ON_COMMAND(ID_SMOOTHING_TAUBINSMOOTHING, &CMeshEditorDoc::OnSmoothingTaubinsmoothing)
	ON_COMMAND(ID_RESIZING_NON, &CMeshEditorDoc::OnResizingNonhomogenous)
END_MESSAGE_MAP()

//For console:print some information
//=====================================================================================
#include <io.h>  
#include <fcntl.h>  
void InitConsole()  
{  
	int nRet= 0;  
	FILE* fp;  
	AllocConsole();  
	nRet= _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);  
	fp = _fdopen(nRet, "w");  
	*stdout = *fp;  
	setvbuf(stdout, NULL, _IONBF, 0); 
}
//=====================================================================================

// CMeshEditorDoc construction/destruction

CMeshEditorDoc::CMeshEditorDoc()
{
	InitConsole();
	polyMesh = NULL;
	smoother = new Smoother();
	resizer = new Resizer();
	parameterSet1 = new ParameterSetDlg1();
	parameterSet2 = new ParameterSetDlg2();
	parameterSet3 = new ParameterSetDlg3();
	parameterSet4 = new ParameterSetDlg4();
	parameterSet5 = new ParameterSetDlg5();
	resizingParameterSet1 = new ResizingParameterSetDlg1;
	processViewerDlg = NULL;
}

CMeshEditorDoc::~CMeshEditorDoc()
{
	if(polyMesh != NULL)
		delete polyMesh;
	if(smoother != NULL)
		delete smoother;
	if(resizer != NULL)
		delete resizer;
	if(parameterSet1 != NULL)
		delete parameterSet1;
	if(parameterSet2 != NULL)
		delete parameterSet2;
	if(parameterSet3 != NULL)
		delete parameterSet3;
	if(parameterSet4 != NULL)
		delete parameterSet4;
	if(parameterSet5 != NULL)
		delete parameterSet5;
	if(resizingParameterSet1 != NULL)
		delete resizingParameterSet1;
}

BOOL CMeshEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CMeshEditorDoc serialization

void CMeshEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CMeshEditorDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CMeshEditorDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CMeshEditorDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMeshEditorDoc diagnostics

#ifdef _DEBUG
void CMeshEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMeshEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMeshEditorDoc commands


void CMeshEditorDoc::OnFileOpen()
{
	//打开文件
	CFileDialog fileDlg(TRUE,0,0,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"Ply2 Files(*.ply2)|*.ply2",0); 
	if(fileDlg.DoModal() != IDOK)
		return;
	BeginWaitCursor();     
	CString filePath = fileDlg.GetPathName();            
	FileManager *fileManager = new FileManager();
	if(polyMesh != NULL)
	{
		delete polyMesh;
	}
	polyMesh = new PolyMesh;
	fileManager->openPly2(filePath,polyMesh);
	EndWaitCursor();

	//设置窗口标题
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	title = "FileName: " + fileDlg.GetFileName();
	CString tempString1;
	tempString1.Format("%d",polyMesh->getVertexN());
	CString tempString2;
	tempString2 = title + "   VertexNum: " + tempString1;
	tempString1.Format("%d",polyMesh->getFaceN());
	tempString2 = tempString2 + "   FaceNum: " + tempString1;
	pChild->SetWindowTextA(tempString2);
	//for smoothing
	smoother->setPolyMesh(polyMesh);
	polyMesh->computeVertexLink();                   //计算所有顶点的邻近点及个数
	//for resizing
	resizer->setPolyMesh(polyMesh);
	//设置显示模式，并显示
	mode = FLAT_MODE;
	drawFun();
}

void CMeshEditorDoc::drawFun()
{
	POSITION pos = GetFirstViewPosition();          
	CMeshEditorView* pView = (CMeshEditorView*)GetNextView(pos);
	pView->drawFun();
}

void CMeshEditorDoc::selDrawFun()
{
	POSITION pos = GetFirstViewPosition();
	CMeshEditorView* pView = (CMeshEditorView*)GetNextView(pos);
	switch(mode)
	{
	case FLAT_MODE:
		if(polyMesh == NULL)
			return;
		if(polyMesh->normal_f == NULL)
			polyMesh->computeFaceNormal();
		pView->flatShade(polyMesh);
		break;
	case WIRE_MODE:
		if(polyMesh == NULL)
			return;
		if(polyMesh->normal_f == NULL)
			polyMesh->computeFaceNormal();
		if(polyMesh->normal == NULL)
			polyMesh->computeNormal();
		pView->wireFrame(polyMesh);
		break;
	}
}

void CMeshEditorDoc::OnFlatShade()
{
	if(polyMesh == NULL)
	{
		::AfxMessageBox("Please read file first, then you can show in flat mode!");
		return;
	}
	mode = FLAT_MODE;
	this->drawFun();
}


void CMeshEditorDoc::OnWireFrame()
{
	if(polyMesh == NULL)
	{
		::AfxMessageBox("Please read file first, then you can show in wireframe mode!");
		return;
	}
	mode = WIRE_MODE;
	this->drawFun();
}

//采用拉普拉斯流方法进行平滑
void CMeshEditorDoc::OnSmoothingLaplacianflow()
{
	if(polyMesh == NULL)
	{
		::AfxMessageBox(_T("Please read file first, then you can do Laplacian flow fairing!"));
		return;
	}
	if(parameterSet1->DoModal() == IDOK)
	{
		//设置迭代过程的进程显示
		processViewerDlg = new ProcessViewerDlg();
		POSITION pos = GetFirstViewPosition();
		CMeshEditorView* pView = (CMeshEditorView*)GetNextView(pos);
		processViewerDlg->Create(IDD_PROCESSVIEWER, pView);
		processViewerDlg->processCtrl.SetRange(0, parameterSet1->iters1);
		processViewerDlg->processCtrl.SetStep(1);
		processViewerDlg->processCtrl.SetPos(0);
		processViewerDlg->ShowWindow(SW_SHOW);

		//进行平滑前的一些准备
		double initVolume = polyMesh->computeVolume();   //模型原始的体积
		for(int i=0;i<parameterSet1->iters1;i++)
		{
			smoother->laplacianFlow(parameterSet1->weightSet1,parameterSet1->stepSize1);
			processViewerDlg->processCtrl.StepIt();
			if(parameterSet1->isVolumePreservation1)       //如果保体积
			{
				double curVolume = polyMesh->computeVolume();   //模型当前的体积
				polyMesh->rescale(pow(initVolume/curVolume,1.0/3.0));
			}
			polyMesh->computeFaceNormal();
			polyMesh->computeNormal();
			drawFun();
		}
		processViewerDlg->ShowWindow(SW_HIDE);
		delete processViewerDlg;
	}
}

//采用显式平均曲率流的方法进行平滑
void CMeshEditorDoc::OnSmoothingExplicitmeancurvatureflow()
{
	if(polyMesh == NULL)
	{
		::AfxMessageBox(_T("Please read file first, then you can do implicit curvature flow fairing!"));
		return;
	}

	if(parameterSet2->DoModal() == IDOK) 
	{
		//设置迭代过程的进程显示
		processViewerDlg = new ProcessViewerDlg();
		POSITION pos = GetFirstViewPosition();
		CMeshEditorView* pView = (CMeshEditorView*)GetNextView(pos);
		processViewerDlg->Create(IDD_PROCESSVIEWER, pView);
		processViewerDlg->processCtrl.SetRange(0, parameterSet2->iters2);
		processViewerDlg->processCtrl.SetStep(1);
		processViewerDlg->processCtrl.SetPos(0);
		processViewerDlg->ShowWindow(SW_SHOW);

		//进行平滑前的一些准备
		double initVolume = polyMesh->computeVolume();   //模型原始的体积
		for(int i=0;i<parameterSet2->iters2;i++)
		{
			smoother->explicitMeanCurvatureFlow(parameterSet2->stepSize2);
			processViewerDlg->processCtrl.StepIt();
			if(parameterSet2->isVolumePreservation2)       //如果保体积
			{
				double curVolume = polyMesh->computeVolume();   //模型当前的体积
				polyMesh->rescale(pow(initVolume/curVolume,1.0/3.0));
			}
			polyMesh->computeFaceNormal();
			polyMesh->computeNormal();
			drawFun();
		}
		processViewerDlg->ShowWindow(SW_HIDE);
		delete processViewerDlg;
	}
}

//采用二次拉普拉斯方法进行平滑
void CMeshEditorDoc::OnSmoothingBilaplacianflowbykobbelt()
{
	// TODO: 在此添加命令处理程序代码
	if(polyMesh == NULL)
	{
		::AfxMessageBox(_T("Please read file first, then you can do Bilaplacian flow by Kobbelt fairing!"));
		return;
	}
	if(parameterSet3->DoModal() == IDOK)
	{
		//设置迭代过程的进程显示
		processViewerDlg = new ProcessViewerDlg();
		POSITION pos = GetFirstViewPosition();
		CMeshEditorView* pView = (CMeshEditorView*)GetNextView(pos);
		processViewerDlg->Create(IDD_PROCESSVIEWER, pView);
		processViewerDlg->processCtrl.SetRange(0, parameterSet3->iters3);
		processViewerDlg->processCtrl.SetStep(1);
		processViewerDlg->processCtrl.SetPos(0);
		processViewerDlg->ShowWindow(SW_SHOW);

		//进行平滑前的一些准备
		double initVolume = polyMesh->computeVolume();   //模型原始的体积
		for(int i=0;i<parameterSet3->iters3;i++)
		{
			smoother->bilaplacianFlow(parameterSet3->weigthSet3,parameterSet3->stepSize3);
			processViewerDlg->processCtrl.StepIt();
			if(parameterSet3->isVolumePreservation3)       //如果保体积
			{
				double curVolume = polyMesh->computeVolume();   //模型当前的体积
				polyMesh->rescale(pow(initVolume/curVolume,1.0/3.0));
			}
			polyMesh->computeFaceNormal();
			polyMesh->computeNormal();
			drawFun();
		}
		processViewerDlg->ShowWindow(SW_HIDE);
		delete processViewerDlg;
	}
}

//采用Taubin平滑方法进行平滑
void CMeshEditorDoc::OnSmoothingTaubinsmoothing()
{
	if(polyMesh == NULL)
	{
		::AfxMessageBox(_T("Please read file first, then you can do Taubin smoothing!"));
		return;
	}
	if(parameterSet4->DoModal() == IDOK)
	{
		if(_tstof(parameterSet4->lambda) >= parameterSet4->mu)
		{
			::AfxMessageBox("Lambda must be less than mu, please input lambda again！");
			return;
		}
		//设置迭代过程的进程显示
		processViewerDlg = new ProcessViewerDlg();
		POSITION pos = GetFirstViewPosition();
		CMeshEditorView* pView = (CMeshEditorView*)GetNextView(pos);
		processViewerDlg->Create(IDD_PROCESSVIEWER, pView);
		processViewerDlg->processCtrl.SetRange(0, parameterSet4->iters4);
		processViewerDlg->processCtrl.SetStep(1);
		processViewerDlg->processCtrl.SetPos(0);
		processViewerDlg->ShowWindow(SW_SHOW);

		//进行平滑前的一些准备
		double initVolume = polyMesh->computeVolume();   //模型原始的体积
		for(int i=0;i<parameterSet4->iters4;i++)
		{
			smoother->taubinSmoothing(parameterSet4->weightSet4,_tstof(parameterSet4->lambda),parameterSet4->mu);
			processViewerDlg->processCtrl.StepIt();
			if(parameterSet4->isVolumePreservation4)       //如果保体积
			{
				double curVolume = polyMesh->computeVolume();   //模型当前的体积
				polyMesh->rescale(pow(initVolume/curVolume,1.0/3.0));
			}
			polyMesh->computeFaceNormal();
			polyMesh->computeNormal();
			drawFun();
		}
		processViewerDlg->ShowWindow(SW_HIDE);
		delete processViewerDlg;
	}
}

//使用隐式平均曲率流的方法进行平滑
void CMeshEditorDoc::OnSmoothingImplicitMeancurvatureflow()
{
	if(polyMesh == NULL)
	{
		::AfxMessageBox(_T("Please read file first, then you can do implicit curvature flow fairing!"));
		return;
	}

	if(parameterSet5->DoModal() == IDOK) 
	{
		//设置迭代过程的进程显示
		processViewerDlg = new ProcessViewerDlg();
		POSITION pos = GetFirstViewPosition();
		CMeshEditorView* pView = (CMeshEditorView*)GetNextView(pos);
		processViewerDlg->Create(IDD_PROCESSVIEWER, pView);
		processViewerDlg->processCtrl.SetRange(0, parameterSet5->iters5);
		processViewerDlg->processCtrl.SetStep(1);
		processViewerDlg->processCtrl.SetPos(0);
		processViewerDlg->ShowWindow(SW_SHOW);

		//进行平滑前的一些准备
		double initVolume = polyMesh->computeVolume();   //模型原始的体积
		for(int i=0;i<parameterSet5->iters5;i++)
		{
			smoother->implicitMeanCurvatureFlow(parameterSet5->stepSize5);
			processViewerDlg->processCtrl.StepIt();
			if(parameterSet5->isVolumePreservation5)       //如果保体积
			{
				double curVolume = polyMesh->computeVolume();   //模型当前的体积
				polyMesh->rescale(pow(initVolume/curVolume,1.0/3.0));
			}
			polyMesh->computeFaceNormal();
			polyMesh->computeNormal();
			drawFun();
		}
		processViewerDlg->ShowWindow(SW_HIDE);
		delete processViewerDlg;
	}
}


void CMeshEditorDoc::OnResizingNonhomogenous()
{
	//如果还没有打开文件，首先提示用户打开文件
	if(polyMesh == NULL)
	{
		::AfxMessageBox(_T("Please read file first, then you can do non-homogenous resizing!"));
		return;
	}
	if(resizingParameterSet1->DoModal() == IDOK) 
	{
		printf("开始执行\n");
		//获得三个方向的缩放量
		float s[3]={resizingParameterSet1->resizingOfX,resizingParameterSet1->resizingOfY,resizingParameterSet1->resizingOfZ};
		//计算face和cell的整体的关系
		resizer->computeCellAndFaceRelationship();
		//计算每个cell的vulnerability值
		resizer->computeCellVulnerability();
		//输出每个cell的vulnerability值
		//	resizer->printCellVulnerability();
		resizer->computeCellScale(s);
		//输出每个cell的Phi值
		//resizer->printCellPhi();
		//resizer->printCellScale();
		resizer->computeOldCellVertex();
		resizer->computeNewCellVertex();
		resizer->computeVertexInCell();
		resizer->updateVertex();
		polyMesh->computeNormal();
		polyMesh->computeFaceNormal();
		drawFun();
		printf("执行结束\n");
	}
}
