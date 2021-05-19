
// TestMovieListLibraryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestMovieListLibraryDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

template <typename TP>
std::time_t to_time_t(TP tp)
{
	using namespace std::chrono;
	auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
		+ system_clock::now());
	return system_clock::to_time_t(sctp);
}


// CTestMovieListLibraryDlg dialog
CTestMovieListLibraryDlg::CTestMovieListLibraryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestMovieListLibraryDlg::IDD, pParent)
{	
	m_dataStorage = DataStorage::getInstance();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_signaturesHandler = std::make_shared<SignatureHandler>();

	m_Player = new CPlayer(m_signaturesHandler);

	converter = ConvertBSRTtoString();
}

void CTestMovieListLibraryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATUS_INFORMATION, m_ProgramStatusCEdit);

	DDX_Control(pDX, IDC_DATA_BASE_EDIT, m_pathToDataBaseCEdit);

	DDX_Control(pDX, IDC_EDIT_FILE_TO_COMPARE, m_fileNameToCompareCEdit);
	DDX_Control(pDX, IDC_BROWSE_FILE_TO_COMPARE, m_browseFileToSearchButton);
	DDX_Control(pDX, IDC_START_SEARCH_VIDEO, m_startSearchButton);

	DDX_Control(pDX, IDC_DATABASE_NAME_EDIT, m_pathToNewDataBaseCEdit);
	DDX_Control(pDX, IDC_START_NEW_DATA_BASE, m_startCreationNewDataBase);


	DDX_Control(pDX, IDC_START_CORRELATION, m_startCorrelationButton);
}

BEGIN_MESSAGE_MAP(CTestMovieListLibraryDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOAD_DATA_BASE, &CTestMovieListLibraryDlg::OnBnClickedLoadDataBase)
	ON_BN_CLICKED(IDC_BROWSE_FILE_TO_COMPARE, &CTestMovieListLibraryDlg::OnBnClickedBrowseFileToCompare)
	ON_BN_CLICKED(IDC_CREATE_NEW_DATABASE_BUTTON, &CTestMovieListLibraryDlg::OnBnClickedBrowseNewDatabaseButton)
	ON_BN_CLICKED(IDC_START_SEARCH_VIDEO, &CTestMovieListLibraryDlg::OnBnClickedStartSearchVideo)
	ON_BN_CLICKED(IDC_START_NEW_DATA_BASE, &CTestMovieListLibraryDlg::OnBnClickedStartNewDataBase)
	ON_BN_CLICKED(IDC_START_CORRELATION, &CTestMovieListLibraryDlg::OnBnClickedStartCorrelation)
END_MESSAGE_MAP()


// CTestMovieListLibraryDlg message handlers
BOOL CTestMovieListLibraryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (!workWithIniFile::readIniFile()) {
		CDialog::MessageBox(CString("Can't read .ini file"));
		CDialog::EndDialog(IDCANCEL);
		return FALSE;
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//if( InitAll() != S_OK )  EndDialog(IDCANCEL);

	m_browseFileToSearchButton.EnableWindow(FALSE);
	m_startSearchButton.EnableWindow(FALSE);
	m_startCreationNewDataBase.EnableWindow(FALSE);
	m_startCorrelationButton.EnableWindow(FALSE);

	m_ProgramStatusCEdit.SetWindowText(CString("Choose Action"));
	m_pathToNewDataBaseCEdit.SetWindowText(CString("Please enter base Name"));
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


void CTestMovieListLibraryDlg::OnBnClickedLoadDataBase()
{
	CFileDialog dlg(TRUE, _T(""), _T(""), OFN_FILEMUSTEXIST, _T("All Files (*.*)|*.*||"), this);
	if (IDOK == dlg.DoModal()) {
		m_pathToDataBaseCEdit.SetWindowText(dlg.GetPathName());
	}
	CString fileName;
	m_pathToDataBaseCEdit.GetWindowText(fileName);
	m_ProgramStatusCEdit.SetWindowText(CString("Loading base..."));

	//TODO: сделать прерывания на обновление UI

	std::string strFileName = CW2A(fileName.GetString());

	if (m_dataStorage->loadDataFromFile(strFileName)) {
		m_ProgramStatusCEdit.SetWindowText(CString("Base loaded sucsesful"));
		m_startCorrelationButton.EnableWindow(TRUE);
	}
	else {
		m_ProgramStatusCEdit.SetWindowText(CString("Base loaded unsucsesful"));
	}

}

void CTestMovieListLibraryDlg::OnBnClickedBrowseFileToCompare()
{
	//m_addButton.EnableWindow(FALSE);
	//CFileDialog dlg(TRUE, _T(""), _T(""), OFN_FILEMUSTEXIST, _T("All Files (*.*)|*.*||"), this);
	//if (IDOK == dlg.DoModal()) {
	//	m_fileNameToCompareCEdit.SetWindowText(dlg.GetPathName());
	//}
	//CString fileName1;
	//m_fileNameToCompareCEdit.GetWindowText(fileName1);
	//m_bstrFileName_1 = fileName1;
	//if (CheckMovie(m_bstrFileName_1) != S_OK) {
	//	m_fileNameToCompareCEdit.SetWindowTextW(CString("Can't read file, plz choose different file"));
	//	m_searchVPButton.EnableWindow(FALSE);
	//	return;
	//}
	//m_signaturesHandler = std::make_shared<SignatureHandler>();
	//m_Player = new CPlayer(m_signaturesHandler);
	//m_searchVPButton.EnableWindow(TRUE);
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


void CTestMovieListLibraryDlg::OnBnClickedBrowseNewDatabaseButton()
{
	CFolderPickerDialog m_dlg;
	CString m_Folder;

	m_dlg.m_ofn.lpstrTitle = _T("Pick Folder with Data");
	m_dlg.m_ofn.lpstrInitialDir = _T("D:\\Work\\DiplomaWork");
	if (m_dlg.DoModal() == IDOK) {
		m_Folder = m_dlg.GetPathName();
		m_startCreationNewDataBase.EnableWindow(TRUE);
		std::string pathToFolder = CW2A(m_Folder);
		std::string status = "New data base from: \"" + pathToFolder + "\"";
		m_ProgramStatusCEdit.SetWindowText(CString(status.c_str()));
		m_pathToFolderNewDataBase = m_Folder;
	}

}


void CTestMovieListLibraryDlg::OnBnClickedStartSearchVideo()
{

}


void CTestMovieListLibraryDlg::OnBnClickedStartNewDataBase()
{
	CString baseName;
	m_pathToNewDataBaseCEdit.GetWindowText(baseName);

	auto pair = ReadWriteData::createDataBaseFromFolder(std::string(CW2A(m_pathToFolderNewDataBase)));
	
	for (auto signatureHandler : pair.first) {
		signatureHandler->splitVideoToVideoPart();
	}
	
	//ATLASSERT((pair.first.size() == pair.second.size()));
	auto signatureHandlerIterVec = pair.first.begin();
	auto videoInfoIterVec = pair.second.begin();

	if (pair.first.size() == 0 || pair.second.size() == 0) {
		m_ProgramStatusCEdit.SetWindowText(CString("Can't create base, some error occur"));
	}

	for (signatureHandlerIterVec; signatureHandlerIterVec != pair.first.end(); signatureHandlerIterVec++) {
		m_dataStorage->addSignatureHandler(*signatureHandlerIterVec, *videoInfoIterVec);
		videoInfoIterVec++;
	}
	std::string pathtoFile = CW2A(baseName);
	//m_dataStorage->writeToDisk(pathtoFile);

	std::string outString = "Create base with name: \"" + pathtoFile + "\"";
	m_ProgramStatusCEdit.SetWindowText(CString(outString.c_str()));
	m_startCorrelationButton.EnableWindow(TRUE);
}


void CTestMovieListLibraryDlg::OnBnClickedStartCorrelation()
{
	std::ofstream outFile("outFile.txt");
	auto signatureHandlerVec = m_dataStorage->getDataBase();

	size_t signatureHandlerIndex = 0;
	for (signatureHandlerIndex; signatureHandlerIndex < signatureHandlerVec.size() - 1; signatureHandlerIndex++) {
		auto signaturesMain = signatureHandlerVec[signatureHandlerIndex]->getSignatures();
		size_t currentSignaturehandlerIndex = signatureHandlerIndex + 1;
		for (size_t signaturesMainIndex = 0; signaturesMainIndex <= signaturesMain.size() - 1; signaturesMainIndex++) {
			while (currentSignaturehandlerIndex != signatureHandlerVec.size() - 1) {
				auto videoPartsVec = signatureHandlerVec[currentSignaturehandlerIndex]->getVideoParts();
				auto signaturesOther = signatureHandlerVec[currentSignaturehandlerIndex]->getSignatures();
				for (size_t videoPartIndex = 0; videoPartIndex <= videoPartsVec.size() - 1; videoPartIndex++) {

					if (signaturesMain[signaturesMainIndex]->difference(*signaturesOther[videoPartsVec[videoPartIndex]->mainSignatureIndex]) <= 0.3) {
						outFile << "SignatureHandleIndex_1:\t " << signatureHandlerIndex << " Frame: " << signaturesMainIndex << " && SignatureHandleIndex_2: " << currentSignaturehandlerIndex << " Frame: " << videoPartsVec[videoPartIndex]->mainSignatureIndex << " With diff: " << signaturesMain[signaturesMainIndex]->difference(*signaturesOther[videoPartsVec[videoPartIndex]->mainSignatureIndex]) << std::endl;
					}
				}
				currentSignaturehandlerIndex++;
			}
			currentSignaturehandlerIndex = signatureHandlerIndex + 1;
		}
	}
	m_ProgramStatusCEdit.SetWindowText(CString("Correletion Completed"));
}
