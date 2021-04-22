#pragma once
#include "WorkWithFrames.h"
#include <cstdio>
#include <json/value.h>
#include "ReadWriteData.h"

class DataInformation
{
public:
	bool addNewVideoToDataBase(std::shared_ptr<SignatureHandler> signatureHandler) {
		if (signatureHandler->getSigAmount() != 0 && !signatureHandler->getVideoParts().empty()) {
			ReadWriteData::writeSignatureHandlerToFile(signatureHandler, videos.size());
			videos.push_back(std::move(signatureHandler));
			return true;
		}
		return false;
	}
	bool loadDataFromFile() {
		auto videoFromFile = ReadWriteData::readAllDataFromFile();
		if (!videoFromFile.empty()) {
			videos = videoFromFile;
			return true;
		}
		return false;
	}
private:
	std::vector<std::shared_ptr<SignatureHandler>> videos;
};

