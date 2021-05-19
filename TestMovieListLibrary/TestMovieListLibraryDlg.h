
// TestMovieListLibraryDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "Player.h"
#include "ConverterBSRTtoString.h"
#include <numeric>
#include "TestMovieListLibrary.h"
#include "SignatureHandler.h"
#include "DataStorage.h"
#include "WorkWithIniFile.h"
#include <filesystem>
#include <chrono>

// CTestMovieListLibraryDlg dialog
class CTestMovieListLibraryDlg : public CDialog
{
// Construction
public:
	CTestMovieListLibraryDlg(CWnd* pParent = NULL);// standard constructor
	~CTestMovieListLibraryDlg() {
		delete m_Player;
	}
// Dialog Data
	enum { IDD = IDD_TESTMOVIELISTLIBRARY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	CButton m_browseFileToSearchButton;

	CEdit m_ProgramStatusCEdit;

	static const int cTaskFrameCnt = 5*25;

	CPlayer *m_Player;

	CComBSTR m_bstrFileName_1;
	CComBSTR m_bstrFileName_2;

	CComBSTR m_bsrtOutFileName;
	CStatic m_textField;

	std::shared_ptr<SignatureHandler> m_signaturesHandler;

protected:
	HRESULT InitPlayer(CPlayer *player, CComBSTR path);
	void ReloadPlayer();
	HRESULT CheckMovie(CComBSTR fileName);
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedLoadDataBase();
	afx_msg void OnBnClickedAdd();
	//void GetOutputFileName();
private:
	ConvertBSRTtoString converter;
	DataStorage *m_dataStorage;
	CString m_pathToFolderNewDataBase;
public:
	//afx_msg void OnBnClickedBrowse2();

	afx_msg void OnBnClickedBrowseFileToCompare();
	afx_msg void OnBnClickedBaseFromDir();
	CButton m_getPathToDirCButton;
	CEdit m_pathToDataBaseCEdit;
	CEdit m_fileNameToCompareCEdit;
	afx_msg void OnBnClickedBrowseNewDatabaseButton();
	CEdit m_pathToNewDataBaseCEdit;
	afx_msg void OnBnClickedStartSearchVideo();
	CButton m_startSearchButton;
	CButton m_startCreationNewDataBase;
	afx_msg void OnBnClickedStartNewDataBase();
	afx_msg void OnBnClickedStartCorrelation();
	CButton m_startCorrelationButton;

};
