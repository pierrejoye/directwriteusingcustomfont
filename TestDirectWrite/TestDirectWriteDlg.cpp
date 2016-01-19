
// TestDirectWriteDlg.cpp : implementation file
//

#include "stdafx.h"
#include <windows.h>
#include "resource.h"
#include <Commdlg.h>
#include "TestDirectWrite.h"
#include "TestDirectWriteDlg.h"
#include "afxdialogex.h"

#include <cstdio>
#include <ctime>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

OPENFILENAME ofn ;
// a another memory buffer to contain the file name
wchar_t szFile[OFS_MAXPATHNAME] ;


CTestDirectWriteDlg::CTestDirectWriteDlg(CWnd* pParent /*=NULL*/)
	: 
	m_pD2DFactory(NULL),
	m_pRT(NULL),
	m_pDWriteFactory(NULL),
	m_pGeometrySink(NULL),
	m_pPathGeometry(NULL),
	m_pSolidBrushFill(NULL),
	m_pSolidBrushOutline(NULL),
	//m_szOutline(L"Great "),
	CDialogEx(CTestDirectWriteDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestDirectWriteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestDirectWriteDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTestDirectWriteDlg message handlers

BOOL CTestDirectWriteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	HRESULT hr = CreateDevInDependentResources();
	if (!SUCCEEDED(hr))
	{
		MessageBox(L"Something went wrong in creating device independent resources!", L"DirectWrite Demo", MB_ICONERROR);
		return FALSE;
	}

	return TRUE;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestDirectWriteDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if(FAILED(CreateDevDependentResources()))
			return;

		if (m_pRT->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED)
			return;

		m_pRT->BeginDraw();
		m_pRT->SetTransform(D2D1::IdentityMatrix());
		m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));
		m_pRT->SetTransform(D2D1::Matrix3x2F::Translation(10.0f,60.0f));
		m_pRT->DrawGeometry(m_pPathGeometry, m_pSolidBrushOutline, 3.0f);
		m_pRT->FillGeometry(m_pPathGeometry, m_pSolidBrushFill);

		HRESULT hr = m_pRT->EndDraw();

		if(FAILED(hr))
			ReleaseDevDependentResources();
	}
}

HCURSOR CTestDirectWriteDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestDirectWriteDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// Cleanup resources.
	ReleaseDevDependentResources();
	ReleaseDevInDependentResources();
}

HRESULT CTestDirectWriteDlg::CreateDevInDependentResources()
{
	HRESULT hr = S_OK;
	IFR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory));

	IFR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_pDWriteFactory)));

	IDWriteFontFacePtr pFontFace = NULL;
	IDWriteFontFilePtr pFontFiles = NULL;

	if (SUCCEEDED(hr))
	{
		CString strPath;

		 // open a file name
		ZeroMemory( &ofn , sizeof( ofn));
		ofn.lStructSize = sizeof ( ofn );
		ofn.hwndOwner = NULL  ;
		ofn.lpstrFile = szFile ;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof( szFile );
		ofn.lpstrFilter = L"All\0*.*\0Text\0*.TTF\0";
		ofn.nFilterIndex =1;
		ofn.lpstrFileTitle = NULL ;
		ofn.nMaxFileTitle = 0 ;
		ofn.lpstrInitialDir=NULL ;
		ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ;
		GetOpenFileName( &ofn );
	
		// Now simpley display the file name 
		MessageBox (ofn.lpstrFile , L"File Name" , MB_OK);

		strPath.ReleaseBuffer ();
		strPath += szFile;

		hr = m_pDWriteFactory->CreateFontFileReference(
			strPath,
			NULL,
			&pFontFiles);
	}

	IDWriteFontFile* fontFileArray[] = {pFontFiles};

	if(pFontFiles==NULL)
	{
		MessageBox(L"No font file is found at executable folder", L"Error");
		return E_FAIL;
	}

	IFR(m_pDWriteFactory->CreateFontFace(
		DWRITE_FONT_FACE_TYPE_TRUETYPE,
		1, // file count
		fontFileArray,
		0,
		DWRITE_FONT_SIMULATIONS_NONE,
		&pFontFace
		));

	//CString szOutline = m_szOutline;
	CString szOutline(L"Hello!");

	UINT* pCodePoints = new UINT[szOutline.GetLength()];
	UINT16* pGlyphIndices = new UINT16[szOutline.GetLength()];
	ZeroMemory(pCodePoints, sizeof(UINT) * szOutline.GetLength());
	ZeroMemory(pGlyphIndices, sizeof(UINT16) * szOutline.GetLength());
	for(int i=0; i<szOutline.GetLength(); ++i)
	{
		pCodePoints[i] = szOutline.GetAt(i);
	}
	pFontFace->GetGlyphIndicesW(pCodePoints, szOutline.GetLength(), pGlyphIndices);

	//Create the path geometry
	IFR(m_pD2DFactory->CreatePathGeometry(&m_pPathGeometry));

	IFR(m_pPathGeometry->Open((ID2D1GeometrySink**)&m_pGeometrySink));

	IFR(pFontFace->GetGlyphRunOutline(
		ConvertPointSizeToDIP(48.0f), 
		pGlyphIndices, 
		NULL,
		NULL,
		szOutline.GetLength(),
		FALSE,
		FALSE,
		m_pGeometrySink));

	IFR(m_pGeometrySink->Close());

	if(pCodePoints)
	{
		delete [] pCodePoints;
		pCodePoints = NULL;
	}

	if(pGlyphIndices)
	{
		delete [] pGlyphIndices;
		pGlyphIndices = NULL;
	}

	return hr;
}

void CTestDirectWriteDlg::ReleaseDevInDependentResources()
{
	if (m_pGeometrySink)
		m_pGeometrySink.Release();
	if (m_pPathGeometry)
		m_pPathGeometry.Release();
	if (m_pDWriteFactory)
		m_pDWriteFactory.Release();
	if (m_pD2DFactory)
		m_pD2DFactory.Release();
}

HRESULT CTestDirectWriteDlg::CreateDevDependentResources()
{
	HRESULT hr = S_OK;

	if (m_pRT)
		return hr;

	if (!IsWindowVisible())
		return E_FAIL;

	RECT rc;
	GetClientRect(&rc);
	D2D1_SIZE_U size = D2D1::SizeU((rc.right-rc.left), (rc.bottom-rc.top));

	IFR(m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(GetSafeHwnd(), size), &m_pRT));
	hr = m_pRT->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Black),
        &m_pSolidBrushFill
        );
	IFR(m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue),
		&m_pSolidBrushOutline
		));

	return hr;
}

void CTestDirectWriteDlg::ReleaseDevDependentResources()
{
	if (m_pSolidBrushOutline)
		m_pSolidBrushOutline.Release();
	if(m_pSolidBrushFill)
		m_pSolidBrushFill.Release();
	if (m_pRT)
		m_pRT.Release();
}

FLOAT CTestDirectWriteDlg::ConvertPointSizeToDIP(FLOAT points)
{
	return (points/72.0f)*96.0f;
}

CString CTestDirectWriteDlg::GetWeekDay()
{
	wchar_t * weekday[] = { L"This", L"Is",
		L"some", L"Direct2d",
		L"Example"};


	COleDateTime t;
	t = COleDateTime::GetCurrentTime();
	return weekday[t.GetDayOfWeek()-1];
}