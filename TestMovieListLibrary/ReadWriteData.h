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
#include <filesystem>


extern double c_2;
extern double c_3;
extern size_t partSize;


class VideoFileInfo {
public:
	VideoFileInfo(std::string fullPathToFile, std::string modifiedDate, size_t fileSize, size_t selfIndex) : fullPathToFile{ fullPathToFile }, modifiedDate{ modifiedDate }, fileSize{ fileSize }, selfIndex{ selfIndex }{}
	VideoFileInfo() {}
	std::string fullPathToFile;
	std::string modifiedDate;
	size_t fileSize;
	size_t selfIndex;
};


struct ParametrsFromDataBase {

	ParametrsFromDataBase(double C_2, double C_3, size_t partSize) : base_c_2{ C_2 }, base_c_3{ C_3 }, basePartSize{ partSize }{}
	const double base_c_2;
	const double base_c_3;
	const size_t basePartSize;
};

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

	void writeSignatureHandlerToFile(std::shared_ptr<SignatureHandler> signatureHandler, size_t signatureHandlerIndex) {

		std::ofstream fileId;
		fileId.open(dataFileName);

		Json::Value signatureVecString(Json::arrayValue);
		Json::Value event;

		std::string sigHandlerName = "signatureHandler_" + std::to_string(signatureHandlerIndex);
		auto signatureVec = signatureHandler->getSignatures();
		for (auto it : signatureVec) {
			CString signature;
			it->saveToString(signature);
			CT2CA pszConvertedAnsiString(signature);
			std::string signString(pszConvertedAnsiString);
			signatureVecString.append(Json::Value(signString));
		}
		event[ReadWriteData::baseName][sigHandlerName]["selfIndex"] = signatureHandler->selfIndex;
		event[ReadWriteData::baseName][sigHandlerName]["signatures"] = signatureVecString;
		auto videoPartVec = signatureHandler->getVideoParts();
		size_t partIndex = 0;
		for (auto singleVideoPart : videoPartVec) {
			auto videoPartName = "videoPart_" + std::to_string(partIndex);
			event[ReadWriteData::baseName][sigHandlerName][videoPartName][VideoPartNames::mainSignatureIndex] = singleVideoPart->mainSignatureIndex;
			event[ReadWriteData::baseName][sigHandlerName][videoPartName][VideoPartNames::lastSignatureIndex] = singleVideoPart->lastSignatureIndex;
			event[ReadWriteData::baseName][sigHandlerName][videoPartName][VideoPartNames::selfIndex] = singleVideoPart->selfIndex;
			partIndex++;
		}

		fileId << event;

		fileId.close();
	}

	void writeAllDataToFile(std::vector<std::shared_ptr<SignatureHandler>> signatureHandlerVec, std::vector<std::shared_ptr<VideoFileInfo>> fileInfoVec) {
		std::ofstream fileId;
		auto completeFileName = dataFileName + std::to_string(fileCounting) + ".json";
		fileId.open(dataFileName);

		Json::Value event;

		event[ReadWriteData::baseName]["parameters"]["c_2"] = c_2;
		event[ReadWriteData::baseName]["parameters"]["c_3"] = c_3;
		event[ReadWriteData::baseName]["parameters"]["partSize"] = partSize;

		size_t index = 0;
		for (auto signatureHandler : signatureHandlerVec) {
			Json::Value signatureVecString(Json::arrayValue);
			std::string sigHandlerName = "signatureHandler_" + std::to_string(signatureHandler->selfIndex);
			auto signatureVec = signatureHandler->getSignatures();
			for (auto it : signatureVec) {
				CString signature;
				it->saveToString(signature);
				CT2CA pszConvertedAnsiString(signature);
				std::string signString(pszConvertedAnsiString);
				signatureVecString.append(Json::Value(signString));
			}

			event[ReadWriteData::baseName][sigHandlerName]["selfIndex"] = signatureHandler->selfIndex;
			event[ReadWriteData::baseName][sigHandlerName]["fullPathToFile"] = fileInfoVec[index]->fullPathToFile;
			event[ReadWriteData::baseName][sigHandlerName]["modifiedDate"] = fileInfoVec[index]->modifiedDate;
			event[ReadWriteData::baseName][sigHandlerName]["fileSize"] = fileInfoVec[index]->fileSize;
			event[ReadWriteData::baseName][sigHandlerName]["signatureAmount"] = signatureVec.size();
			event[ReadWriteData::baseName][sigHandlerName]["signatures"] = signatureVecString;
			auto videoPartVec = signatureHandler->getVideoParts();
			size_t partIndex = 0;
			for (auto singleVideoPart : videoPartVec) {
				auto videoPartName = "videoPart_" + std::to_string(partIndex);
				event[ReadWriteData::baseName][sigHandlerName][videoPartName][VideoPartNames::firstSignatureIndex] = singleVideoPart->firstSignatureIndex;
				event[ReadWriteData::baseName][sigHandlerName][videoPartName][VideoPartNames::mainSignatureIndex] = singleVideoPart->mainSignatureIndex;
				event[ReadWriteData::baseName][sigHandlerName][videoPartName][VideoPartNames::lastSignatureIndex] = singleVideoPart->lastSignatureIndex;
				event[ReadWriteData::baseName][sigHandlerName][videoPartName][VideoPartNames::selfIndex] = singleVideoPart->selfIndex;
				partIndex++;
			}
			index++;
		}
		fileId << event;

		fileId.close();
		fileCounting++;
	}

	std::pair<std::vector<std::shared_ptr<SignatureHandler>>,std::vector<std::shared_ptr<VideoFileInfo>>> readAllDataFromFile() {
		Json::Value root;   // will contain the root value after parsing.
		Json::CharReaderBuilder builder;
		
		auto currentFolderPath = std::filesystem::current_path();

		auto pathToDir = currentFolderPath.string();
		pathToDir += "/Data";

		for (const auto& file : std::filesystem::directory_iterator(pathToDir)) {

			std::ifstream ifStream(file.path().string(), std::ifstream::binary);
			if (!ifStream.is_open()) {
				return {};
			}
			std::string errs;
			bool ok = Json::parseFromStream(builder, ifStream, &root, &errs);
			if (!ok)
			{
				std::cout << errs << "\n";
				return {};
			}

			std::vector<std::shared_ptr<SignatureHandler>> signatureHandlerVec;
			std::vector<std::shared_ptr<VideoFileInfo>> fileInfoVec;

			size_t handlerIndex = 0;
			auto base_c_2 = root[ReadWriteData::baseName]["parameters"]["c_2"].asDouble();
			auto base_c_3 = root[ReadWriteData::baseName]["parameters"]["c_3"].asDouble();
			auto base_partSize = root[ReadWriteData::baseName]["parameters"]["partSize"].asUInt();
			parametrs = std::make_shared<ParametrsFromDataBase>(base_c_2, base_c_3, base_partSize);
			while (1) {
				auto signatureHandlerName = "signatureHandler_" + std::to_string(handlerIndex);
				auto signaturesJsonArr = root[ReadWriteData::baseName][signatureHandlerName]["signatures"];
				if (signaturesJsonArr == NULL) {
					break;
				}
				auto newSignatureHandler = std::make_shared<SignatureHandler>();
				auto newFileInfo = std::make_shared<VideoFileInfo>();
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

				newFileInfo->fullPathToFile = root[ReadWriteData::baseName][signatureHandlerName]["fullPathToFile"].asCString();
				newFileInfo->modifiedDate = root[ReadWriteData::baseName][signatureHandlerName]["modifiedDate"].asCString();
				newFileInfo->fileSize = root[ReadWriteData::baseName][signatureHandlerName]["fileSize"].asUInt();
				newFileInfo->selfIndex = newSignatureHandler->selfIndex;

				size_t partIndex = 0;
				std::vector<std::shared_ptr<VideoPart>> videoPartsVec;
				while (1) {
					auto videoPartName = "videoPart_" + std::to_string(partIndex);
					auto lastSignatureIndex = root[ReadWriteData::baseName][signatureHandlerName][videoPartName][VideoPartNames::lastSignatureIndex].asUInt();
					auto mainSignatureIndex = root[ReadWriteData::baseName][signatureHandlerName][videoPartName][VideoPartNames::mainSignatureIndex].asUInt();
					auto firstSignatureIndex = root[ReadWriteData::baseName][signatureHandlerName][videoPartName][VideoPartNames::firstSignatureIndex].asUInt();
					auto selfIndex = root[ReadWriteData::baseName][signatureHandlerName][videoPartName][VideoPartNames::selfIndex].asUInt();

					if (lastSignatureIndex == 0 && selfIndex == 0 && mainSignatureIndex == 0) {
						if (videoPartsVec.empty()) {
							return {};
						}
						break;
					}
					auto newVideoPartPtr = std::make_shared<VideoPart>(firstSignatureIndex, mainSignatureIndex, lastSignatureIndex, selfIndex);
					videoPartsVec.push_back(newVideoPartPtr);
					partIndex++;
				}
				if (videoPartsVec.empty()) {
					std::cout << "Some error in reading file" << std::endl;
					return {};
				}
				newSignatureHandler->addVideoParts(videoPartsVec);
				signatureHandlerVec.push_back(newSignatureHandler);
				fileInfoVec.push_back(newFileInfo);
				handlerIndex++;
			}
			if (!signatureHandlerVec.empty()) {
				sortData(signatureHandlerVec, fileInfoVec);
			}
			return std::make_pair(signatureHandlerVec, fileInfoVec);
		}

		return {};

	}

	std::shared_ptr<ParametrsFromDataBase> getParametrs() const {
		return parametrs;
	}

	private:
	 void sortData(std::vector<std::shared_ptr<SignatureHandler>> &signatureHandlerVec, std::vector<std::shared_ptr<VideoFileInfo>> &fileinfoVec) {
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
		if (!fileinfoVec.empty()) {
			std::sort(fileinfoVec.begin(), fileinfoVec.end(), [](std::shared_ptr<VideoFileInfo> const& s1, std::shared_ptr<VideoFileInfo> const& s2)->bool {
				return s1->selfIndex < s2->selfIndex;
				});
		}
	}
	size_t fileCounting = 0;
	std::shared_ptr<ParametrsFromDataBase> parametrs;
};

