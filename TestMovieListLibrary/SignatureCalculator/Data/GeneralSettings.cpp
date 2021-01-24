#include "StdAfx.h"
#include "GeneralSettings.h"

CGeneralSettings::CGeneralSettings(void)
		:	m_cDefaultCrop(5)
		,	m_cDefaultSigSize(64)
{
	m_sigSize = m_cDefaultSigSize;
	m_crop = m_cDefaultCrop;
	m_imageSize = enum_ImageSize_128x128;
	m_algorithmSens = enum_Sens_Highest;

}
CGeneralSettings::~CGeneralSettings(void)
{
}
CGeneralSettings::CGeneralSettings(const CGeneralSettings &settings)
		:	m_cDefaultCrop(5)
		,	m_cDefaultSigSize(64)
{
	*this = settings;
}
CGeneralSettings& CGeneralSettings::operator =(const CGeneralSettings &settings)
{
	m_crop = settings.m_crop;
	m_imageSize = settings.m_imageSize;
	m_sigSize = settings.m_sigSize;
	m_algorithmSens = settings.m_algorithmSens;

	return *this;
}
HRESULT CGeneralSettings::load(IXMLDOMNode *pSettingsNode)
{
	if (pSettingsNode == NULL)
	{
		return E_INVALIDARG;
	}
	CComQIPtr<IXMLDOMElement> pSettingsElement(pSettingsNode);
	if (pSettingsElement == NULL)
	{
		return E_NOINTERFACE;
	}

	CComVariant attrVal;
	CComCritSecLock<CComAutoCriticalSection> lock(m_critSec, TRUE);

	m_crop = m_cDefaultCrop;
	pSettingsElement->getAttribute(CComBSTR(_T("crop")), &attrVal);
	if (attrVal.vt != VT_NULL)
	{
		attrVal.ChangeType(VT_UINT);
		m_crop = attrVal.uintVal;
		attrVal.Clear();
	}

	m_sigSize = m_cDefaultSigSize;
	pSettingsElement->getAttribute(CComBSTR(_T("sigSize")), &attrVal);
	if (attrVal.vt != VT_NULL)
	{
		attrVal.ChangeType(VT_UINT);
		m_sigSize = attrVal.uintVal;
		attrVal.Clear();
	}

	m_imageSize = enum_ImageSize_128x128;
	pSettingsElement->getAttribute(CComBSTR(_T("imageSize")), &attrVal);
	if (attrVal.vt != VT_NULL)
	{
		attrVal.ChangeType(VT_INT);
		m_imageSize = (EnumImageSize)attrVal.intVal;
		attrVal.Clear();
	}

	m_algorithmSens = enum_Sens_Highest;
	pSettingsElement->getAttribute(CComBSTR(_T("sensitivity")), &attrVal);
	if (attrVal.vt != VT_NULL)
	{
		attrVal.ChangeType(VT_INT);
		m_algorithmSens = (EnumAlgorithmSensitivity)attrVal.intVal;
		attrVal.Clear();
	}

	return S_OK;
}
HRESULT CGeneralSettings::save(IXMLDOMNode *pParentNode)
{
	if (pParentNode == NULL)
		return E_INVALIDARG;
	CComPtr<IXMLDOMDocument> pDocument;
	HRESULT hr = S_OK;
	if (FAILED(hr = pParentNode->get_ownerDocument(&pDocument)))
	{
		return hr;
	}
	CComPtr<IXMLDOMElement> pSettingsElement;
	if (FAILED(hr = pDocument->createElement(CComBSTR(_T("GeneralSettings")), &pSettingsElement)))
	{
		return hr;
	}
	CComVariant attrVal;
	CComCritSecLock<CComAutoCriticalSection> lock(m_critSec, TRUE);

	attrVal = m_crop;
	if (FAILED(hr = pSettingsElement->setAttribute(CComBSTR(_T("crop")), attrVal)))
	{
		return hr;
	}
	attrVal.Clear();

	attrVal = m_sigSize;
	if (FAILED(hr = pSettingsElement->setAttribute(CComBSTR(_T("sigSize")), attrVal)))
	{
		return hr;
	}
	attrVal.Clear();

	attrVal = (UINT)m_imageSize;
	if (FAILED( hr = pSettingsElement->setAttribute(CComBSTR(_T("imageSize")), attrVal)))
	{
		return hr;
	}
	attrVal.Clear();

	attrVal = (UINT)m_algorithmSens;
	if (FAILED( hr = pSettingsElement->setAttribute(CComBSTR(_T("sensitivity")), attrVal)))
	{
		return hr;
	}
	attrVal.Clear();

	if (FAILED(hr = pParentNode->appendChild(pSettingsElement, NULL)))
	{
		return hr;
	}

	return S_OK;
}

bool CGeneralSettings::getImageSize( size_t &w, size_t &h )
{
	switch(m_imageSize)
	{
	case enum_ImageSize_64x64:
		w = h = 64;
		break;
	case enum_ImageSize_128x128:
		w = h = 128;
		break;
	case enum_ImageSize_256x256:
		w = h = 256;
		break;
	case enum_ImageSize_512x512:
		w = h = 512;
		break;
	default:
		return false;
	};
	return true;
}