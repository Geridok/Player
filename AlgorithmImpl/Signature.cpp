//#include "stdafx.h"
#include "Signature.h"
#include <math.h>

//#define  __Debug__
#define FOURIER_COEFF_MIN 2.0f
#define SIGNATURE_DIFF_DELTA 0.2f

CSignature::CSignature()
		: m_cDefaultThreshold(0.3)
		, m_cDefaultThresholdBetweenShots(0.5)
		, m_sum(0)
		, m_refCounter(1)
{
	m_signature.reserve(10);
	m_threshold = m_cDefaultThreshold;
	m_thresholdBetweenShots = m_cDefaultThresholdBetweenShots;

}
CSignature::~CSignature()
{
}
/*
double CSignature::difference(const CSignature &signature)
{
	double result = 0.0;
	CComCritSecLock<CComAutoCriticalSection> lock(m_critSec, TRUE);
	size_t size1 = m_signature.size();
	size_t size2 = signature.m_signature.size();
	double diffCounter = 0;
	size_t i = 0;
	for (i = 0; i < size1 && i < size2; ++i)
	{
		if (m_signature[i] + signature.m_signature[i] > 0)
		{
			if (::fabs(m_signature[i] - signature.m_signature[i])/(m_signature[i] + signature.m_signature[i]) > SIGNATURE_DIFF_DELTA)
			{
				++diffCounter;
			}
		}
	}
	result = diffCounter/(double)i;
#ifdef __Debug__
	CString str;
	str.Format(_T("Diff: %f\n"), result);
	OutputDebugString(str);
#endif
	return result;
}*/

double CSignature::difference(const CSignature &signature)
{
	double result = 0.0;
	CComCritSecLock<CComAutoCriticalSection> lock(m_critSec, TRUE);
	size_t size1 = m_signature.size();
	size_t size2 = signature.m_signature.size();
	for (size_t i = 0, j = 0; i < size1 && j < size2; ++i, ++j)
	{
		result += ::fabs(m_signature[i] - signature.m_signature[j]);
	}
#ifdef __Debug__
	CString str;
	str.Format(_T("Diff: %f\n"), result);
	OutputDebugString(str);
#endif

	// Difference normalization to (0..1).Max value of sum(|Ai-Bi|) (where Sum(Ai) = 1, sum(Bi) = 1) is 2

	//return result/2.0f;
	return result <= 1.0f ? result : 1.0f;
}

bool CSignature::compare(const CSignature &signature)
{
	if (difference(signature) > m_threshold)
	{
		return false;
	}
	return true;
}
bool CSignature::compareShots(const CSignature &signature)
{
	if (difference(signature) > m_thresholdBetweenShots)
	{
		return false;
	}
	return true;
}
void CSignature::normalize(void)
{
	if (m_sum == 0)
		return;
	CComCritSecLock<CComAutoCriticalSection> lock(m_critSec, TRUE);
	size_t size = m_signature.size();
	for (size_t i = 0; i < size; ++i)
	{
		m_signature[i] /= m_sum;
	}
}
void CSignature::insertNumber(double num, double fourierCoeffMin)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_critSec, TRUE);
	num = (num <= fourierCoeffMin) ? 0 : num;
	m_sum += num;
	m_signature.push_back(num);
	//normalize();
}
void CSignature::loadFromString(const CString &sigStr)
{
/*
#ifdef __Debug__
	OutputDebugString(sigStr + _T("\n"));
#endif*/


	CString str = sigStr;
	str += ' ';	//Add ' ' symbol to the end of the string to generalize parse algorithm
	int pos = -1, prevPos = 0;
	CComCritSecLock<CComAutoCriticalSection> lock(m_critSec, TRUE);
	m_signature.clear();
	m_sum = 0;
	while ((pos = str.Find(' ', prevPos + 1)) != -1)
	{
		CString digit = str.Mid(prevPos, pos-prevPos);
		double num = 0;
		prevPos = pos;
		num = ::_wtof(digit);
		m_signature.push_back(num);
		//m_sum += num;
	}
}
void CSignature::saveToString(CString &sigStr)
{
	sigStr = _T("");
	CComCritSecLock<CComAutoCriticalSection> lock(m_critSec, TRUE);
	CString tempStr;
	for (std::vector<double>::iterator iter = m_signature.begin(); iter != m_signature.end(); ++iter)
	{
		tempStr.Format(_T(" %0.5f"), (*iter));
		sigStr += tempStr;
	}
}

void CSignature::addRef()
{
	::InterlockedIncrement(&m_refCounter);
}

void CSignature::release()
{
	if (::InterlockedDecrement(&m_refCounter) == 0)
	{
		delete this;
	}
}