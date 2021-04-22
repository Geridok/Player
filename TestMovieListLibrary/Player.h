#pragma once
#include "WorkWithFrames.h"
// CPlayer
class CPlayer
{
public:
	CPlayer(std::shared_ptr<SignatureHandler> signatureHandler) : m_signaturesHandler{std::move(signatureHandler)} { }
	~CPlayer() { 
		pIMovie = NULL;
	}

	HRESULT Init(BSTR file_name);
	void Work();
private:
	std::shared_ptr<SignatureHandler> m_signaturesHandler;
	size_t framesAmount = 0;
	CComPtr<MovieListLibraryLib::ISLTMMovie8> pIMovie;
private:
	HRESULT InitLocal(BSTR file_name);
	bool PrepareTask();
	bool DecodeFrame();
	void ChangePriority();
	HRESULT GetMovieInfo();
};
