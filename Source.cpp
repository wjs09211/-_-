#include <afxwin.h>
#include <atlimage.h>
#include <afxdlgs.h>
#include <afxext.h>
#include "resource.h"
class MyDialog: public CDialog
{private:
	int IsDash;
	int Fill;
	int size;
public:
	MyDialog()
	{	IsDash=0;
		Fill=0;
	}
	void DoDataExchange(CDataExchange *pDX)
	{	DDX_Check(pDX,IDC_CHECK1,IsDash);
		DDX_Check(pDX,IDC_CHECK2,Fill);
		DDX_CBIndex(pDX,IDC_COMBO1,size);
	}
	BOOL OnCommand(WPARAM wParam,LPARAM lParam);
};
class MyDialog2 :public CDialog
{private:
	int width;
	int heigh;
public:
	int GetWidth(){return width;}
	int GetHeigh(){return heigh;}
	MyDialog2(CWnd *parent)
		:CDialog(IDD_DIALOG1,parent)
	{	width=500;
		heigh=400;
	}
void DoDataExchange(CDataExchange *pDX)
	{
		DDX_Text(pDX,IDC_EDIT1,width);
		DDX_Text(pDX,IDC_EDIT2,heigh);
		DDV_MinMaxInt(pDX,width,200,2000);
		DDV_MinMaxInt(pDX,heigh,200,2000);
	}
};

enum state{ Line, Square, Circle, Pen, Eraser };

class MyFrame :public CFrameWnd
{
private:
	state Draw;
	CMenu *pMenu;
	CToolBar toolBar;
	CImage bitmap;
	CImage oldBit;
	CImage reBit;
	bool undoAble;
	bool redoAble;
	CPoint bPoint, ePoint;
	COLORREF LColor,FColor;
	CString pathname;
	int SolidDash;
	int Fill;
	MyDialog* pdlg;
	int width;
	int height;
	int size;
	CRect rect;
public:
	MyFrame()
	{	
		Create(NULL, "Hello MFC");
		pMenu=new CMenu();
		pMenu->LoadMenu(IDR_MENU1);
		SetMenu(pMenu);
		toolBar.Create(this);
		toolBar.LoadToolBar(IDR_TOOLBAR1);
		toolBar.SetBarStyle(CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY);

		pdlg= new MyDialog();
		pdlg->Create(IDD_DIALOGBAR,this);
		pdlg->UpdateData(FALSE);
		pdlg->ShowWindow(SW_SHOW);
		LColor=RGB(255,0,0);
		FColor=RGB(255,0,0);
		SolidDash=PS_SOLID;
		Fill = 0;
		

		redoAble = false;
		undoAble = false;
		size = 1;
		width=500;
		height=400;
		bitmap.Create(2000,2000,24);
		oldBit.Create(2000,2000,24);
		reBit.Create(2000,2000,24);
		CDC* aDC=CDC::FromHandle(bitmap.GetDC());
		CBrush brush(RGB(255,255,255));
		rect.SetRect(-1,-1,501,401);
		aDC->FillRect(CRect(0,0,500,400),&brush);
		bitmap.ReleaseDC();
	}
	void SetSolid(int s){SolidDash=s;}
	void SetFill(int f){ Fill=f;}
	void SetSize(int n){ size=n;}
	afx_msg void OnPaint()
	{	CPaintDC dc(this);
		dc.Rectangle( rect );
	  	bitmap.BitBlt(dc,0,0,width,height,0,0,SRCCOPY);
		pdlg->ShowWindow(SW_SHOW);
	}

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point)
	{	if ( rect.PtInRect( point ) ) {
		SetCapture();
		bPoint=point;
		ePoint=point;
		if ( Draw == Pen || Draw == Eraser )
		{
			bitmap.BitBlt(oldBit.GetDC(),0,0,width,height,0,0,SRCCOPY);
			undoAble=true;
			redoAble=false;
			oldBit.ReleaseDC();
		}
		if ( Draw == Eraser ) {
			CDC* aDC=CDC::FromHandle(bitmap.GetDC());
			CPen pen2(SolidDash,size,RGB( 255,255,255 ));
			CBrush brush( RGB( 255,255,255 ) );
			CClientDC dc2(this);
			aDC->SelectObject(&pen2);
			dc2.SelectObject( &pen2 );
			aDC->SelectObject(&brush);
			dc2.SelectObject( &brush );
			dc2.Rectangle(point.x-size,point.y-size,point.x+size,point.y+size);
			aDC->Rectangle(point.x-size,point.y-size,point.x+size,point.y+size);
			bitmap.ReleaseDC();
		}
	}
	}
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point)
	{	if (GetCapture()==this){
		if ( Draw != Pen && Draw != Eraser) {
			bitmap.BitBlt(oldBit.GetDC(),0,0,width,height,0,0,SRCCOPY);
			undoAble=true;
			redoAble=false;
			oldBit.ReleaseDC();
		}
		CDC* aDC=CDC::FromHandle(bitmap.GetDC());
		CPen pen(SolidDash,size,LColor);
		aDC->SelectObject(&pen);
		
		if ( Draw == Line ) { //畫線
			aDC->MoveTo(bPoint);
			aDC->LineTo(point);
		}
		else if ( Draw == Square ) {
			if ( Fill == 1 ) {
				CBrush brush( FColor );
				aDC->SelectObject(&brush);
				aDC->Rectangle( bPoint.x, bPoint.y, point.x, point.y );
			}
			else if ( Fill == 0 ) {
				aDC->MoveTo(bPoint);
				aDC->LineTo(point.x,bPoint.y);
				aDC->LineTo(point);
				aDC->LineTo(bPoint.x,point.y);
				aDC->LineTo(bPoint);
			}
		}
		else if ( Draw == Circle ) {
			if ( Fill == 1 ) {
				CBrush brush( FColor );
				aDC->SelectObject(&brush);
				aDC->Ellipse( bPoint.x, bPoint.y, point.x, point.y );
			}
			else if ( Fill == 0 ) {
				aDC->Arc(bPoint.x,bPoint.y,point.x,point.y,0,0,0,0);
			}
		}
		bitmap.ReleaseDC();
		Invalidate();
		ReleaseCapture();
	}
	}
	afx_msg void OnMouseMove(UINT nFlags, CPoint point)
	{	if (GetCapture()==this){
			CClientDC dc(this);
			CPen pen(SolidDash,size,LColor);
			dc.SelectObject(&pen);
			dc.SetROP2(R2_NOTXORPEN);

			if ( Draw == Line ) {
				dc.MoveTo(bPoint);
				dc.LineTo(ePoint);
				dc.MoveTo(bPoint);
				dc.LineTo(point);
				ePoint=point;
			}
			else if ( Draw == Square ) {
				dc.MoveTo(bPoint);
				dc.LineTo(ePoint.x,bPoint.y);
				dc.LineTo(ePoint);
				dc.LineTo(bPoint.x,ePoint.y);
				dc.LineTo(bPoint);
				dc.MoveTo(bPoint);
				dc.LineTo(point.x,bPoint.y);
				dc.LineTo(point);
				dc.LineTo(bPoint.x,point.y);
				dc.LineTo(bPoint);
				ePoint=point;
			}
			else if ( Draw == Circle ) {
				dc.Arc(bPoint.x,bPoint.y,ePoint.x,ePoint.y,0,0,0,0);
				dc.Arc(bPoint.x,bPoint.y,point.x,point.y,0,0,0,0);
				ePoint=point;
			}
			else if ( Draw == Pen ){
				CDC* aDC=CDC::FromHandle(bitmap.GetDC());
				CPen pen2(SolidDash,size,LColor);
				CClientDC dc2(this);
				dc2.SelectObject(&pen2);
				aDC->SelectObject(&pen2);
				CBrush brush(LColor);
				dc2.SelectObject(&brush);
				aDC->SelectObject(&brush);
				aDC->Ellipse( point.x-size,point.y-size,point.x+size,point.y+size );
				dc2.Ellipse( point.x-size,point.y-size,point.x+size,point.y+size );
				bitmap.ReleaseDC();
			}
			else if ( Draw == Eraser ){				
				CDC* aDC=CDC::FromHandle(bitmap.GetDC());
				CPen pen2(SolidDash,size,RGB( 255,255,255 ));
				CBrush brush( RGB( 255,255,255 ) );
				CClientDC dc2(this);
				aDC->SelectObject(&pen2);
				dc2.SelectObject( &pen2 );
				aDC->SelectObject(&brush);
				dc2.SelectObject( &brush );
				dc2.Rectangle(point.x-size,point.y-size,point.x+size,point.y+size);
				aDC->Rectangle(point.x-size,point.y-size,point.x+size,point.y+size);
				bitmap.ReleaseDC();
			}
		}
	}

	afx_msg void OnSave()
	{	CFileDialog fdlg(false,"gif","picture");
		if (fdlg.DoModal()==IDOK)
		{	
			bitmap.Save(fdlg.GetPathName());
		}
	}
	afx_msg void OnLoad()
	{	CFileDialog fdlg(true,"gif","picture");
		if (fdlg.DoModal()==IDOK)
		{
			CImage buffer;
			buffer.Load(fdlg.GetPathName());
			buffer.BitBlt(bitmap.GetDC(),0,0,width,height,0,0,SRCCOPY);
			bitmap.ReleaseDC();
			Invalidate();
		}
	}
	afx_msg void OnOptions()
	{
		MyDialog2 dlg(this);
		if (dlg.DoModal()==IDOK)
		{	
			/*bitmap.Create(width,height,24);
		oldBit.Create(width,height,24);
		reBit.Create(width,height,24);
		CDC* aDC=CDC::FromHandle(bitmap.GetDC());
		CBrush brush(RGB(255,255,255));
		rect.SetRect(0,100,500,400);
		aDC->FillRect(CRect(0,0,500,400),&brush);
		aDC->Rectangle(rect);
		bitmap.ReleaseDC();*/
			CImage buffer;
			buffer.Create( width,height,24 );
			bitmap.BitBlt(buffer.GetDC(),0,0,width,height,0,0,SRCCOPY);
			width=dlg.GetWidth();
			height=dlg.GetHeigh();
			CDC* aDC=CDC::FromHandle(bitmap.GetDC());
			CBrush brush(RGB(255,255,255));
			aDC->FillRect(CRect(0,0,width,height),&brush);
			buffer.BitBlt(bitmap.GetDC(),0,0,width,height,0,0,SRCCOPY);
			bitmap.ReleaseDC();
			buffer.ReleaseDC();
			rect.SetRect(-1,-1,width+1,height+1);
			Invalidate();
			undoAble=false;
			redoAble = false;
		}


	}
	void OnColorFull()
	{
	CColorDialog cdlg(RGB(255,0,0),CC_FULLOPEN);
	if (cdlg.DoModal()==IDOK)
		FColor=cdlg.GetColor();
	// TODO: 在此加入您的命令處理常式程式碼
	}
	void OnColorLine()
	{
		CColorDialog cdlg(RGB(255,0,0),CC_FULLOPEN);
		if (cdlg.DoModal()==IDOK)
		LColor=cdlg.GetColor();

	// TODO: 在此加入您的命令處理常式程式碼
	}
	afx_msg void OnUnDo()
	{	bitmap.BitBlt(reBit.GetDC(),0,0,width,height,0,0,SRCCOPY);
		oldBit.BitBlt(bitmap.GetDC(),0,0,width,height,0,0,SRCCOPY);
		undoAble=false;
		redoAble = true;
		bitmap.ReleaseDC();
		reBit.ReleaseDC();
		Invalidate();
	}
	afx_msg void UpdateUnDo(CCmdUI* aCmdUI)
	{	aCmdUI->Enable(undoAble);
	}
	afx_msg void OnReDo()
	{	reBit.BitBlt(bitmap.GetDC(),0,0,width,height,0,0,SRCCOPY);
		redoAble=false;
		undoAble = true;
		bitmap.ReleaseDC();
		Invalidate();
	}
	afx_msg void UpdateReDo(CCmdUI* aCmdUI)
	{	aCmdUI->Enable(redoAble);
	}
	afx_msg void Draw_Line()
	{
		Draw = Line;
	}
	afx_msg void Draw_Square()
	{
		Draw = Square;
	}
	afx_msg void Draw_Circle()
	{
		Draw = Circle;
	}
	afx_msg void Draw_Pen()
	{
		Draw = Pen;
	}
	afx_msg void Draw_Eraser()
	{
		Draw = Eraser;
	}
	afx_msg void UpdateLine(CCmdUI* aCmdUI)
	{	
		aCmdUI->SetCheck( Draw == Line);
	}
	afx_msg void UpdateSquare(CCmdUI* aCmdUI)
	{	
		aCmdUI->SetCheck( Draw == Square);
	}
	afx_msg void UpdateCircle(CCmdUI* aCmdUI)
	{	
		aCmdUI->SetCheck( Draw == Circle);
	}
	afx_msg void UpdatePen(CCmdUI* aCmdUI)
	{	
		aCmdUI->SetCheck( Draw == Pen);
	}
	afx_msg void UpdateEraser(CCmdUI* aCmdUI)
	{	
		aCmdUI->SetCheck( Draw == Eraser);
	}

DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(MyFrame,CFrameWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_COLOR_LINE, OnColorLine)
	ON_COMMAND(ID_COLOR_FILL, OnColorFull)
	ON_COMMAND(ID_EDIT_UNDO40006,OnUnDo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO40006,UpdateUnDo)
	ON_COMMAND(ID_EDIT_REDO40007,OnReDo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO40007,UpdateReDo)
	ON_COMMAND(ID_BUTTON40010, Draw_Line )
	ON_COMMAND(ID_BUTTON40011, Draw_Square )
	ON_COMMAND(ID_BUTTON40014, Draw_Circle )
	ON_COMMAND(ID_BUTTON40020, Draw_Pen )
	ON_COMMAND(ID_BUTTON40022, Draw_Eraser )
	ON_COMMAND(ID_FILE_SAVE40001, OnSave )
	ON_COMMAND(ID_FILE_LOAD, OnLoad )
	ON_COMMAND(ID_IMAGE_OPTIONS, OnOptions )
	ON_UPDATE_COMMAND_UI(ID_BUTTON40010,UpdateLine)
	ON_UPDATE_COMMAND_UI(ID_BUTTON40011,UpdateSquare)
	ON_UPDATE_COMMAND_UI(ID_BUTTON40014,UpdateCircle)
	ON_UPDATE_COMMAND_UI(ID_BUTTON40020,UpdatePen)
	ON_UPDATE_COMMAND_UI(ID_BUTTON40022,UpdateEraser)
	
END_MESSAGE_MAP()


class MyApp :public CWinApp
{public:
	BOOL InitInstance()
	{	CFrameWnd *frame= new MyFrame();
		m_pMainWnd=frame;
		frame->ShowWindow(SW_SHOW);
		return true;
	}
};
MyApp app;

 BOOL MyDialog::OnCommand(WPARAM wParam,LPARAM lParam)
	{	UpdateData();
		((MyFrame *) AfxGetMainWnd())->SetSolid(IsDash);
		((MyFrame *) AfxGetMainWnd())->SetFill(Fill);
		((MyFrame *) AfxGetMainWnd())->SetSize(size+1);
		return CDialog::OnCommand(wParam,lParam);
	}
