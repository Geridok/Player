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
	bool addNewVideoToDataBase(std::shared_ptr<SignatureHandler> signatureHandler) {
		if (signatureHandler->getSigAmount() != 0 && !signatureHandler->getVideoParts().empty()) {
			signatureHandler->selfIndex = videos.size();
			m_dataWorker->writeSignatureHandlerToFile(signatureHandler, videos.size());
			videos.push_back(std::move(signatureHandler));
			return true;
		}
		return false;
	}
	bool loadDataFromFile() {
		auto videoFromFile = m_dataWorker->readAllDataFromFile();
		if (!videoFromFile.empty()) {
			videos = videoFromFile;
			return true;
		}
		return false;
	}
	const std::vector<std::shared_ptr<SignatureHandler>> getDataBase() const {
		if (videos.empty()) {
			return {};
		}
		return videos;
	}
private:
	std::vector<std::shared_ptr<SignatureHandler>> videos;
	ReadWriteData *m_dataWorker;
};

