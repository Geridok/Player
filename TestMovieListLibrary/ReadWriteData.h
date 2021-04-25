#pragma once
#include "SignatureCalculator/Data/Signature.h"
#include <string>
#include <iostream>
#include <fstream>
#include "VideoPart.h"
#include <json/writer.h>
#include <json/value.h>
#include <json/json.h>
#include "WorkWithFrames.h"
#include <algorithm>

class ReadWriteData
{
private:
	/* Here will be the instance stored. */
	static ReadWriteData* instance;

	/* Private constructor to prevent instancing. */
	ReadWriteData();
public:
	/* Static access method. */
	static ReadWriteData* getInstance();

public:
	static const std::string dataFileName;
	static const std::string baseName;
	static std::vector<CSignature*> readSignaturesFromFile(std::string fileName, size_t signature_amount) {

		std::vector<CSignature*> signaturesVec;
		for (size_t i = 0; i < signature_amount; i++)
		{
			CSignature* newSignature = new CSignature();
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
	static void writeSignaturesToFile(std::string fileName, std::vector<CSignature*> signatures) {
		size_t count = 0;
		for (auto it : signatures) {
			std::string realFileName = fileName + "_" + std::to_string(count) + ".sig";
			std::ofstream outputStream(realFileName.c_str(), std::ios::binary);
			outputStream << it->getSize();
			CString signature;
			it->saveToString(signature);
			char* c_signature = new char[it->getSize()];
			sprintf(c_signature, "%S", signature);
			outputStream.write(c_signature, it->getSize());
			delete[]c_signature;
			count++;
		}
	}
	void writeDataToFile(std::vector<std::shared_ptr<SignatureHandler>> signatureHandlerVec) {

		std::ofstream fileId;
		fileId.open(dataFileName, std::ios::out | std::ios::app);

		Json::Value event;
		Json::Value signatureVecString(Json::arrayValue);
		size_t count = 0;
		for (auto singleSignatureHandler : signatureHandlerVec) {
			std::string sigHandlerName = "signatureHandler_" + std::to_string(count);
			auto signatureVec = singleSignatureHandler->getSignatures();
			for (auto it : signatureVec) {
				CString signature;
				it->saveToString(signature);
				char* c_signature = new char[it->getSize()];
				std::string signString(c_signature);
				delete[]c_signature;
				signatureVecString.append(Json::Value(signString));
			}
			event[ReadWriteData::baseName][sigHandlerName]["selfIndex"] = singleSignatureHandler->selfIndex;
			event[ReadWriteData::baseName][sigHandlerName]["signatures"] = signatureVecString;
			auto videoPartVec = singleSignatureHandler->getVideoParts();
			size_t index = 0;
			for (auto singleVideoPart : videoPartVec) {
				std::string videoPartName = "videoPart+" + std::to_string(index);
				event[ReadWriteData::baseName][sigHandlerName][videoPartName][VideoPartNames::mainSignatureIndex] = singleVideoPart->mainSignatureIndex;
				event[ReadWriteData::baseName][sigHandlerName][videoPartName][VideoPartNames::lastSignatureIndex] = singleVideoPart->lastSignatureIndex;
				event[ReadWriteData::baseName][sigHandlerName][videoPartName][VideoPartNames::selfIndex] = singleVideoPart->selfIndex;
				index++;
			}
		}


		fileId << event;

		fileId.close();

	}

	void writeSignatureHandlerToFile(std::shared_ptr<SignatureHandler> signatureHandler, size_t signatureHandlerIndex) {

		std::ofstream fileId;
		fileId.open(dataFileName);

		Json::Value signatureVecString(Json::arrayValue);

		std::string sigHandlerName = "signatureHandler_" + std::to_string(signatureHandlerIndex);
		auto signatureVec = signatureHandler->getSignatures();
		for (auto it : signatureVec) {
			CString signature;
			it->saveToString(signature);
			CT2CA pszConvertedAnsiString(signature);
			std::string signString(pszConvertedAnsiString);
			signatureVecString.append(Json::Value(signString));
		}
		m_event[ReadWriteData::baseName][sigHandlerName]["selfIndex"] = signatureHandler->selfIndex;
		m_event[ReadWriteData::baseName][sigHandlerName]["signatures"] = signatureVecString;
		auto videoPartVec = signatureHandler->getVideoParts();
		size_t partIndex = 0;
		for (auto singleVideoPart : videoPartVec) {
			auto videoPartName = "videoPart_" + std::to_string(partIndex);
			m_event[ReadWriteData::baseName][sigHandlerName][videoPartName][VideoPartNames::mainSignatureIndex] = singleVideoPart->mainSignatureIndex;
			m_event[ReadWriteData::baseName][sigHandlerName][videoPartName][VideoPartNames::lastSignatureIndex] = singleVideoPart->lastSignatureIndex;
			m_event[ReadWriteData::baseName][sigHandlerName][videoPartName][VideoPartNames::selfIndex] = singleVideoPart->selfIndex;
			partIndex++;
		}

		fileId << m_event;

		fileId.close();
	}
	std::vector<std::shared_ptr<SignatureHandler>> readAllDataFromFile() {
		Json::Value root;   // will contain the root value after parsing.
		Json::CharReaderBuilder builder;
		std::ifstream test(ReadWriteData::dataFileName, std::ifstream::binary);
		if (!test.is_open()) {
			return {};
		}
		std::string errs;
		bool ok = Json::parseFromStream(builder, test, &root, &errs);
		if (!ok)
		{
			// report to the user the failure and their locations in the document.
			std::cout << errs << "\n";
		}

		std::vector<std::shared_ptr<SignatureHandler>> signatureHandlerVec;
		size_t handlerIndex = 0;
		while (1) {
			auto signatureHandlerName = "signatureHandler_" + std::to_string(handlerIndex);
			auto signaturesJsonArr = root[ReadWriteData::baseName][signatureHandlerName]["signatures"];
			if (signaturesJsonArr == NULL) {
				break;
			}
			auto newSignatureHandler = std::make_shared<SignatureHandler>();
			std::vector<CSignature*> signatureVec;
			Json::String str;
			for (Json::Value::const_iterator signature = signaturesJsonArr.begin(); signature != signaturesJsonArr.end(); ++signature) {
				CSignature* newSignature = new CSignature();
				str = signature->asString();
				std::string _Str = str;
				newSignature->loadFromString(CString(_Str.c_str()));
				signatureVec.push_back(newSignature);
			}
			if (signatureVec.empty()) {
				if (signatureHandlerVec.empty()) {
					return {};
				}
				break;
			}
			newSignatureHandler->addSignature(signatureVec);
			newSignatureHandler->selfIndex = root[ReadWriteData::baseName][signatureHandlerName]["selfIndex"].asUInt();
			size_t partIndex = 0;
			std::vector<std::shared_ptr<VideoPart>> videoPartsVec;
			while (1) {
				auto videoPartName = "videoPart_" + std::to_string(partIndex);
				auto lastSignatureIndex = root[ReadWriteData::baseName][signatureHandlerName][videoPartName][VideoPartNames::lastSignatureIndex].asUInt();
				auto mainSignatureIndex = root[ReadWriteData::baseName][signatureHandlerName][ videoPartName][VideoPartNames::mainSignatureIndex].asUInt();
				auto selfIndex = root[ReadWriteData::baseName][signatureHandlerName][ videoPartName][VideoPartNames::selfIndex].asUInt();
				if (lastSignatureIndex == 0 && selfIndex == 0 && mainSignatureIndex == 0) {
					if (videoPartsVec.empty()) {
						return {};
					}
					break;
				}
				auto newVideoPartPtr = std::make_shared<VideoPart>(mainSignatureIndex, lastSignatureIndex, selfIndex);
				videoPartsVec.push_back(newVideoPartPtr);
				partIndex++;
			}
			if (videoPartsVec.empty()) {
				std::cout << "Some error in reading file" << std::endl;
				return {};
			}
			newSignatureHandler->addVideoParts(videoPartsVec);
			signatureHandlerVec.push_back(newSignatureHandler);
			handlerIndex++;
		}
		if (!signatureHandlerVec.empty()) {
			sortData(signatureHandlerVec);
		}
		return signatureHandlerVec;

	}
	private:

	 void sortData(std::vector<std::shared_ptr<SignatureHandler>> &signatureHandlerVec) {
		for (auto signatureHandler : signatureHandlerVec) {
			auto videoParts = signatureHandler->getVideoParts();
			if (!videoParts.empty()) {
				std::sort(videoParts.begin(), videoParts.end(), [](std::shared_ptr<VideoPart> const& s1, std::shared_ptr<VideoPart> const& s2)->bool {
					return s1->selfIndex < s2->selfIndex;
					});
			}
		}
		if (!signatureHandlerVec.empty()) {
			std::sort(signatureHandlerVec.begin(), signatureHandlerVec.end(), [](std::shared_ptr<SignatureHandler> const& s1, std::shared_ptr<SignatureHandler> const& s2)->bool {
				return s1->selfIndex < s2->selfIndex;
				});
		}
		
	}
	Json::Value m_event;
};

