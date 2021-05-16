#pragma once
#include <atlbase.h>
#include "SignatureCalculator/Data/Signature.h"
#include "SignatureCalculator/Data/GeneralSettings.h"
#include <vector>
#include "ImageFormats.h"
#include <chrono>
#include "VideoPart.h"
#include "videoSplitter.h"
#include <memory>

#define SIGNATURE_SIZE 512
extern std::vector<std::chrono::microseconds> calc_1;
extern std::vector<std::chrono::microseconds> calc_2;
extern bool firstVideo;


class SignatureHandler
{
public:
	SignatureHandler() { 
		HRESULT hres = workWithImage.CoCreateInstance(CLSID_StretchImage);
		ATLASSERT(hres == S_OK);


		size_t nSize = SIGNATURE_SIZE * SIGNATURE_SIZE * 4;
		m_pData.reset(new uint8_t[nSize]);
		m_nDataSize = nSize;

	}
	~SignatureHandler() {
		for (auto it : signatures) {
			it->release();
		}
	}

	void addFrame(MovieListLibraryLib::ShotInfo2 newFrame) {

		/*if (sizeof(newFrame._pShot) > 0) {
			MovieListLibraryLib::ShotInfo2 resizedFrame(newFrame);
			resizedFrame._pShot = m_pData.get();
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
		}*/

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
	void splitVideoToVideoPart() {
		videoParts = splitVideo(signatures);
	}
	
	std::vector<std::shared_ptr<VideoPart>> getVideoParts() const {
		return videoParts;
	}

public:
	size_t selfIndex;

private:
	std::vector<CSignature*> signatures;
	std::vector<std::shared_ptr<VideoPart>> videoParts;
	ATL::CComPtr<IStretchImage> workWithImage;
	std::unique_ptr<uint8_t[]> m_pData;
	size_t m_nDataSize = 0;
};

