#include "../stdafx.h"
#include "SignatureCalculator.h"
#include "Data/Signature.h"
//#include "../BitmapSaver.h"
#include <vfw.h>

//#define __SAVE_IMAGE___
//#define __SAVE_FOURIER__
//-------------------------------------------------------------
CSignatureCalculator::CSignatureCalculator(void)
{
	m_pFFT = NULL;
	m_pF = NULL;
	m_pG = NULL;

	m_width = m_height = 0;
	m_sigSize = 0;
	m_algorithmSens = CGeneralSettings::enum_Sens_Highest;
	m_bInitialized = false;
}

CSignatureCalculator::~CSignatureCalculator(void)
{
	deinitialize();
}
HRESULT CSignatureCalculator::initialize(CGeneralSettings::EnumImageSize imageSize, size_t sigSize, CGeneralSettings::EnumAlgorithmSensitivity algSens)
{
	unsigned int nx = 0, ny = 0, nyp = 0;
	size_t align = sizeof(Complex);
	deinitialize();

	if (sigSize <= 0)
	{
		return E_INVALIDARG;
	}
	switch (imageSize)
	{
	case CGeneralSettings::enum_ImageSize_64x64:
		nx = ny = 64;
		m_width = m_height = 64;
		break;
	case CGeneralSettings::enum_ImageSize_128x128:
		nx = ny = 128;
		m_width = m_height = 128;
		break;
	case CGeneralSettings::enum_ImageSize_256x256:
		nx = ny = 256;
		m_width = m_height = 256;
		break;
	case CGeneralSettings::enum_ImageSize_512x512:
		nx = ny = 512;
		m_width = m_height = 512;
		break;
	default:
		return E_INVALIDARG;
	};
	if (ny != 0 )
	{
		nyp=ny/2+1;
		if ((m_pF = new(std::nothrow) Array::array2<double>(nx,ny,align)) == NULL)
		{
			return E_OUTOFMEMORY;
		}
		if ((m_pG = new(std::nothrow) Array::array2<Complex> (nx,nyp,align)) == NULL)
		{
			deinitialize();
			return E_OUTOFMEMORY;
		}
		if ((m_pFFT = new(std::nothrow) rcfft2d( nx, nx, *m_pF, *m_pG)) == NULL)
		{
			deinitialize();
			return E_OUTOFMEMORY;
		}
	}
	m_sigSize = sigSize;
	m_algorithmSens = algSens;
		
	m_bInitialized = true;

	return S_OK;
}
void CSignatureCalculator::deinitialize(void)
{
	/*SAFE_DELETE(m_pFFT);
	SAFE_DELETE(m_pF);
	SAFE_DELETE(m_pG);

	m_bInitialized = false;*/
}
HRESULT CSignatureCalculator::fillArray(const BYTE *pFrame, DWORD fmt, WORD bitCount)
{
	if (pFrame == NULL)
	{
		return E_INVALIDARG;
	}
	if ((fmt != BI_RGB && fmt != MAKEFOURCC('U','Y','V','Y')))	//Supported only UYVY16 and RGB/RGBA formats
	{
		return E_INVALIDARG;
	}
	if (fmt == BI_RGB)
	{
		if (bitCount == 32) 
		{
			return fillRGBAArray(pFrame);;
		}
		else if (bitCount == 24)
		{
			return fillRGBArray(pFrame);
		}
	}
	if (fmt == MAKEFOURCC('U','Y','V','Y') && bitCount == 16)
	{
		return fillUYVYArray(pFrame);
	}
	return E_INVALIDARG;
}
HRESULT CSignatureCalculator::fillRGBArray(const BYTE *pFrame)	
{
	if (pFrame == NULL)
	{
		return E_INVALIDARG;
	}
	if (!m_bInitialized)
	{
		return E_FAIL;
	}
	for (int j = 0; j < (int)m_height; ++j)	//There is no a line's alignment because we have images only with size multiple by 4
	{
		for (int i = 0; i < (int)m_width; ++i)
		{
			RGBTRIPLE *pTriple = &((RGBTRIPLE*)pFrame)[j*m_width+i];
			double val = 0.2125*pTriple->rgbtRed + 0.7154*pTriple->rgbtGreen + 0.0721*pTriple->rgbtBlue;
			(*m_pF)(i,j) = val/255;
		}
	}
	return S_OK;
}
HRESULT CSignatureCalculator::fillRGBAArray(const BYTE *pFrame)
{
	if (pFrame == NULL)
	{
		return E_INVALIDARG;
	}
	if (!m_bInitialized)
	{
		return E_FAIL;
	}
	for (int j = 0; j < (int)m_height; ++j)
	{
		for (int i = 0; i < (int)m_width; ++i)
		{
			RGBQUAD *pQuad = &((RGBQUAD*)pFrame)[j*m_width+i];
			double val = 0.2125*pQuad->rgbRed + 0.7154*pQuad->rgbGreen + 0.0721*pQuad->rgbBlue;
			(*m_pF)(i,j) = val/255;
		}
	}
	return S_OK;
}
HRESULT CSignatureCalculator::fillUYVYArray(const BYTE *pFrame)
{
	if (pFrame == NULL)
	{
		return E_INVALIDARG;
	}
	if (!m_bInitialized)
	{
		return E_FAIL;
	}
	int width = (int)m_width/2;
	for (int j = 0; j < (int)m_height; ++j)
	{
		for (int i = 0, ii = 0; i < width; ++i)	//Use width/2 (UYVY=BGRA)
		{
			RGBQUAD *pQuad = &((RGBQUAD*)pFrame)[j*width+i];
			(*m_pF)(ii++,j) = (double)pQuad->rgbGreen/255.0f;
			(*m_pF)(ii++,j) = (double)pQuad->rgbReserved/255.0f;
		}
	}
	return S_OK;
}
HRESULT CSignatureCalculator::calculate(const BYTE *pFrame, DWORD fmt, WORD bitCount, CSignature **ppSignature)
{
	if (pFrame == NULL ||
		ppSignature == NULL ||
		*ppSignature != NULL)
	{
		return E_INVALIDARG;
	}
	if (!m_bInitialized)
	{
		return E_FAIL;
	}
	HRESULT hr = S_OK;
	if ((*ppSignature = new(std::nothrow) CSignature) == NULL)
	{
		return E_OUTOFMEMORY;
	}
	if (FAILED(hr = fillArray(pFrame, fmt, bitCount)))
	{
		(*ppSignature)->release();
		*ppSignature = NULL;
		//SAFE_DELETE(*ppSignature);
		
		return hr;
	}
	try
	{
		//Save Y image into file
		//-----------------------------------
#ifdef __SAVE_IMAGE___	
		static int index = 0;
		BYTE *pArray = NULL;
		BYTE color = 0;
		double dblColor = 0;
		if ((index%10)==0)
		{
			static int imageNum = 0;
			pArray = new BYTE[m_width*m_height];
			for (size_t i = 0; i < m_width; ++i)
			{
				for (size_t j = 0; j < m_height; ++j)
				{
					dblColor = (*m_pF)(i,j);
					color = BYTE((double)(*m_pF)(i,j)*255.0f);
					pArray[j*m_height+i] = color;
				}
			}
			CBitmapSaver saver;
			CString fileName;
			fileName.Format(_T("e:\\test\\tempBitmap_OnAir%d.bmp"), imageNum++);
			saver.saveBitmapIntoFileCImage(fileName, m_width, m_height, pArray);
			SAFE_DELETE(pArray);
			index = 0;
		}
		//++index;
#endif
		m_pFFT->fft(*m_pF, *m_pG);
	}
	catch(...)
	{
		(*ppSignature)->release();
		*ppSignature = NULL;
		//SAFE_DELETE(*ppSignature);

		return E_FAIL;
	}
	double real = 0, imag = 0;
	size_t mdim = (size_t)::sqrt(static_cast<double>(m_sigSize));	//matrix dimension

#ifdef __SAVE_FOURIER__
	BYTE *pArray = NULL;
	BYTE color = 0;
	double dblColor = 0;
	static int fourierIndex = 0;
	int fourDim = 64;
	if ((fourierIndex%10)==0)
	{
		static int imageNum = 0;
		pArray = new BYTE[fourDim*fourDim];
		double maxFourier = 0;
		for (size_t i = 0; i < fourDim; ++i)
		{
			for (size_t j = 0; j < fourDim; ++j)
			{
				real = (*m_pG)(i, j ).real();
				imag = (*m_pG)(i, j ).imag();
				dblColor = ::sqrt(real*real + imag*imag);
				if (i != 0 || j != 0)
				{
					maxFourier = (dblColor > maxFourier) ? dblColor : maxFourier;
				}
			}
		}
		for (size_t i = 0; i < fourDim; ++i)
		{
			for (size_t j = 0; j < fourDim; ++j)
			{
				real = (*m_pG)(i, j ).real();
				imag = (*m_pG)(i, j ).imag();
				//dblColor = (int)::sqrt(real*real + imag*imag) % 255;
				color = (BYTE)(((double)::sqrt(real*real + imag*imag)/maxFourier)*255.0f);
				if (i == 0 && j == 0)
					color = 0;
				pArray[j*fourDim+i] = color;
			}
		}
		CBitmapSaver saver;
		CString fileName;
		fileName.Format(_T("e:\\test\\tempFourier_OnAir%d.bmp"), imageNum++);
		saver.saveBitmapIntoFileCImage(fileName, fourDim, fourDim, pArray);
		SAFE_DELETE(pArray);
		fourierIndex = 0;
	}
	//++index;
	/*for (size_t i = 1; i <= mdim; ++i)
	{
	for (size_t j = 1; j <= mdim; ++j)
	{
	real = (*m_pG)(i, j ).real();
	imag = (*m_pG)(i, j ).imag();
	(*ppSignature)->insertNumber(::sqrt(real*real + imag*imag));
	}
	}*/
#endif
	/*for (size_t i = 1; i <= mdim; ++i)
	{
		for (size_t j = 1; j <= mdim; ++j)
		{
			real = (*m_pG)(i, j ).real();
			imag = (*m_pG)(i, j ).imag();
			(*ppSignature)->insertNumber(::sqrt(real*real + imag*imag));
		}
	}*/

	int mdimLess = (int)mdim - 1;
	for (int k = 0; k < (int)mdim; ++k)
	{
		if ((k % 2) != 0)
		{
			for (int j = 0; j <= k; ++j)
			{
				int i = -j + k;
				real = (*m_pG)(i+1, j+1 ).real();
				imag = (*m_pG)(i+1, j+1 ).imag();
				(*ppSignature)->insertNumber(::sqrt(real*real + imag*imag), (double)((int)m_algorithmSens));
			}
		}
		else
		{
			for (int j = k; j >= 0; --j)
			{
				int i = -j + k;
				real = (*m_pG)(i+1, j+1 ).real();
				imag = (*m_pG)(i+1, j+1 ).imag();
				(*ppSignature)->insertNumber(::sqrt(real*real + imag*imag), (double)((int)m_algorithmSens));
			}
		}
	}
	for (int k = (int)mdim; k < (int)mdim*2 - 1; ++k)
	{
		if ((k % 2) != 0)
		{
			for (int j = k - mdimLess; j < (int)mdim; ++j)
			{
				int i = -j + k;
				real = (*m_pG)(i+1, j+1 ).real();
				imag = (*m_pG)(i+1, j+1 ).imag();
				(*ppSignature)->insertNumber(::sqrt(real*real + imag*imag), (double)((int)m_algorithmSens));
			}
		}
		else
		{
			for (int j = mdimLess; j >= k - mdimLess; --j)
			{
				int i = -j + k;
				real = (*m_pG)(i+1, j+1 ).real();
				imag = (*m_pG)(i+1, j+1 ).imag();
				(*ppSignature)->insertNumber(::sqrt(real*real + imag*imag), (double)((int)m_algorithmSens));
			}
		}
	}

	(*ppSignature)->normalize();

	return S_OK;
}
HRESULT CSignatureCalculator::calculateDirect(const BYTE *pFrame, CSignature **ppSignature)
{
	if (pFrame == NULL ||
		ppSignature == NULL ||
		*ppSignature != NULL)
	{
		return E_INVALIDARG;
	}
	if (!m_bInitialized)
	{
		return E_FAIL;
	}
	if ((*ppSignature = new(std::nothrow) CSignature) == NULL)
	{
		return E_OUTOFMEMORY;
	}
	for (int j = 0; j < (int)m_height; ++j)
	{
		for (int i = 0; i < (int)m_width; ++i)
		{
			(*m_pF)(i,j) = pFrame[m_width*j+i];
		}
	}
	double real = 0, imag = 0;
	int mdim = (int)::sqrt(static_cast<double>(m_sigSize));	//matrix dimension

	for (int i = 1; i <= mdim; ++i)
	{
		for (int j = 1; j <= mdim; ++j)
		{
			real = (*m_pG)(i, j ).real();
			imag = (*m_pG)(i, j ).imag();
			(*ppSignature)->insertNumber(::sqrt(real*real + imag*imag), (double)((int)m_algorithmSens));
		}
	}
	(*ppSignature)->normalize();

	return S_OK;
}
bool CSignatureCalculator::isInitialized( void )
{
	return m_bInitialized;
}