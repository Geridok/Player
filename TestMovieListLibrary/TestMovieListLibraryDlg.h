
// TestMovieListLibraryDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "Player.h"
#include "ConverterBSRTtoString.h"
#include "SignatureComparator.h"
#include <numeric>
#include "TestMovieListLibrary.h"
#include "WorkWithFrames.h"
#include "SignatureComparator.h"
#include "DataInformation.h"

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

	CButton m_startButton;
	CButton m_fileToSearchButton;
	CButton m_AddFileToDataBaseButton;

	CEdit m_fileNameToCompareCEdit;
	CEdit m_FileNameToAddCEdit;

	CEdit m_AvgCalcTimeCEdit;
	CEdit m_AvgCompTimeCEdit;

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
	afx_msg void OnBnClickedAddToDataBase();
	afx_msg void OnBnClickedStart();
	//void GetOutputFileName();
private:
	ConvertBSRTtoString converter;
	SignatureComparator comparator;
	DataInformation dataStorage;
	bool isAddAction = true;
public:
	//afx_msg void OnBnClickedBrowse2();

	afx_msg void OnBnClickedBrowseFileToCompare();
};
