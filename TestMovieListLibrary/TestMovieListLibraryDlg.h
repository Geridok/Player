
// TestMovieListLibraryDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "Player.h"
#include "ConverterBSRTtoString.h"

// CTestMovieListLibraryDlg dialog
class CTestMovieListLibraryDlg : public CDialog
{
// Construction
public:
	CTestMovieListLibraryDlg(CWnd* pParent = NULL);// standard constructor
	~CTestMovieListLibraryDlg() {
		delete m_signaturesHandler;
		delete m_Player;
	}
// Dialog Data
	enum { IDD = IDD_TESTMOVIELISTLIBRARY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CEdit m_File;
	CButton m_Start;
	CEdit m_OtpuFileName;
	static const int cTaskFrameCnt = 5*25;
	CPlayer *m_Player;
	CComBSTR m_fileName1;
	CComBSTR m_outFileName;
	CStatic m_textField;
	WorkWithSignatures* m_signaturesHandler;

protected:
	HRESULT InitPlayer();
	HRESULT CheckMovie(CComBSTR fileName);
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnBnClickedStart();
	void GetOutputFileName();
private:
	ConvertBSRTtoString converter;
};
