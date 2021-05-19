#pragma once
#include "WorkWithFrames.h"
#include <cstdio>
#include <json/value.h>
#include "ReadWriteData.h"


class DataStorage
{
private:

	static DataStorage* instance;
	DataStorage() {
		m_dataWorker = ReadWriteData::getInstance();
	}

public:
	static DataStorage* getInstance() {
		if (instance == 0) {
			instance = new DataStorage();
		}
		return instance;
	}

public:
	bool loadDataFromFile(std::string fileName) {
		auto pair = m_dataWorker->readAllDataFromFile(fileName);
		if (!pair.first.empty() && !pair.second.empty()) {
			videoVec = pair.first;
			fileInfoVec = pair.second;
			return true;
		}
		return false;
	}
	const std::vector<std::shared_ptr<SignatureHandler>> getDataBase() const {
		if (videoVec.empty()) {
			return {};
		}
		return videoVec;
	}
	void addSignatureHandler(std::shared_ptr<SignatureHandler> signatureHandler, std::shared_ptr<VideoFileInfo> fileInfo) {
		if (signatureHandler->getSigAmount() != 0 && !signatureHandler->getVideoParts().empty()) {
			signatureHandler->selfIndex = signatureHandlerIndex;
			videoVec.push_back(signatureHandler);
			fileInfoVec.push_back(fileInfo);
			signatureHandlerIndex++;
		}
	}

	bool writeToDisk(std::string fileName) const {
		if (!videoVec.empty()) {
			m_dataWorker->writeAllDataToFile(videoVec,fileInfoVec,fileName);
			return true;
		}
		return false;
	}
	std::shared_ptr<ParametrsFromDataBase> getParametrsFromDataBase() {
		if (!videoVec.empty()) {
			return m_dataWorker->getParametrs();
		}
		return {};
	}

private:
	std::vector<std::shared_ptr<SignatureHandler>> videoVec;
	std::vector<std::shared_ptr<VideoFileInfo>> fileInfoVec;
	ReadWriteData *m_dataWorker;
	size_t maxSignatureAmount = 571000;
	size_t currentSignatureAmount = 0;
	size_t signatureHandlerIndex = 0;
};

