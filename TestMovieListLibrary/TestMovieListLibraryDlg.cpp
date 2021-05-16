
// TestMovieListLibraryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestMovieListLibraryDlg.h"



//namespace fs = std::filesystem;

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

	DDX_Control(pDX, IDC_EDIT_FILE_TO_COMPARE, m_fileNameToCompareCEdit);
	DDX_Control(pDX, IDC_BROWSE_FILE_TO_COMPARE, m_fileToSearchButton);
	DDX_Control(pDX, IDC_START_SEARCH_VIDEO, m_startSearchButton);

	DDX_Control(pDX, IDC_DATA_BASE_EDIT, m_pathToDataBaseCEdit);

	DDX_Control(pDX, IDC_DATABASE_NAME_EDIT, m_pathToNewDataBaseCEdit);
	DDX_Control(pDX, IDC_START_NEW_DATA_BASE, m_startCreationNewDataBase);


}

BEGIN_MESSAGE_MAP(CTestMovieListLibraryDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOAD_DATA_BASE, &CTestMovieListLibraryDlg::OnBnClickedLoadDataBase)
	ON_BN_CLICKED(IDC_BROWSE_FILE_TO_COMPARE, &CTestMovieListLibraryDlg::OnBnClickedBrowseFileToCompare)
	ON_BN_CLICKED(IDC_CREATE_NEW_DATABASE_BUTTON, &CTestMovieListLibraryDlg::OnBnClickedBrowseNewDatabaseButton)
	ON_BN_CLICKED(IDC_START_SEARCH_VIDEO, &CTestMovieListLibraryDlg::OnBnClickedStartSearchVideo)
	ON_BN_CLICKED(IDC_START_NEW_DATA_BASE, &CTestMovieListLibraryDlg::OnBnClickedStartNewDataBase)
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

	m_addButton.EnableWindow(FALSE);

	m_fileToSearchButton.EnableWindow(FALSE);
	m_searchVPButton.EnableWindow(TRUE);

	if (m_dataStorage->loadDataFromFile()) {
		m_ProgramStatusCEdit.SetWindowTextW(CString("Data base loaded sucsessful"));
		//m_fileToSearchButton.EnableWindow(TRUE);
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
		m_searchVPButton.EnableWindow(FALSE);
		return;
	}
	m_signaturesHandler = std::make_shared<SignatureHandler>();
	m_Player = new CPlayer(m_signaturesHandler);
	m_searchVPButton.EnableWindow(TRUE);
}



void CTestMovieListLibraryDlg::OnBnClickedSearchVp()
{
	
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


		//auto videoPartvec = m_signaturesHandler->getVideoParts();
		//std::vector<double> diffVecScene;
		//std::vector<std::vector<double>> diffVecSceneVec;
		//auto signatures = m_signaturesHandler->getSignatures();
		//size_t signatureIndex = 1;
		//for (auto videoPart : videoPartvec) {
		//	signatureIndex = videoPart->mainSignatureIndex + 1;
		//	while (signatureIndex <= videoPart->lastSignatureIndex) {
		//		diffVecScene.push_back(signatures[videoPart->mainSignatureIndex]->difference(*signatures[signatureIndex]));
		//		signatureIndex++;
		//	}
		//	diffVecSceneVec.push_back(diffVecScene);
		//	diffVecScene = std::vector<double>{};
		//}

		//std::ofstream outFile;
		//outFile.open("Data/ThirdbeginEndCompare_" + std::to_string(c_3) + ".txt");
		//if (outFile.is_open()) {
		//	size_t videoPartIndex = 0;
		//	size_t currIndex = 0;
		//	for (auto diffVec : diffVecSceneVec) {
		//		for (auto diff : diffVec) {
		//			outFile << "firstIndex: " << videoPartvec[videoPartIndex]->mainSignatureIndex << "\tlastIndex: " << videoPartvec[videoPartIndex]->lastSignatureIndex << "\tcurrentIndex: " << currIndex << "\tdiff: " << diff << std::endl;
		//			currIndex++;
		//		}
		//		currIndex++;
		//		outFile << "firstIndex: " << videoPartvec[videoPartIndex]->mainSignatureIndex << "\tlastIndex: " << videoPartvec[videoPartIndex]->lastSignatureIndex << "\tcurrentIndex: " << currIndex << "\tdiff: " << -0.3 << std::endl;
		//		videoPartIndex++;
		//	}
		//}


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



void CTestMovieListLibraryDlg::OnBnClickedLoadDataBase()
{
	CFileDialog dlg(TRUE, _T(""), _T(""), OFN_FILEMUSTEXIST, _T("All Files (*.*)|*.*||"), this);
	if (IDOK == dlg.DoModal()) {
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


void CTestMovieListLibraryDlg::OnBnClickedBaseFromDir()
{
	CString pathToFile;
	CFileDialog dlg(TRUE, _T(""), _T(""), OFN_FILEMUSTEXIST, _T("All Files (*.*)|*.*||"), this);
	if (IDOK == dlg.DoModal()) {
		pathToFile = dlg.GetPathName();
	}

	std::string pathToFileStr = CW2A(pathToFile.GetString());

	auto index = pathToFileStr.find_last_of("\\");
	auto pathToDir = pathToFileStr.substr(0,index);

	for (const auto& file : std::filesystem::directory_iterator(pathToDir)) {

		auto pathToFile = file.path().string();
		auto bstrString = converter.ConvertStringToBSTR(pathToFile);

		if (CheckMovie(bstrString) != S_OK) {
			continue;
		}

		auto signaturesHandler = std::make_shared<SignatureHandler>();
		auto fileInfo = std::make_shared<VideoFileInfo>();

		auto player = new CPlayer(signaturesHandler);
		if (InitPlayer(player, bstrString) != S_OK) {
			delete player;
			continue;
		}
		signaturesHandler->splitVideoToVideoPArt();
		fileInfo->fullPathToFile = pathToFile;
		fileInfo->fileSize = file.file_size();

		auto fileTimeModified = file.last_write_time();
		std::time_t tt = to_time_t(fileTimeModified);
		std::tm* gmt = std::gmtime(&tt);
		std::stringstream buffer;
		buffer << std::put_time(gmt, "%A, %d %B %Y %H:%M");
		std::string formattedFileTime = buffer.str();

		fileInfo->modifiedDate = formattedFileTime;

		m_dataStorage->addSignatureHandler(signaturesHandler,fileInfo);
		delete player;
	}
	if (m_dataStorage->writeToDisk()) {
		m_ProgramStatusCEdit.SetWindowTextW(CString("Base created sucsesful"));
	}
	else {
		m_ProgramStatusCEdit.SetWindowTextW(CString("Base didn't created or folder is empty"));
	}

	
}


void CTestMovieListLibraryDlg::OnBnClickedBrowseNewDatabaseButton()
{
	// TODO: Add your control notification handler code here
}


void CTestMovieListLibraryDlg::OnBnClickedStartSearchVideo()
{
	// TODO: Add your control notification handler code here
}


void CTestMovieListLibraryDlg::OnBnClickedStartNewDataBase()
{
	// TODO: Add your control notification handler code here
}
