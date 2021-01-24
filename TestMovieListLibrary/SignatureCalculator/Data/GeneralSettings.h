#pragma once

class CGeneralSettings
{
public:
	CGeneralSettings(void);
	~CGeneralSettings(void);
	CGeneralSettings(const CGeneralSettings &settings);
	CGeneralSettings& operator=(const CGeneralSettings &settings);

	enum EnumImageSize { enum_ImageSize_64x64=0, enum_ImageSize_128x128, enum_ImageSize_256x256, enum_ImageSize_512x512 };
	enum EnumAlgorithmSensitivity { enum_Sens_Lowest=32, enum_Sens_Low=16, enum_Sens_Medium=8, enum_Sens_High=4, enum_Sens_Highest=2 };

	HRESULT load(IXMLDOMNode *pSettingsNode);
	HRESULT save(IXMLDOMNode *pParentNode);

	size_t getCrop() const { return m_crop; }
	void setCrop(size_t val) { m_crop = val; }
	size_t getSigSize() const { return m_sigSize; }
	void setSigSize(size_t val) { m_sigSize = val; }
	CGeneralSettings::EnumImageSize getImageSize() const { return m_imageSize; }
	bool getImageSize(size_t &w, size_t &h);
	void setImageSize(CGeneralSettings::EnumImageSize val) { m_imageSize = val; }
	EnumAlgorithmSensitivity getAlgorithmSens() { return m_algorithmSens; }
	void setAlgorithmSens(EnumAlgorithmSensitivity algSens) { m_algorithmSens = algSens; }

	const size_t getDefaultSigSize() const { return m_cDefaultSigSize; }
	const size_t getDefaultCrop() const { return m_cDefaultCrop; }
private:
	size_t m_crop;		//Crop is in percent
	size_t m_sigSize;	//Amount of coefficients from Fourier transform
	EnumImageSize m_imageSize;
	EnumAlgorithmSensitivity m_algorithmSens;
	CComAutoCriticalSection m_critSec;

	//constants
	const size_t m_cDefaultCrop;
	const size_t m_cDefaultSigSize;
	
};
