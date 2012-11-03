
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
END_MESSAGE_MAP()

//For console:print some information
//=====================================================================================
/*#include <io.h>  
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
}*/
//=====================================================================================

// CMeshEditorDoc construction/destruction

CMeshEditorDoc::CMeshEditorDoc()
{
//	InitConsole();
	polyMesh = NULL;
	smoother = new Smoother();
	parameterSet1 = new ParameterSetDlg1();
	parameterSet2 = new ParameterSetDlg2();
	parameterSet3 = new ParameterSetDlg3();
	parameterSet4 = new ParameterSetDlg4();
	parameterSet5 = new ParameterSetDlg5();
	processViewerDlg = NULL;
}

CMeshEditorDoc::~CMeshEditorDoc()
{
	if(polyMesh != NULL)
		delete polyMesh;
	if(smoother != NULL)
		delete smoother;
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
	//���ļ�
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

	//���ô��ڱ���
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

	smoother->setPolyMesh(polyMesh);
	polyMesh->computeVertexLink();                   //�������ж�����ڽ��㼰����
	//������ʾģʽ������ʾ
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

//����������˹����������ƽ��
void CMeshEditorDoc::OnSmoothingLaplacianflow()
{
	if(polyMesh == NULL)
	{
		::AfxMessageBox(_T("Please read file first, then you can do Laplacian flow fairing!"));
		return;
	}
	if(parameterSet1->DoModal() == IDOK)
	{
		//���õ������̵Ľ�����ʾ
		processViewerDlg = new ProcessViewerDlg();
		POSITION pos = GetFirstViewPosition();
		CMeshEditorView* pView = (CMeshEditorView*)GetNextView(pos);
		processViewerDlg->Create(IDD_PROCESSVIEWER, pView);
		processViewerDlg->processCtrl.SetRange(0, parameterSet1->iters1);
		processViewerDlg->processCtrl.SetStep(1);
		processViewerDlg->processCtrl.SetPos(0);
		processViewerDlg->ShowWindow(SW_SHOW);

		//����ƽ��ǰ��һЩ׼��
		double initVolume = polyMesh->computeVolume();   //ģ��ԭʼ�����
		for(int i=0;i<parameterSet1->iters1;i++)
		{
			smoother->laplacianFlow(parameterSet1->weightSet1,parameterSet1->stepSize1);
			processViewerDlg->processCtrl.StepIt();
			if(parameterSet1->isVolumePreservation1)       //��������
			{
				double curVolume = polyMesh->computeVolume();   //ģ�͵�ǰ�����
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

//������ʽƽ���������ķ�������ƽ��
void CMeshEditorDoc::OnSmoothingExplicitmeancurvatureflow()
{
	if(polyMesh == NULL)
	{
		::AfxMessageBox(_T("Please read file first, then you can do implicit curvature flow fairing!"));
		return;
	}

	if(parameterSet2->DoModal() == IDOK) 
	{
		//���õ������̵Ľ�����ʾ
		processViewerDlg = new ProcessViewerDlg();
		POSITION pos = GetFirstViewPosition();
		CMeshEditorView* pView = (CMeshEditorView*)GetNextView(pos);
		processViewerDlg->Create(IDD_PROCESSVIEWER, pView);
		processViewerDlg->processCtrl.SetRange(0, parameterSet2->iters2);
		processViewerDlg->processCtrl.SetStep(1);
		processViewerDlg->processCtrl.SetPos(0);
		processViewerDlg->ShowWindow(SW_SHOW);

		//����ƽ��ǰ��һЩ׼��
		double initVolume = polyMesh->computeVolume();   //ģ��ԭʼ�����
		for(int i=0;i<parameterSet2->iters2;i++)
		{
			smoother->explicitMeanCurvatureFlow(parameterSet2->stepSize2);
			processViewerDlg->processCtrl.StepIt();
			if(parameterSet2->isVolumePreservation2)       //��������
			{
				double curVolume = polyMesh->computeVolume();   //ģ�͵�ǰ�����
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

//���ö���������˹��������ƽ��
void CMeshEditorDoc::OnSmoothingBilaplacianflowbykobbelt()
{
	// TODO: �ڴ�����������������
	if(polyMesh == NULL)
	{
		::AfxMessageBox(_T("Please read file first, then you can do Bilaplacian flow by Kobbelt fairing!"));
		return;
	}
	if(parameterSet3->DoModal() == IDOK)
	{
		//���õ������̵Ľ�����ʾ
		processViewerDlg = new ProcessViewerDlg();
		POSITION pos = GetFirstViewPosition();
		CMeshEditorView* pView = (CMeshEditorView*)GetNextView(pos);
		processViewerDlg->Create(IDD_PROCESSVIEWER, pView);
		processViewerDlg->processCtrl.SetRange(0, parameterSet3->iters3);
		processViewerDlg->processCtrl.SetStep(1);
		processViewerDlg->processCtrl.SetPos(0);
		processViewerDlg->ShowWindow(SW_SHOW);

		//����ƽ��ǰ��һЩ׼��
		double initVolume = polyMesh->computeVolume();   //ģ��ԭʼ�����
		for(int i=0;i<parameterSet3->iters3;i++)
		{
			smoother->bilaplacianFlow(parameterSet3->weigthSet3,parameterSet3->stepSize3);
			processViewerDlg->processCtrl.StepIt();
			if(parameterSet3->isVolumePreservation3)       //��������
			{
				double curVolume = polyMesh->computeVolume();   //ģ�͵�ǰ�����
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

//����Taubinƽ����������ƽ��
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
			::AfxMessageBox("Lambda must be less than mu, please input lambda again��");
			return;
		}
		//���õ������̵Ľ�����ʾ
		processViewerDlg = new ProcessViewerDlg();
		POSITION pos = GetFirstViewPosition();
		CMeshEditorView* pView = (CMeshEditorView*)GetNextView(pos);
		processViewerDlg->Create(IDD_PROCESSVIEWER, pView);
		processViewerDlg->processCtrl.SetRange(0, parameterSet4->iters4);
		processViewerDlg->processCtrl.SetStep(1);
		processViewerDlg->processCtrl.SetPos(0);
		processViewerDlg->ShowWindow(SW_SHOW);

		//����ƽ��ǰ��һЩ׼��
		double initVolume = polyMesh->computeVolume();   //ģ��ԭʼ�����
		for(int i=0;i<parameterSet4->iters4;i++)
		{
			smoother->taubinSmoothing(parameterSet4->weightSet4,_tstof(parameterSet4->lambda),parameterSet4->mu);
			processViewerDlg->processCtrl.StepIt();
			if(parameterSet4->isVolumePreservation4)       //��������
			{
				double curVolume = polyMesh->computeVolume();   //ģ�͵�ǰ�����
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

//ʹ����ʽƽ���������ķ�������ƽ��
void CMeshEditorDoc::OnSmoothingImplicitMeancurvatureflow()
{
	if(polyMesh == NULL)
	{
		::AfxMessageBox(_T("Please read file first, then you can do implicit curvature flow fairing!"));
		return;
	}

	if(parameterSet5->DoModal() == IDOK) 
	{
		//���õ������̵Ľ�����ʾ
		processViewerDlg = new ProcessViewerDlg();
		POSITION pos = GetFirstViewPosition();
		CMeshEditorView* pView = (CMeshEditorView*)GetNextView(pos);
		processViewerDlg->Create(IDD_PROCESSVIEWER, pView);
		processViewerDlg->processCtrl.SetRange(0, parameterSet5->iters5);
		processViewerDlg->processCtrl.SetStep(1);
		processViewerDlg->processCtrl.SetPos(0);
		processViewerDlg->ShowWindow(SW_SHOW);

		//����ƽ��ǰ��һЩ׼��
		double initVolume = polyMesh->computeVolume();   //ģ��ԭʼ�����
		for(int i=0;i<parameterSet5->iters5;i++)
		{
			smoother->implicitMeanCurvatureFlow(parameterSet5->stepSize5);
			processViewerDlg->processCtrl.StepIt();
			if(parameterSet5->isVolumePreservation5)       //��������
			{
				double curVolume = polyMesh->computeVolume();   //ģ�͵�ǰ�����
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