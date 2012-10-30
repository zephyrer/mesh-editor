
// MeshEditorView.cpp : implementation of the CMeshEditorView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MeshEditor.h"
#endif

#include "MeshEditorDoc.h"
#include "MeshEditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMeshEditorView

IMPLEMENT_DYNCREATE(CMeshEditorView, CView)

BEGIN_MESSAGE_MAP(CMeshEditorView, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CMeshEditorView construction/destruction

CMeshEditorView::CMeshEditorView()
{
	// TODO: add construction code here
	eye[0] = 26;
	eye[1] = -48;
	eye[2] = 40;                    //设置眼睛所在的位置

	center[0] = 0.0;
	center[1] = 0.0;
	center[2] = 0.0;                //设置模型中心点的位置

	Rx = 0;
	Ry = -3;
	Rz = 122;                       //鼠标右键最初始的位置

	vx[0] = 0;
	vx[1] = 0;
	vx[2] = 1;

	vy[0] = -(float)(eye[1]/sqrt(eye[0]*eye[0]+eye[1]*eye[1]));
	vy[1] = -(float)(eye[0]/sqrt(eye[0]*eye[0]+eye[1]*eye[1]));
	vy[2] = 0;

	vz[0] = -vy[1];
	vz[1] = -vy[0];
	vz[2] = 0;

	viewAngle = 22.5;

	isLPressed = false;
	isRPressed = false;

	shiftX = 0;
	shiftY = 0;
	shiftZ = 0;
}

CMeshEditorView::~CMeshEditorView()
{
}

BOOL CMeshEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CMeshEditorView drawing

void CMeshEditorView::OnDraw(CDC* pDC)
{
	CMeshEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	/*设定OpenGL当前线程的渲染环境，以后这个线程所有的OpenGL调用都是在这个HDC标识的设备上绘制，其中参数hglrc是线程当前渲染的上下文*/
	wglMakeCurrent(pDC->m_hDC,mHrc);
	/*清除颜色缓冲和深度缓冲*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*push the current matrix stack*/
	glPushMatrix();
	/*设置模型随着上、下、左、右键而随之运动*/
	glTranslatef(shiftX,shiftY,shiftZ);
	/*设置模型随着保持鼠标左键后运动而运动(即模型随着我们按下鼠标之后运动而旋转)*/
	glRotatef(Rx,vz[0],vz[1],vz[2]);
	glRotatef(Ry,vy[0],vy[1],vy[2]);
	glRotatef(Rz,vx[0],vx[1],vx[2]);
	/*又回到了CMPUDoc类中*/
	pDoc->selDrawFun();                      //根据mode的值选择不同的绘制函数         
	glPopMatrix();
	glFlush();
	SwapBuffers(wglGetCurrentDC());
	wglMakeCurrent(pDC->m_hDC,NULL);
	pDC->SetBkColor(RGB(255,255,255));
	pDC->SetTextColor(RGB(0,0,0));
}


// CMeshEditorView diagnostics

#ifdef _DEBUG
void CMeshEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CMeshEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMeshEditorDoc* CMeshEditorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMeshEditorDoc)));
	return (CMeshEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CMeshEditorView message handlers


int CMeshEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	/*像素格式明确了OpenGL绘制平面的特性*/
	PIXELFORMATDESCRIPTOR pFd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		16,0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,0,
		PFD_MAIN_PLANE,0,0,0,0
	};
	CClientDC mDC = CClientDC(this);
	int pixelFormat = ChoosePixelFormat(mDC.m_hDC,&pFd);
	if(!SetPixelFormat(mDC.m_hDC,pixelFormat,&pFd))
		return -1;
	mHrc = wglCreateContext(mDC.m_hDC);
	wglMakeCurrent(mDC.m_hDC,mHrc);
	initOpenGL();
	wglMakeCurrent(mDC.m_hDC,NULL);
	return 0;
}


void CMeshEditorView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	wglDeleteContext(mHrc);
}


void CMeshEditorView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	int i;
	GLdouble up[3];
	GLdouble vector[3];
	GLdouble normal;

	CClientDC mDC = CClientDC(this);

	wglMakeCurrent(mDC.m_hDC,mHrc);
	glViewport(0, 0, cx, cy);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(viewAngle,(GLfloat)cx/(GLfloat)cy,10.0,4000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for(i=0; i<3; i++)
		vector[i] = center[i] - eye[i];

	up[0] =  vector[0]*vector[2];
	up[1] =  vector[1]*vector[2];
	up[2] =  vector[0]*vector[0] + vector[1]*vector[1];

	normal = up[0]*up[0] + up[1]*up[1] + up[2]*up[2];
	normal = sqrt(normal);
	for(i=0; i<3; i++)
		up[i] = up[i]/normal;

	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2],up[0], up[1], up[2]);

	wglMakeCurrent(mDC.m_hDC, NULL);
}


void CMeshEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	isLPressed = true;
	px = point.x;
	py = point.y;
	CView::OnLButtonDown(nFlags, point);
}


void CMeshEditorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	isLPressed = false;
	CView::OnLButtonUp(nFlags, point);
}


void CMeshEditorView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	isRPressed = true;
	px = point.x;
	py = point.y;
	CView::OnRButtonDown(nFlags, point);
}


void CMeshEditorView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	isRPressed = false;
	CView::OnRButtonUp(nFlags, point);
}


void CMeshEditorView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (isLPressed)                //查看左键是否被按下,按下则旋转模型
	{
		int tempX = point.x;           
		int tempY = point.y;          //当前鼠标的位置坐标
		Rz = Rz + (tempX-px)/3;
		Ry = Ry + (tempY-py)/3;
		px = tempX;
		py = tempY;
		CClientDC* mDC = new CClientDC(this);
		OnDraw(mDC);
		delete mDC;
	} 
	else if(isRPressed)
	{
		int tempX = point.x;
		int tempY = point.y;
		viewAngle = viewAngle+(tempY-py)/20.0f;
		px = tempX;
		py = tempY;
		CClientDC* mDC = new CClientDC(this);
		RECT* mRect = new RECT;
		GetClientRect(mRect);
		OnSize(SIZE_RESTORED,mRect->right,mRect->bottom);
		delete mRect;
		OnDraw(mDC);
		delete mDC;
	}
	CView::OnMouseMove(nFlags, point);
}


void CMeshEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nChar)
	{
	case VK_UP:
		shiftZ = shiftZ+5;
		break;
	case VK_DOWN:
		shiftZ = shiftZ-5;
		break;
	case VK_RIGHT:
		shiftX = shiftX+5;
		break;
	case VK_LEFT:
		shiftX = shiftX-5;
		break;
	}
	CClientDC* mDC = new CClientDC(this); 
	OnDraw(mDC);
	delete mDC;
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

/*显示之前设置OpenGL的显示环境*/
void CMeshEditorView::initOpenGL()
{
	GLfloat light_position1[4] = {-52, -16, -50, 0};
	GLfloat light_position2[4] = {-26, -48, -50, 0};
	GLfloat light_position3[4] = { 16, -52, -50, 0};

	GLfloat direction1[3] = {52,16,50};
	GLfloat direction2[3] = {26,48,50};
	GLfloat direction3[3] = {-16,52,50};

	GLfloat light_position4[4] = {52, 16, 50, 0};
	GLfloat light_position5[4] = {26, 48, 50, 0};
	GLfloat light_position6[4] = {-16, 52, 50, 0};

	GLfloat direction4[3] = {-52,-16,-50};
	GLfloat direction5[3] = {-26,-48,-50};
	GLfloat direction6[3] = {16,-52,-50};

	GLfloat color1[4] = {1,0,0,1};
	GLfloat color2[4] = {0,1,0,1};
	GLfloat color3[4] = {0,0,1,1};

	GLfloat color4[4] = {1,0,0,1};
	GLfloat color5[4] = {0,1,0,1};
	GLfloat color6[4] = {0,0,1,1};

	//设置材质属性
	GLfloat ambient[4] = {0.3f,0.3f,0.3f,0.5f};

	GLfloat material_color[4] = {1,1,1,0.3f};                    //设置材质颜色属性
	GLfloat material_specular[4] = {0.5,0.5,0.5,0.5};            //设置材质镜面属性
	GLfloat material_ambient[4] = {0.0,0.0,0.0,0.0};                      

	glClearColor(1,1,1,0);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);          //设置0号光源的位置
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction1);         //设置0号光源的方向
	glLightfv(GL_LIGHT0, GL_DIFFUSE, color1);                    //散射光成分
	glLightfv(GL_LIGHT0, GL_SPECULAR, color1);                   //镜面光成分

	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, color2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, color2);

	glLightfv(GL_LIGHT2, GL_POSITION, light_position3);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, direction3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, color3);
	glLightfv(GL_LIGHT2, GL_SPECULAR, color3);

	glLightfv(GL_LIGHT3, GL_POSITION, light_position4);
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, direction4);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, color4);
	glLightfv(GL_LIGHT3, GL_SPECULAR, color4);

	glLightfv(GL_LIGHT4, GL_POSITION, light_position5);
	glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, direction5);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, color5);
	glLightfv(GL_LIGHT4, GL_SPECULAR, color5);

	glLightfv(GL_LIGHT5, GL_POSITION, light_position6);
	glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, direction6);
	glLightfv(GL_LIGHT5, GL_DIFFUSE, color6);
	glLightfv(GL_LIGHT5, GL_SPECULAR, color6);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);     //环境光成分

	//设置模型的材料属性
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
	//第一个参数指定了是哪个表面需要被渲染，
	//第二个参数是哪个材料属性将要被设置，
	//第三个参数是指定哪种颜色的光会被反射
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_color);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_ambient);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128);

	glEnable(GL_LIGHTING);           //设置允许光照

	glEnable(GL_LIGHT3);
	glEnable(GL_LIGHT4);
	glEnable(GL_LIGHT5);

	glEnable(GL_DEPTH_TEST);        // 用来开启更新深度缓冲区的功能启动它，OpenGL就可以跟踪再Z轴上的像素
	//这样，它只会再那个像素前方没有东西时，才会绘画这个像素
	glDepthFunc(GL_LESS);

	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glLineWidth(0.5);               //设置线宽
	glPointSize(2.0);               //设置点的大小
}

void CMeshEditorView::drawFun()
{
	CClientDC* mDC = new CClientDC(this);
	this->OnDraw(mDC);
	delete mDC;
}

void CMeshEditorView::flatShade(PolyMesh* _polyMesh)
{
	glShadeModel(GL_SMOOTH);

	int face_N = _polyMesh->getFaceN();
	int (*face)[3] = _polyMesh->face;
	float (*normal_f)[3] = _polyMesh->normal_f;
	float (*vertex)[3] = _polyMesh->vertex;
	for(int i=0; i<face_N; i++)
	{
		if(face[i][0] < 0)
			continue;
		glBegin(GL_POLYGON);
		glNormal3fv(normal_f[i]);
		glVertex3fv(vertex[face[i][0]]);
		glVertex3fv(vertex[face[i][1]]);
		glVertex3fv(vertex[face[i][2]]);
		glEnd();
	}
}

void CMeshEditorView::wireFrame(PolyMesh* _polyMesh)
{
	int vertex_N = _polyMesh->getVertexN();
	int face_N = _polyMesh->getFaceN();
	int (*face)[3] = _polyMesh->face;
	float (*normal_f)[3] = _polyMesh->normal_f;
	float (*normal)[3] = _polyMesh->normal;
	float (*vertex)[3] = _polyMesh->vertex;

	glShadeModel(GL_SMOOTH);
	glDisable(GL_LIGHTING);

	glColor3f(0.0f, 0.0f, 0.0f);
	for(int i=0; i<face_N; i++)
	{
		if(face[i][0] < 0)
			continue;

		if(face[i][0] > face[i][1])
		{
			glBegin(GL_LINES);
			glVertex3fv(vertex[face[i][0]]);
			glVertex3fv(vertex[face[i][1]]);
			glEnd();
		}

		if(face[i][1] > face[i][2])
		{
			glBegin(GL_LINES);
			glVertex3fv(vertex[face[i][1]]);
			glVertex3fv(vertex[face[i][2]]);
			glEnd();
		}

		if(face[i][2] > face[i][0])
		{
			glBegin(GL_LINES);
			glVertex3fv(vertex[face[i][2]]);
			glVertex3fv(vertex[face[i][0]]);
			glEnd();
		}
	}
	glEnable(GL_LIGHTING);

	float d = (float)(0.05*viewAngle/22.5);

	for(int i=0; i<face_N; i++)
	{
		if(face[i][0] < 0)
			continue;

		float p1[3], p2[3], p3[3];

		//下面重新计算平面对应的点位置，主要是为了防止模型从Flat Shade转到WireFrame时模型视角变化
		p1[0] = vertex[face[i][0]][0] - d*normal[face[i][0]][0];
		p1[1] = vertex[face[i][0]][1] - d*normal[face[i][0]][1];
		p1[2] = vertex[face[i][0]][2] - d*normal[face[i][0]][2];

		p2[0] = vertex[face[i][1]][0] - d*normal[face[i][1]][0];
		p2[1] = vertex[face[i][1]][1] - d*normal[face[i][1]][1];
		p2[2] = vertex[face[i][1]][2] - d*normal[face[i][1]][2];

		p3[0] = vertex[face[i][2]][0] - d*normal[face[i][2]][0];
		p3[1] = vertex[face[i][2]][1] - d*normal[face[i][2]][1];
		p3[2] = vertex[face[i][2]][2] - d*normal[face[i][2]][2];

		glBegin(GL_POLYGON);
		glNormal3fv(normal_f[i]);
		glVertex3fv(p1);
		glVertex3fv(p2);
		glVertex3fv(p3);
		glEnd();
	}
}
