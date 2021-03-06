#pragma once
#include "WorkWithFrames.h"

// CPlayer
class CPlayer
{
public:
	CPlayer(SignatureHandler *signatureHandler) : m_signaturesHandler{signatureHandler} { }
	~CPlayer() { 
		pIMovie = NULL;
	}

	HRESULT Init(BSTR file_name);
	void Work();
private:
	SignatureHandler *m_signaturesHandler;
	size_t framesAmount = 0;
	CComPtr<MovieListLibraryLib::ISLTMMovie8> pIMovie;
private:
	HRESULT InitLocal(BSTR file_name);
	bool PrepareTask();
	bool DecodeFrame();
	void ChangePriority();
	HRESULT GetMovieInfo();
};
