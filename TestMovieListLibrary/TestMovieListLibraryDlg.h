
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
		delete m_signaturesHandler_1;
		delete m_Player_1;
	}
// Dialog Data
	enum { IDD = IDD_TESTMOVIELISTLIBRARY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	CEdit m_editFileName_1;
	CEdit m_editFileName_2;

	CButton m_Start;
	CEdit m_OtpuFileName;
	static const int cTaskFrameCnt = 5*25;

	CPlayer *m_Player_1;
	CPlayer *m_Player_2;

	CComBSTR m_bstrFileName_1;
	CComBSTR m_bstrFileName_2;

	CComBSTR m_outFileName;
	CStatic m_textField;

	WorkWithSignatures* m_signaturesHandler_1;
	WorkWithSignatures* m_signaturesHandler_2;

protected:
	HRESULT InitPlayer(CPlayer *player, CComBSTR path);

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
public:
	afx_msg void OnBnClickedBrowse2();
};
