#pragma once
#include "FFTW/fftw++.h"
#include "FFTW/array.h"
#include "Data/GeneralSettings.h"
class CSignature;

class CSignatureCalculator
{
public:
	// default constructor & destructor
	CSignatureCalculator(void);
	~CSignatureCalculator(void);
	// init & free local resources
	HRESULT initialize(CGeneralSettings::EnumImageSize imageSize, size_t sigSize,
		CGeneralSettings::EnumAlgorithmSensitivity algSens);	//create arrays which are necessary for proper working
	void deinitialize(void);
	bool isInitialized(void);
	// main function - build signature for specified image
	HRESULT calculate(const BYTE *pFrame, DWORD fmt, WORD bitCount, CSignature **ppSignature);

protected:
	HRESULT calculateDirect(const BYTE *pFrame, CSignature **ppSignature);		//Assumes that pFrame is Y component of the frame

private:
	rcfft2d *m_pFFT;	//Compute the complex two-dimensional Fourier transform
	Array::array2<double> *m_pF;//(nx,ny,align);
	Array::array2<Complex> *m_pG;//(nx,ny,align);

	size_t m_width, m_height;
	size_t m_sigSize;
	CGeneralSettings::EnumAlgorithmSensitivity m_algorithmSens;

	bool m_bInitialized;
	HRESULT fillArray(const BYTE *pFrame, DWORD fmt, WORD bitCount);
	HRESULT fillRGBAArray(const BYTE *pFrame);
	HRESULT fillRGBArray(const BYTE *pFrame);
	HRESULT fillUYVYArray(const BYTE *pFrmae);
};
