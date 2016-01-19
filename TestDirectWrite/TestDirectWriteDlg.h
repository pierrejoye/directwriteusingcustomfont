
// TestDirectWriteDlg.h : header file
//

#pragma once


// CTestDirectWriteDlg dialog
class CTestDirectWriteDlg : public CDialogEx
{
// Construction
public:
	CTestDirectWriteDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTDIRECTWRITE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
private:
	HRESULT CreateDevInDependentResources();
	void ReleaseDevInDependentResources();
	HRESULT CreateDevDependentResources();
	void ReleaseDevDependentResources();
	FLOAT ConvertPointSizeToDIP(FLOAT points);
	CString GetWeekDay();


// Implementation
protected:
	HICON m_hIcon;
	ID2D1FactoryPtr m_pD2DFactory;
	ID2D1HwndRenderTargetPtr m_pRT;
	IDWriteFactoryPtr m_pDWriteFactory;
	ID2D1SimplifiedGeometrySinkPtr m_pGeometrySink;
	ID2D1PathGeometryPtr m_pPathGeometry;
	ID2D1SolidColorBrushPtr m_pSolidBrushFill;
	ID2D1SolidColorBrushPtr m_pSolidBrushOutline;

	CString m_szOutline;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
};
