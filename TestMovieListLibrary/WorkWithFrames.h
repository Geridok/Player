#pragma once
#include "stdafx.h"
#include <atlbase.h>
#include "SignatureCalculator/Data/Signature.h"
#include "SignatureCalculator/SignatureCalculator.h"
#include "SignatureCalculator/Data/GeneralSettings.h"
#include <vector>
#include "ImageFormats.h"
#include "ReadWriteSignature.h"
#define SIGNATURE_SIZE 512

class WorkWithSignatures
{
public:
	WorkWithSignatures() {
		calculator.initialize(CGeneralSettings::enum_ImageSize_512x512, 128, CGeneralSettings::enum_Sens_High); 
		HRESULT hres = workWithImage.CoCreateInstance(CLSID_StretchImage);
		ATLASSERT(hres == S_OK);
	}
	~WorkWithSignatures() {
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
			calculator.calculate(resizedFrame._pShot, BI_RGB, 24, &sign);
			if (sign != NULL) {
					signatures.push_back(sign);	
			}
			free(ch);
		}

	}
	void compareVideo(std::string fileNameOutput,size_t step = 1) {
		std::vector<double> diffVec;
		fileNameOutput += ".txt";
		size_t nextShot = step;
		std::ofstream fout(fileNameOutput);
		for (size_t i =	0; i < signatures.size(); i++)
		{
			if (nextShot == signatures.size()) break;
			double diff = signatures[i]->difference(*signatures[nextShot]);
			diffVec.push_back(diff);
			nextShot++;
		}
		size_t i = 1;
		for (auto it : diffVec) {
			fout << "Frame: " + std::to_string(i) + " and: "+ std::to_string(i+step) + "\t" + std::to_string(it) << std::endl;
			i++;
		}
	}
	void saveSignaturesToFile(std::string fileName) {
		if (!signatures.empty()) {
			RWSignature.writeSignaturesToFile(fileName, signatures);
		}
	}
	bool loadSignaturesFromFile(std::string fileName,size_t signaturesAmount) {
		signatures = RWSignature.readSignaturesFromFile(fileName, signaturesAmount);
		if (signatures.empty()) return false;
		return true;
	}
	std::vector<CSignature*> GetSignatures() const {
		return signatures;
	}
private:
	std::vector<CSignature*> signatures;
	CSignatureCalculator calculator;
	ReadWriteSignature RWSignature;
	ATL::CComPtr<IStretchImage> workWithImage;
};

