
// grimpanDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "grimpan.h"
#include "grimpanDlg.h"
#include "afxdialogex.h"
#include "CMyForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CgrimpanDlg 대화 상자

CgrimpanDlg::CgrimpanDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_GRIMPAN_DIALOG, pParent)
	, m_isAddNewObject(false)
	, m_isInBound(false)
	, m_isLButtonUp(false)
	, m_isCropping(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CgrimpanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CgrimpanDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_FIGURES_LINE, &CgrimpanDlg::OnFiguresLine)
	ON_COMMAND(ID_FIGURES_RECTANGLE, &CgrimpanDlg::OnFiguresRectangle)
	ON_COMMAND(ID_FIGURES_CROPBOX, &CgrimpanDlg::OnFiguresCropbox)
	ON_COMMAND(ID_FILE_EXIT, &CgrimpanDlg::OnFileExit)
	ON_COMMAND(ID_FILE_OPEN, &CgrimpanDlg::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, &CgrimpanDlg::OnFileSave)
	ON_COMMAND(ID_FILE_NEW, &CgrimpanDlg::OnFileNew)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CgrimpanDlg 메시지 처리기

BOOL CgrimpanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	initializeCanvas();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CgrimpanDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	if (m_pForm != NULL)
	{
		m_pForm->DestroyWindow();
	}
}

void CgrimpanDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CgrimpanDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(GetDlgItem(IDC_STATIC_RECT)); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		OnDrawImage();
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CgrimpanDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CgrimpanDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_figure == nullptr)
		return;

	if (point.x < 0 || point.x > m_canvasDuringDraw->GetWidth() || point.y < 0 || point.y > m_canvasDuringDraw->GetHeight())
		return;

	if (nFlags == MK_LBUTTON && m_isAddNewObject == true)
	{
		m_figure->SetPoint(1, point.x, point.y);

		Invalidate(FALSE);
	}
	else if (nFlags == MK_LBUTTON && m_isAddNewObject == false && m_isInBound == true)
	{
		Gdiplus::Point offset;
		offset.X = point.x - m_pointOfLeftDown.x;
		offset.Y = point.y - m_pointOfLeftDown.y;

		m_figure->Drag(offset.X, offset.Y);

		Invalidate(FALSE);

		m_pointOfLeftDown.x = point.x;
		m_pointOfLeftDown.y = point.y;
	}
	else
	{
		m_isInBound = m_figure->IsInBound(point.x, point.y);
		if (m_isInBound == true)
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
		else
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

void CgrimpanDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_figure == nullptr) return;

	m_pointOfLeftDown = point;

	m_isLButtonUp = false;

	if (m_isInBound == true)
	{
		CDialogEx::OnLButtonDown(nFlags, point);
		return;
	}

	m_figure->SetPoint(0, point.x, point.y);
	m_figure->SetPoint(1, point.x, point.y);

	m_isAddNewObject = true;

	Gdiplus::Graphics memG(m_canvasAfterDrawing.get());
	memG.DrawImage(m_canvasDuringDraw.get(), 0, 0, m_canvasDuringDraw->GetWidth(), m_canvasDuringDraw->GetHeight());

	Invalidate(FALSE);

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CgrimpanDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_figure == nullptr) return;

	if (m_isCropping == true && m_isAddNewObject == true)
	{
		std::shared_ptr<Figure::CropRectangle> crop = std::dynamic_pointer_cast<Figure::CropRectangle>(m_figure);
		crop->SetCropPoints();
	}

	m_isAddNewObject = false;
	m_isLButtonUp = true;

	Invalidate(FALSE);

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CgrimpanDlg::OnDrawImage()
{
	CPaintDC dc(GetDlgItem(IDC_STATIC_RECT));

	Gdiplus::Graphics mainG(dc.GetSafeHdc());

	Gdiplus::Graphics memG(m_canvasDuringDraw.get());
	memG.DrawImage(m_canvasAfterDrawing.get(), 0, 0, m_canvasAfterDrawing->GetWidth(), m_canvasAfterDrawing->GetHeight());

	if (m_isCropping == true)
	{
		std::shared_ptr<Figure::CropRectangle> crop = std::dynamic_pointer_cast<Figure::CropRectangle>(m_figure);
		if (m_isInBound == false && m_isLButtonUp == false)
			crop->Draw(&memG, true);
		else
			crop->Draw(&memG, m_canvasAfterDrawing.get());
	}
	else
	{
		if (m_figure != nullptr)
			m_figure->Draw(&memG);
	}

	mainG.DrawImage(m_canvasDuringDraw.get(), 0, 0);

	if (m_isLButtonUp == true)
	{
		Gdiplus::Rect size = m_figure->GetSize();
		Pen pen(Color::Blue, 1);
		pen.SetDashStyle(DashStyle::DashStyleDot);
		mainG.DrawRectangle(&pen, size);
	}
}

void CgrimpanDlg::OnFiguresLine()
{
	m_figure = std::make_shared<Figure::Line>();
	m_isCropping = false;
	checkMenuState(0);
}

void CgrimpanDlg::OnFiguresRectangle()
{
	m_figure = std::make_shared<Figure::Rectangle>();
	m_isCropping = false;
	checkMenuState(1);
}

void CgrimpanDlg::OnFiguresCropbox()
{
	m_figure = std::make_shared<Figure::CropRectangle>();
	m_isCropping = true;
	checkMenuState(2);
}

void CgrimpanDlg::checkMenuState(int index)
{
	CMenu* pMenu = GetMenu();
	switch (index)
	{
	case 0:
		pMenu->CheckMenuItem(ID_FIGURES_LINE, MF_CHECKED);
		pMenu->CheckMenuItem(ID_FIGURES_RECTANGLE, MF_UNCHECKED);
		pMenu->CheckMenuItem(ID_FIGURES_CROPBOX, MF_UNCHECKED);
		break;
	case 1:
		pMenu->CheckMenuItem(ID_FIGURES_LINE, MF_UNCHECKED);
		pMenu->CheckMenuItem(ID_FIGURES_RECTANGLE, MF_CHECKED);
		pMenu->CheckMenuItem(ID_FIGURES_CROPBOX, MF_UNCHECKED);
		break;
	case 2:
		pMenu->CheckMenuItem(ID_FIGURES_LINE, MF_UNCHECKED);
		pMenu->CheckMenuItem(ID_FIGURES_RECTANGLE, MF_UNCHECKED);
		pMenu->CheckMenuItem(ID_FIGURES_CROPBOX, MF_CHECKED);
		break;
	default:
		pMenu->CheckMenuItem(ID_FIGURES_LINE, MF_UNCHECKED);
		pMenu->CheckMenuItem(ID_FIGURES_RECTANGLE, MF_UNCHECKED);
		pMenu->CheckMenuItem(ID_FIGURES_CROPBOX, MF_UNCHECKED);
		break;
	}
}

void CgrimpanDlg::OnFileExit()
{
	::SendMessage(GetSafeHwnd(), WM_CLOSE, NULL, NULL);
}

void CgrimpanDlg::OnFileOpen()
{
	CRect rect;
	GetDlgItem(IDC_STATIC_RECT)->GetClientRect(rect);
	CPoint center_pos = rect.CenterPoint();

	CString filter = _T("image file(*.jpg;*.bmp;*.png;)|*.jpg;*.bmp;*.png;|All Files(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T(""), _T(""), OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDOK)
	{
		Image* img = Image::FromFile(dlg.GetPathName());

		float n = min(rect.Width() / img->GetWidth(), rect.Height() / img->GetHeight());

		UINT n_w = img->GetWidth() * n;
		UINT n_h = img->GetHeight() * n;

		m_canvasAfterDrawing = std::make_shared<Bitmap>(img->GetWidth(), img->GetHeight());
		m_canvasDuringDraw = std::make_shared<Bitmap>(img->GetWidth(), img->GetHeight());

		int x = center_pos.x - (img->GetWidth() / 2);
		int y = center_pos.y - (img->GetHeight() / 2);

		Graphics g(m_canvasAfterDrawing.get());
		g.DrawImage(img, x, y, img->GetWidth(), img->GetHeight());

		Invalidate(FALSE);
	}
}

/*void CgrimpanDlg::OnFileOpen()
{
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_READONLY, _T("image file(*.jpg;*.bmp;*.png;)|*.jpg;*.bmp;*.png;|All Files(*.*)|*.*||"));
	if (fileDlg.DoModal() == IDOK)
	{
		CString path = fileDlg.GetPathName();

		CT2CA pszString(path);
		std::string strPath(pszString);

		m_matImage = imread(strPath, IMREAD_UNCHANGED);

		CreateBitmapInfo(m_matImage.cols, m_matImage.rows, m_matImage.channels() * 8);

		DrawImage();
	}
}*/

void CgrimpanDlg::OnFileSave()
{
	CString filter = _T("Bitmap(*.BMP)|*.BMP|JPEG(*.JPG)|*.JPG|PNG Files(*.png)|*.png||");
	CFileDialog dlg(FALSE, _T(""), _T(""), OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDOK)
	{
		CLSID clsid;
		CString extension = dlg.GetFileExt();
		if (extension == _T("BMP"))
			CLSIDFromString(_T("{557cf400-1a04-11d3-9a73-0000f81ef32e}"), &clsid);
		else if (extension == _T("JPG"))
			CLSIDFromString(_T("{557cf401-1a04-11d3-9a73-0000f81ef32e}"), &clsid);
		else if (extension == _T("PNG"))
			CLSIDFromString(_T("{557cf406-1a04-11d3-9a73-0000f81ef32e}"), &clsid);

		m_canvasAfterDrawing.get()->Save(dlg.GetPathName(), &clsid, NULL);
	}
}

void CgrimpanDlg::initializeCanvas()
{
	CRect rect;
	this->GetClientRect(&rect);

	m_canvasAfterDrawing = std::make_shared<Bitmap>(rect.Width(), rect.Height());
	m_canvasDuringDraw = std::make_shared<Bitmap>(rect.Width(), rect.Height());

	Gdiplus::Graphics graphicsOfcanvasAfterDrawing(m_canvasAfterDrawing.get());
	Gdiplus::Graphics graphicsOfcanvasDuringDraw(m_canvasAfterDrawing.get());

	SolidBrush whiteBrush(Color(255, 255, 255, 255));
	graphicsOfcanvasAfterDrawing.FillRectangle(&whiteBrush, 0, 0, rect.Width(), rect.Height());
	graphicsOfcanvasDuringDraw.FillRectangle(&whiteBrush, 0, 0, rect.Width(), rect.Height());
}

void CgrimpanDlg::OnFileNew()
{
	initializeCanvas();

	Invalidate(FALSE);
}

void CgrimpanDlg::CreateBitmapInfo(int w, int h, int bpp)
{
	if (m_pBitmapInfo != NULL)
	{
		delete m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}

	if (bpp == 8)
		m_pBitmapInfo = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)];
	else // 24 or 32bit
		m_pBitmapInfo = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFO)];

	m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo->bmiHeader.biPlanes = 1;
	m_pBitmapInfo->bmiHeader.biBitCount = bpp;
	m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo->bmiHeader.biSizeImage = 0;
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;

	if (bpp == 8)
	{
		for (int i = 0; i < 256; i++)
		{
			m_pBitmapInfo->bmiColors[i].rgbBlue = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbGreen = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbRed = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbReserved = 0;
		}
	}

	m_pBitmapInfo->bmiHeader.biWidth = w;
	m_pBitmapInfo->bmiHeader.biHeight = -h;
}

/*void CgrimpanDlg::DrawImage()
{
	CClientDC dc(GetDlgItem(IDC_PC_VIEW));

	CRect rect;
	GetDlgItem(IDC_PC_VIEW)->GetClientRect(&rect);

	SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);
	StretchDIBits(dc.GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0, m_matImage.cols, m_matImage.rows, m_matImage.data, m_pBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}*/

void CgrimpanDlg::AllocForm()
{
	CCreateContext context;
	ZeroMemory(&context, sizeof(context));

	CRect rectOfPanelArea;

	GetDlgItem(IDC_STATIC_RECT)->GetWindowRect(&rectOfPanelArea);
	ScreenToClient(&rectOfPanelArea);
	m_pForm = new CMyForm();
	m_pForm->Create(NULL, NULL, WS_CHILD | WS_VSCROLL | WS_HSCROLL, rectOfPanelArea, this, IDD_FORM_MYFORM, &context);
	m_pForm->OnInitialUpdate();
	m_pForm->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_RECT)->DestroyWindow();
}

BOOL CgrimpanDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	Matrix m_mat(1.1f, 0.0f, 0.0f, 1.1f, 0.0f, 0.0f);
	m_mat.Reset();
	if (zDelta > 0)
	{
		// m_figure.SetTransform(&m_mat);
	}

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}
