#pragma once
#include "stdafx.h"
#include "SignatureCalculator/Data/Signature.h"
#include <string>
#include <iostream>
#include <fstream>
class ReadWriteSignature
{
public:
	std::vector<CSignature*> readSignaturesFromFile(std::string fileName,size_t signature_amount) {

		std::vector<CSignature*> signaturesVec;
		for (size_t i = 0; i < signature_amount; i++)
		{
			CSignature *newSignature = new CSignature();
			std::string realFileName = fileName + "_" + std::to_string(i) + ".sig";
			std::ifstream inputStream(realFileName.c_str(), std::ios::binary);
			if (inputStream) {
				size_t signatureSize = 0;
				inputStream >> signatureSize;
				char* signature = new char[signatureSize];
				inputStream.read(signature, signatureSize);
				newSignature->loadFromString(CString(signature));
				signaturesVec.push_back(newSignature);
			}
		}
		return signaturesVec;
	}
	void writeSignaturesToFile(std::string fileName, std::vector<CSignature*> signatures) {
		size_t count = 0;
		for (auto it : signatures) {
			std::string realFileName = fileName + "_" + std::to_string(count) + ".sig";
			std::ofstream outputStream(realFileName.c_str(), std::ios::binary);
			outputStream << it->getSize();
			CString signature;
			it->saveToString(signature);
			char* c_signature = new char[it->getSize()];
			sprintf(c_signature,"%S",signature);
			outputStream.write(c_signature, it->getSize());
			delete[]c_signature;
			count++;
		}
	}
private:
	
};

