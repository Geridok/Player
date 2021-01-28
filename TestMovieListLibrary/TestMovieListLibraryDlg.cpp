
// TestMovieListLibraryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestMovieListLibrary.h"
#include "TestMovieListLibraryDlg.h"
#include "WorkWithFrames.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestMovieListLibraryDlg dialog
CTestMovieListLibraryDlg::CTestMovieListLibraryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestMovieListLibraryDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_signaturesHandler_1 = new WorkWithSignatures();
	WorkWithSignatures *second = new WorkWithSignatures();
	m_Player_1 = new CPlayer(m_signaturesHandler_1);
	converter = ConvertBSRTtoString();
}

void CTestMovieListLibraryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE, m_editFileName_1);
	DDX_Control(pDX, IDC_FILE2, m_editFileName_2);
	DDX_Control(pDX, IDC_START, m_Start);
	DDX_Control(pDX, IDC_OUT_FILE_NAME, m_OtpuFileName);
}

BEGIN_MESSAGE_MAP(CTestMovieListLibraryDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSE, &CTestMovieListLibraryDlg::OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_START, &CTestMovieListLibraryDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_BROWSE2, &CTestMovieListLibraryDlg::OnBnClickedBrowse2)
END_MESSAGE_MAP()


// CTestMovieListLibraryDlg message handlers
BOOL CTestMovieListLibraryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//if( InitAll() != S_OK )  EndDialog(IDCANCEL);

	m_Start.EnableWindow(TRUE);
	//m_File.EnableWindow(FALSE);
	//m_File_2.EnableWindow(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestMovieListLibraryDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestMovieListLibraryDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestMovieListLibraryDlg::OnBnClickedBrowse()
{
	CFileDialog dlg(TRUE, _T(""), _T(""), OFN_FILEMUSTEXIST, _T("All Files (*.*)|*.*||"), this);
	if( IDOK==dlg.DoModal() ) {
		m_editFileName_1.SetWindowText(dlg.GetPathName());
	}
}


void CTestMovieListLibraryDlg::OnBnClickedBrowse2()
{
	CFileDialog dlg(TRUE, _T(""), _T(""), OFN_FILEMUSTEXIST, _T("All Files (*.*)|*.*||"), this);
	if (IDOK == dlg.DoModal()) {
		m_editFileName_2.SetWindowText(dlg.GetPathName());
	}
}

void CTestMovieListLibraryDlg::OnBnClickedStart()
{

	CString fileName1;
	CString fileName2;

	m_editFileName_1.GetWindowText(fileName1);
	m_bstrFileName_1 = fileName1;

	m_editFileName_2.GetWindowText(fileName2);
	m_bstrFileName_2 = fileName2;

	if( CheckMovie(m_bstrFileName_1) == S_OK) {
		InitPlayer(m_Player_1,m_bstrFileName_1);
		GetOutputFileName();
		m_signaturesHandler_1->compareVideo(converter.ConvertBSTRToString(m_outFileName));
		
	}
	else {
		return;
	}
	if( CheckMovie(m_bstrFileName_2) == S_OK ) {

	}
	else {
		return;
	}
}


void CTestMovieListLibraryDlg::GetOutputFileName()
{
	CString str;
	m_OtpuFileName.GetWindowText(str);
	m_outFileName = str;
}


HRESULT CTestMovieListLibraryDlg::InitPlayer(CPlayer* player, CComBSTR path)
{
		HRESULT hres = player->Init(path);
		ATLASSERT(hres == S_OK);
		if( hres != S_OK )  return hres;
	return S_OK;
}

HRESULT CTestMovieListLibraryDlg::CheckMovie(CComBSTR fileName)
{
	CComPtr<MovieListLibraryLib::ISLTMMovieCreator> pICreator;
	HRESULT hres = pICreator.CoCreateInstance(MovieListLibraryLib::CLSID_SLTMMovieCreator);
	ATLASSERT(hres==S_OK);
	if( hres != S_OK )  return hres;
	hres = pICreator->Init(25.0, 10, 0, 0, 0);
	CComPtr<MovieListLibraryLib::ISLTMMovie> pIUnk;
	hres = pICreator->CreateMovie(MovieListLibraryLib::eMovieType_Film, fileName, 10,0,0, &pIUnk); 
	ATLASSERT(hres==S_OK);
	if( hres != S_OK )  return hres;
	return S_OK;
}

