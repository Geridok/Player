
// TestMovieListLibraryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestMovieListLibraryDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::vector<std::chrono::microseconds> calc_1 = std::vector<std::chrono::microseconds>{};
std::vector<std::chrono::microseconds> calc_2 = std::vector<std::chrono::microseconds>{};
std::vector<std::chrono::microseconds> compare = std::vector<std::chrono::microseconds>{};

bool firstVideo = true;


// CTestMovieListLibraryDlg dialog
CTestMovieListLibraryDlg::CTestMovieListLibraryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestMovieListLibraryDlg::IDD, pParent)
{	
	m_dataStorage = DataStorage::getInstance();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_signaturesHandler = std::make_shared<SignatureHandler>();

	m_Player = new CPlayer(m_signaturesHandler);

	converter = ConvertBSRTtoString();
	comparator = SignatureComparator();
}

void CTestMovieListLibraryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FILE_TO_ADD, m_FileNameToAddCEdit);
	DDX_Control(pDX, IDC_EDIT_AVG_CALC, m_AvgCalcTimeCEdit);
	DDX_Control(pDX, IDC_EDIT_FILE_TO_COMPARE, m_fileNameToCompareCEdit);
	DDX_Control(pDX, IDC_STATUS_INFORMATION, m_ProgramStatusCEdit);

	DDX_Control(pDX, IDC_ADD_TO_DATABASE, m_addButton);
	DDX_Control(pDX, IDC_BROWSE_FILE_TO_COMPARE, m_fileToSearchButton);
	DDX_Control(pDX, IDC_BROWSE_DATA_BASE, m_FileNameToAddButton);
	DDX_Control(pDX, IDC_SEARCH_LINEAR, m_searchLinearButton);
	DDX_Control(pDX, IDC_SEARCH_VP, m_searchVPButton);
}

BEGIN_MESSAGE_MAP(CTestMovieListLibraryDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSE_DATA_BASE, &CTestMovieListLibraryDlg::OnBnClickedAddToDataBase)
	ON_BN_CLICKED(IDC_ADD_TO_DATABASE, &CTestMovieListLibraryDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_BROWSE_FILE_TO_COMPARE, &CTestMovieListLibraryDlg::OnBnClickedBrowseFileToCompare)
	ON_BN_CLICKED(IDC_SEARCH_LINEAR, &CTestMovieListLibraryDlg::OnBnClickedSearchLinear)
	ON_BN_CLICKED(IDC_SEARCH_VP, &CTestMovieListLibraryDlg::OnBnClickedSearchVp)
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

	m_addButton.EnableWindow(FALSE);

	m_fileToSearchButton.EnableWindow(FALSE);
	m_searchLinearButton.EnableWindow(FALSE);
	m_searchVPButton.EnableWindow(FALSE);

	if (m_dataStorage->loadDataFromFile()) {
		m_ProgramStatusCEdit.SetWindowTextW(CString("Data base loaded sucsessful"));
		m_fileToSearchButton.EnableWindow(TRUE);
	}
	else {
		m_ProgramStatusCEdit.SetWindowTextW(CString("Loading failed. Data base empty or some error occur"));
	}
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


void CTestMovieListLibraryDlg::OnBnClickedAddToDataBase()
{
	CFileDialog dlg(TRUE, _T(""), _T(""), OFN_FILEMUSTEXIST, _T("All Files (*.*)|*.*||"), this);
	if( IDOK==dlg.DoModal() ) {
		m_FileNameToAddCEdit.SetWindowText(dlg.GetPathName());
	}
	CString fileName1;
	m_FileNameToAddCEdit.GetWindowText(fileName1);
	m_bstrFileName_1 = fileName1;
	if (CheckMovie(m_bstrFileName_1) != S_OK) {
		m_FileNameToAddCEdit.SetWindowTextW(CString("Can't read file, plz choose different file"));
		m_addButton.EnableWindow(FALSE);
		return;
	}
	m_signaturesHandler = std::make_shared<SignatureHandler>();
	m_Player = new CPlayer(m_signaturesHandler);
	m_addButton.EnableWindow(TRUE);
	m_addButton.SetFocus();
}

void CTestMovieListLibraryDlg::OnBnClickedBrowseFileToCompare()
{
	m_addButton.EnableWindow(FALSE);
	CFileDialog dlg(TRUE, _T(""), _T(""), OFN_FILEMUSTEXIST, _T("All Files (*.*)|*.*||"), this);
	if (IDOK == dlg.DoModal()) {
		m_fileNameToCompareCEdit.SetWindowText(dlg.GetPathName());
	}
	CString fileName1;
	m_fileNameToCompareCEdit.GetWindowText(fileName1);
	m_bstrFileName_1 = fileName1;
	if (CheckMovie(m_bstrFileName_1) != S_OK) {
		m_fileNameToCompareCEdit.SetWindowTextW(CString("Can't read file, plz choose different file"));
		m_searchLinearButton.EnableWindow(FALSE);
		m_searchVPButton.EnableWindow(FALSE);
		return;
	}
	m_signaturesHandler = std::make_shared<SignatureHandler>();
	m_Player = new CPlayer(m_signaturesHandler);
	m_searchLinearButton.EnableWindow(TRUE);
	m_searchVPButton.EnableWindow(TRUE);
}

void CTestMovieListLibraryDlg::OnBnClickedSearchLinear()
{
	// TODO: Add your control notification handler code here
}


void CTestMovieListLibraryDlg::OnBnClickedSearchVp()
{
	// TODO: Add your control notification handler code here
}

void CTestMovieListLibraryDlg::OnBnClickedAdd()
{
		m_ProgramStatusCEdit.SetWindowTextW(CString("The video is being processed"));
		CString fileName;
		m_FileNameToAddCEdit.GetWindowText(fileName);
		CDialog::OnPaint();
		CComBSTR str = fileName;
		if (InitPlayer(m_Player, m_bstrFileName_1) != S_OK) {
			LPCTSTR lp = L"Error when processed video";
			m_ProgramStatusCEdit.Clear();
			m_ProgramStatusCEdit.ReplaceSel(lp);
			return;
		}
		m_ProgramStatusCEdit.SetWindowTextW(CString("Split video to parts"));
		m_signaturesHandler->splitVideoToVideoPArt();
		m_ProgramStatusCEdit.SetWindowTextW(CString("Add new video to data storage and write to disk"));
		if (!m_dataStorage->addNewVideoToDataBase(m_signaturesHandler)) {
			m_ProgramStatusCEdit.SetWindowTextW(CString("Error when write data to disk"));
			return;
		}
		m_ProgramStatusCEdit.SetWindowTextW(CString("New video added sucsessful"));
		m_addButton.EnableWindow(FALSE);
		m_FileNameToAddCEdit.SetWindowTextW(CString(""));
		m_fileNameToCompareCEdit.SetWindowTextW(CString(""));
	/*if (!stop) {
		CString fileName1;
		CString fileName2;

		m_fileNameToCompareCEdit.GetWindowText(fileName1);
		m_bstrFileName_1 = fileName1;

		m_FileNameToAddCEdit.GetWindowText(fileName2);
		m_bstrFileName_2 = fileName2;

		InitPlayer(m_Player, m_bstrFileName_1);
		firstVideo = false;

		//GetOutputFileName();
		std::pair<std::pair<uint64_t, uint64_t>, std::pair<uint64_t, uint64_t>> ranges;
		for (int index = 0; index < m_signaturesHandler->getSigAmount(); index++) {
			//ranges = comparator.comparePair(m_signaturesHandler, index, m_signaturesHandler_2, 0, 0.5);
			if (ranges.second.second != 0) { break; }
		}

		auto fileNameOutput = converter.ConvertBSTRToString(m_bsrtOutFileName);
		std::string diffOut = fileNameOutput + "Diff.txt";
		std::string calcOut_1 = fileNameOutput + "Calc1.txt";
		std::string calcOut_2 = fileNameOutput + "Calc2.txt";
		std::string compOut = fileNameOutput + "Comp.txt";

		size_t i = 1;
		{
			i = 0;
			std::ofstream diffFout(diffOut);
			for (auto it : diffVec) {
				diffFout << std::to_string(i) + "\t" + std::to_string(it) << std::endl;
				i++;
			}
		}
		{
			i = 0;
			std::ofstream calcFout_1(calcOut_1);
			for (auto it : calc_1) {
				calcFout_1 << std::to_string(i) << "\t" << it.count() << std::endl;
				i++;
			}
		}
		{
			i = 0;
			std::ofstream calcFout_2(calcOut_2);
			for (auto it : calc_2) {
				calcFout_2 << std::to_string(i) << "\t" << it.count() << std::endl;
				i++;
			}
		}
		{
			i = 0;
			std::ofstream compFout(compOut);
			for (auto it : compare) {
				compFout << std::to_string(i) << "\t" << it.count() << std::endl;
				i++;
			}
		}
		
		//auto avg = std::accumulate(calc_1.begin(), calc_1.end(), decltype(calc_1)::value_type(0));

		CString avg_1;
		avg_1.Format(_T("%d"), std::accumulate(calc_1.begin(), calc_1.end(), decltype(calc_1)::value_type(0)) / calc_1.size());
		m_AvgCalcTimeCEdit.SetWindowText(avg_1);

		CString avgcomp;
		avgcomp.Format(_T("%d"), std::accumulate(compare.begin(), compare.end(), decltype(compare)::value_type(0)) / compare.size());
		m_AvgCompTimeCEdit.SetWindowText(avgcomp);

		

		
	}*/
	ReloadPlayer();
}


HRESULT CTestMovieListLibraryDlg::InitPlayer(CPlayer* player, CComBSTR path)
{
		HRESULT hres = player->Init(path);
		ATLASSERT(hres == S_OK);
		if( hres != S_OK )  return hres;
	return S_OK;
}

void CTestMovieListLibraryDlg::ReloadPlayer()
{
	delete m_Player;

	m_signaturesHandler = std::make_shared<SignatureHandler>();

	m_Player = new CPlayer(m_signaturesHandler);
}

HRESULT CTestMovieListLibraryDlg::CheckMovie(CComBSTR fileName)
{
	CComPtr<MovieListLibraryLib::ISLTMMovieCreator> pICreator;
	HRESULT hres = pICreator.CoCreateInstance(MovieListLibraryLib::CLSID_SLTMMovieCreator);
	//ATLASSERT(hres==S_OK);
	if( hres != S_OK )  return hres;
	hres = pICreator->Init(25.0, 10, 0, 0, 0);
	CComPtr<MovieListLibraryLib::ISLTMMovie> pIUnk;
	hres = pICreator->CreateMovie(MovieListLibraryLib::eMovieType_Film, fileName, 10,0,0, &pIUnk); 
	//ATLASSERT(hres==S_OK);
	if( hres != S_OK )  return hres;
	return S_OK;
}






