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


class ReadWriteData
{
public:
	static std::string dataFileName;
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
	static void writeDataToFile(std::vector<std::shared_ptr<SignatureHandler>> signatureHandlerVec) {

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
			event[sigHandlerName]["signatures"] = signatureVecString;
			auto videoPartVec = singleSignatureHandler->getVideoParts();
			size_t index = 0;
			for (auto singleVideoPart : videoPartVec) {
				std::string videoPartName = "videoPart+" + std::to_string(index);
				event[sigHandlerName][videoPartName][VideoPartNames::mainSignatureIndex] = singleVideoPart->mainSignatureIndex;
				event[sigHandlerName][videoPartName][VideoPartNames::lastSignatureIndex] = singleVideoPart->lastSignatureIndex;
				event[sigHandlerName][videoPartName][VideoPartNames::selfIndex] = singleVideoPart->selfIndex;
				index++;
			}
		}


		fileId << event;

		fileId.close();

	}
	static void test() {
		std::ofstream fileId;
		fileId.open("Data/test.json", std::ios::out | std::ios::app);

		Json::Value event;
		std::string sigHandlerName = "signatureHandler_" + std::to_string(0);
		for (int i = 0; i < 100;i++) {
			auto videoPartName = std::to_string(i);
			event[sigHandlerName][videoPartName][VideoPartNames::mainSignatureIndex] = 9;
			event[sigHandlerName][videoPartName][VideoPartNames::lastSignatureIndex] = 3;
			event[sigHandlerName][videoPartName][VideoPartNames::selfIndex] = 32;
		}
		fileId << event;

		fileId.close();
	}


	static void writeSignatureHandlerToFile(std::shared_ptr<SignatureHandler> signatureHandler, size_t signatureHandlerIndex) {

		std::ofstream fileId;
		fileId.open(dataFileName, std::ios::out | std::ios::app);

		Json::Value event;
		Json::Value signatureVecString(Json::arrayValue);

		std::string sigHandlerName = "signatureHandler_" + std::to_string(signatureHandlerIndex);
		auto signatureVec = signatureHandler->getSignatures();
		for (auto it : signatureVec) {
			CString signature;
			it->saveToString(signature);
			char* c_signature = new char[it->getSize()];
			std::string signString(c_signature);
			delete[]c_signature;
			signatureVecString.append(Json::Value(signString));
		}
		event[sigHandlerName]["signatures"] = signatureVecString;
		auto videoPartVec = signatureHandler->getVideoParts();
		size_t partIndex = 0;
		for (auto singleVideoPart : videoPartVec) {
			auto videoPartName = "videoPart_" + std::to_string(partIndex);
			event[sigHandlerName][videoPartName][VideoPartNames::mainSignatureIndex] = singleVideoPart->mainSignatureIndex;
			event[sigHandlerName][videoPartName][VideoPartNames::lastSignatureIndex] = singleVideoPart->lastSignatureIndex;
			event[sigHandlerName][videoPartName][VideoPartNames::selfIndex] = singleVideoPart->selfIndex;
			partIndex++;
		}

		fileId << event;

		fileId.close();
	}
	static std::vector<std::shared_ptr<SignatureHandler>> readAllDataFromFile() {
		std::ifstream fileId;
		fileId.open(dataFileName, std::ifstream::binary);
		if (!fileId.is_open()) {
			return{};
		}

		Json::Value data;
		Json::Reader reader;

		fileId >> data;

		if (!reader.parse(fileId, data, true)) {
			//for some reason it always fails to parse
			std::cout << "Failed to parse configuration\n"
				<< reader.getFormattedErrorMessages();
			return {};
		}
		std::vector<std::shared_ptr<SignatureHandler>> signatureHandlerVec;
		size_t handlerIndex = 0;
		while (1) {
			auto signatureHandlerName = "signatureHandler_" + handlerIndex;
			auto signaturesJsonArr = data[signatureHandlerName]["signatures"];
			if (signaturesJsonArr == NULL) {
				break;
			}
			auto newSignatureHandler = std::make_shared<SignatureHandler>();
			std::vector<CSignature*> signatureVec;
			for (auto signature : signaturesJsonArr) {
				CSignature* newSignature = new CSignature();
				std::string sig = signature.asString();
				newSignature->loadFromString(CString(sig.c_str()));
				signatureVec.push_back(newSignature);
			}
			if (signatureVec.empty()) {
				std::cout << "Can't read Data from file" << std::endl;
				return {};
			}
			newSignatureHandler->addSignature(signatureVec);
			size_t partIndex = 0;
			std::vector<std::shared_ptr<VideoPart>> videoPartsVec;
			while (1) {
				auto videoPartName = "videoPart_" + std::to_string(partIndex);
				auto singleVideoPart = data[signatureHandlerName][videoPartName];
				if (singleVideoPart == NULL) {
					break;
				}
				auto mainSignatureIndex = singleVideoPart[VideoPartNames::mainSignatureIndex].asUInt64();
				auto lastSignatureIndex = singleVideoPart[VideoPartNames::lastSignatureIndex].asUInt64();
				auto selfIndex = singleVideoPart[VideoPartNames::selfIndex].asUInt64();
				if (mainSignatureIndex == NULL || lastSignatureIndex == NULL || selfIndex == NULL) {
					std::cout << "Can't read Data from file" << std::endl;
					return {};
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
		return signatureHandlerVec;

	}
};

