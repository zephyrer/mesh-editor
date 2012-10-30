
// MeshEditorView.h : interface of the CMeshEditorView class
//

#pragma once

#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>

class CMeshEditorView : public CView
{
protected: // create from serialization only
	CMeshEditorView();
	DECLARE_DYNCREATE(CMeshEditorView)

// Attributes
public:
	CMeshEditorDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CMeshEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	//�Զ������
public:
	HGLRC mHrc;            
	GLdouble eye[3];          //�����۾����ڵĵ�
	GLdouble center[3];       //����ģ�����ĵ��λ��
	GLfloat Rx,Ry,Rz;         
	int px,py;                //������һ�ΰ�����������Ҽ�������ֵ
	float vx[3],vy[3],vz[3];
	bool isLPressed,isRPressed;
	float viewAngle;
	float shiftX,shiftY,shiftZ;  //��¼���������Ҽ�ʱģ���ƶ���λ��

	//�Զ��庯��
public:
	void drawFun();
	void initOpenGL();                                   //��ʾ֮ǰ����OpenGL����ʾ����
	void flatShade(PolyMesh* mesh);
	void wireFrame(PolyMesh* mesh);

	//�Զ�����Ϣ������
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in MeshEditorView.cpp
inline CMeshEditorDoc* CMeshEditorView::GetDocument() const
   { return reinterpret_cast<CMeshEditorDoc*>(m_pDocument); }
#endif

