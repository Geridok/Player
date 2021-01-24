#include "StdAfx.h"
#include "Player.h"


HRESULT CPlayer::InitLocal(BSTR file_name)
{
	CComPtr<MovieListLibraryLib::ISLTMMovieCreator> pICreator;
	HRESULT hres = pICreator.CoCreateInstance(MovieListLibraryLib::CLSID_SLTMMovieCreator);
	ATLASSERT(hres==S_OK);  if( hres != S_OK )  return hres;
	CComPtr<MovieListLibraryLib::ISLTMMovie> pIUnk;
	hres = pICreator->CreateMovie(MovieListLibraryLib::eMovieType_Film, file_name, 10,0,0, &pIUnk); 
	ATLASSERT(hres==S_OK);  if( hres != S_OK )  return hres;
	hres = pIUnk->QueryInterface(&pIMovie);
	ATLASSERT(hres==S_OK);  if( hres != S_OK )  return hres;
	if (!PrepareTask()) ATLASSERT(TRUE);
	return S_OK;
}



HRESULT CPlayer::Init(BSTR file_name)
{
	CComPtr<MovieListLibraryLib::ISLTMMovieTypes> pIMovieCreator;
	HRESULT hres = pIMovieCreator.CoCreateInstance(MovieListLibraryLib::CLSID_SLTMMovieCreator);
	ATLASSERT(hres == S_OK);  if (hres != S_OK)  return hres;
	MovieListLibraryLib::SLTMMovieType movie_type = MovieListLibraryLib::eMovieType_Pause;
	hres = pIMovieCreator->GetFileType(file_name, &movie_type);
	ATLASSERT(hres == S_OK);  if (hres != S_OK)  return hres;
	InitLocal(file_name);
	// create working thread
	GetMovieInfo();
	PrepareTask();
	this->Work();
	return S_OK;
}
//

HRESULT CPlayer::GetMovieInfo()
{
	// obtain task info
	CComQIPtr<MovieListLibraryLib::ISLTMMovie8> pIMovie8(pIMovie);
	pIMovie8->Unprepare();
	MovieListLibraryLib::SLTMMovieInfoEx3 info3;  CComBSTR fname;
	ZeroMemory(&info3,sizeof(info3));
	info3._cb = sizeof(info3);
	HRESULT hres = pIMovie8->GetInfoEx3(&info3,&fname);
	framesAmount = info3._durationShots;
	ATLASSERT(hres==S_OK);  if( hres != S_OK )  return hres;
	return S_OK;
}
//
void CPlayer::Work()
{
	while (framesAmount > 0)
	{
		if (DecodeFrame()) {
			framesAmount--;
		}
		else {
			ATLASSERT(TRUE);
		}
	}
	size_t op = 0;
}
//
bool CPlayer::PrepareTask()
{
	CComQIPtr<MovieListLibraryLib::ISLTMMovieAudioN> pIAudioN(pIMovie);
	ATLASSERT(pIAudioN != NULL);  if( pIAudioN==NULL )  return false;
	MovieListLibraryLib::PrepareInfoMulti minfo;
	ZeroMemory(&minfo,sizeof(minfo));
	minfo._cb = sizeof(minfo);
	minfo._useSLIni = FALSE;
	minfo._sampleRate = 48000;
	minfo._channels = 2;
	minfo._sampleType = MovieListLibraryLib::SoundSampeType_Float;
	minfo._useAudioStreamsCount = 0;
	minfo._useAudioStreams[0] = 0;
	minfo._targetUpperFieldFirst = TRUE;
	minfo._pSwapFields = FALSE;
	minfo._requestedFormatsOrder[0] = 7;
	minfo._requestedFormatsOrder[1] = 7;
	minfo._requestedFormatsOrder[2] = 7;
	minfo._requestedFormatsOrder[3] = 7;
	minfo._resultFormat = 0xFFFFFFFF;
	minfo._withAlpha = FALSE;
	HRESULT hres = pIAudioN->PrepareMulti(&minfo);
	ATLASSERT(hres==S_OK);  if( hres != S_OK )  return false;
	CComQIPtr<MovieListLibraryLib::ISLTMMovieReadPriority> pIPriority(pIMovie);
	ATLASSERT(pIPriority != NULL);  if( pIPriority==NULL )  return false;
	pIPriority->SetReadPriorityLow(TRUE);
	return true;
}

void CPlayer::ChangePriority()
{
	CComQIPtr<MovieListLibraryLib::ISLTMMovieReadPriority> pIPriority(pIMovie);
	ATLASSERT(pIPriority != NULL);  if( pIPriority==NULL )  return;
	pIPriority->SetReadPriorityLow(FALSE);
}

bool CPlayer::DecodeFrame()
{
	long tmp;
	HRESULT hres = pIMovie->IncrementShotNumber(1,&tmp);
	if (hres == S_OK) {}
	else if( hres==MovieListLibraryLib::eS_Skip_Data_No_Increment )  return false;
	else if( hres==MovieListLibraryLib::eS_Skip_Data_Increment ||
			 hres==MovieListLibraryLib::eS_Repeat_Last_Frame || hres==S_FALSE )  return true;
	else {
		ATLASSERT(TRUE);  return false;
	}
	MovieListLibraryLib::ShotInfo2 info2;
	ZeroMemory(&info2,sizeof(info2));
	info2._cb = sizeof(info2);
	hres = pIMovie->GetCurrShotEx2(&info2, NULL);
	m_signaturesHandler->addFrame(info2);
	ATLASSERT(hres==S_OK);  if( hres != S_OK )  return false;
	return true;
}
