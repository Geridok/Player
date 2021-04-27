#pragma once
#include <atlbase.h>
#include "SignatureCalculator/Data/Signature.h"
#include "SignatureCalculator/SignatureCalculator.h"
#include "SignatureCalculator/Data/GeneralSettings.h"
#include <vector>
#include "ImageFormats.h"
#include <chrono>
#include "VideoPart.h"
#include "videoSplitter.h"
#define SIGNATURE_SIZE 512
extern std::vector<std::chrono::microseconds> calc_1;
extern std::vector<std::chrono::microseconds> calc_2;
extern bool firstVideo;
extern double c_3;

class SignatureHandler
{
public:
	SignatureHandler() {
		calculator.initialize(CGeneralSettings::enum_ImageSize_512x512, 128, CGeneralSettings::enum_Sens_High); 
		HRESULT hres = workWithImage.CoCreateInstance(CLSID_StretchImage);
		ATLASSERT(hres == S_OK);
	}
	~SignatureHandler() {
		for (auto it : signatures) {
			it->release();
		}
	}

	void addFrame(MovieListLibraryLib::ShotInfo2 newFrame) {
		
		if (sizeof(newFrame._pShot) > 0) {
			MovieListLibraryLib::ShotInfo2 resizedFrame(newFrame);
			unsigned char* ch = (unsigned char*)malloc(resizedFrame._shotSizeBytes);
			resizedFrame._pShot = ch;
			workWithImage->StretchImage(resizedFrame._pShot, newFrame._pShot, SIGNATURE_SIZE, SIGNATURE_SIZE, newFrame._cx, newFrame._cy, SIGNATURE_SIZE * 3, newFrame._cx * 3, FORMAT_BGR);
			CSignature *sign = NULL;
			auto start = std::chrono::high_resolution_clock::now();
			calculator.calculate(resizedFrame._pShot, BI_RGB, 24, &sign);
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
			if (firstVideo) { 
				calc_1.push_back(duration);
			}
			else {
				calc_2.push_back(duration);
			}
			if (sign != NULL) {
					signatures.push_back(sign);	
			}
			free(ch);
		}

	}
	bool addSignature(std::vector<CSignature*> signatures) {
		if (this->signatures.empty()) {
			this->signatures = signatures;
			return true;
		}
		return false;
		
	}

	bool addVideoParts(std::vector<std::shared_ptr<VideoPart>> videoParts) {
		if (this->videoParts.empty()) {
			this->videoParts = videoParts;
			return true;

		}
		return false;
	}
	void saveSignaturesToFile(std::string fileName) {
		if (!signatures.empty()) {
			//rwData.writeSignaturesToFile(fileName, signatures);
		}
	}
	bool loadSignaturesFromFile(std::string fileName,size_t signaturesAmount) {
		//signatures = data::readSignaturesFromFile(fileName, signaturesAmount);
		if (signatures.empty()) return false;
		return true;
	}
	std::vector<CSignature*> getSignatures() const {
		return signatures;
	}

	size_t getSigAmount() const {
		return signatures.size();
	}
	void splitVideoToVideoPArt() {
		videoParts = splitVideo(0.6, 0.2, signatures);
	}
	
	std::vector<std::shared_ptr<VideoPart>> getVideoParts() const {
		return videoParts;
	}

public:
	size_t selfIndex;

private:
	std::vector<CSignature*> signatures;
	std::vector<std::shared_ptr<VideoPart>> videoParts;
	CSignatureCalculator calculator;
	ATL::CComPtr<IStretchImage> workWithImage;
};

