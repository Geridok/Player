#pragma once
#include "WorkWithFrames.h"
#include "AdditionalStructures.h"
#include "DataStorage.h"
#include <list>
class SearchAlgoritm
{
public:
	SearchAlgoritm(double c_2) :c_2{ c_2 } {
		dataStorage = DataStorage::getInstance();
		signatureHandlerStorageVec = dataStorage->getDataBase();
	}


public:
	//Do not use videoPart in search
	std::vector<SearchResult> startLinearSearch(std::shared_ptr<SignatureHandler> signatureHandler) {
		auto signaturesVec = signatureHandler->getSignatures();
		for (auto singleSignature : signaturesVec) {
			auto result = linearSearchInBase(singleSignature);
		}
	}

	//Use VideoPart in Search
	std::vector<SearchResult> VPSearchInBase(std::shared_ptr<SignatureHandler> signatureHandler) {
		auto signaturesVec = signatureHandler->getSignatures();
		for (auto singleSignature : signaturesVec) {
			auto result = linearSearchInBase(singleSignature);
			for (auto receivedSearchInfo : result) {

			}
		}
	}

private:
	std::list<SearchInfo> linearSearchInBase(CSignature* streamSignature) {

		std::list<SearchInfo> resultVector;
		size_t signaturehandlerIndex = 0;
		for (auto signatureHandler : signatureHandlerStorageVec) {
			auto signatures = signatureHandler->getSignatures();
			size_t signatureIndex = 0;
			for (auto singleSignature : signatures) {
				if (streamSignature->difference(*singleSignature) <= c_2) {
					resultVector.push_back(SearchInfo(SearchWindow(signatureIndex - 1,signatureIndex + 1), signatureHandler->selfIndex,signatureIndex));
				}
				signatureIndex++;
			}
			signaturehandlerIndex++;
		}
		return resultVector;
	}

	std::list<SearchInfo> VPSearchInBase(CSignature* streamSignature, double compareThreshold) {
		std::list<SearchInfo> resultVector;

		for (auto signatureHandler : signatureHandlerStorageVec) {
			auto signaturesVec = signatureHandler->getSignatures();
			auto videoPartVec = signatureHandler->getVideoParts();
			for (auto singleVideoPart : videoPartVec) {
				if (streamSignature->difference(*signaturesVec[singleVideoPart->mainSignatureIndex]) <= compareThreshold) {
					resultVector.push_back(SearchInfo(signatureHandler->selfIndex, singleVideoPart->selfIndex));
				}
			}
		}
		for (auto it : searchInfoList) {
			auto counter = std::find_if(resultVector.begin(), resultVector.end(), [&it](const SearchInfo* f)->bool
				{ return (it.signatureHandlerIndex == f->signatureHandlerIndex && it.videoPartIndex == f->videoPartIndex); });
				if (counter != resultVector.end()) {
					resultVector.erase(counter);
				}
		}
		return resultVector;

	}
	bool defineSearchWindow(SearchInfo& currentSearchInfo, CSignature* streamSignature) {
		auto signatureVec = signatureHandlerStorageVec[currentSearchInfo.signatureHandlerIndex]->getSignatures();
		auto videoPartVec = signatureHandlerStorageVec[currentSearchInfo.signatureHandlerIndex]->getVideoParts();
		size_t firstSigIndex = videoPartVec[currentSearchInfo.videoPartIndex]->mainSignatureIndex;
		if (streamSignature->difference(*signatureVec[firstSigIndex]) <= c_2) {
			int currentSignutureIndex = firstSigIndex;
			while (currentSignutureIndex - 1 >= 0) {
				if (streamSignature->difference(*signatureVec[currentSignutureIndex - 1]) > c_2) {
					break;
				}
				currentSignutureIndex--;
			}
			currentSearchInfo.searchWindow.leftIndex = currentSignutureIndex;
		}
		else {
			int currentSignutureIndex = firstSigIndex;
			size_t lastIndexInVideoPart = videoPartVec[currentSearchInfo.videoPartIndex]->lastSignatureIndex;
			while (currentSignutureIndex + 1 <= lastIndexInVideoPart) {
				if (streamSignature->difference(*signatureVec[currentSignutureIndex + 1]) <= c_2) {
					currentSignutureIndex++;
					break;
				}
				currentSignutureIndex++;
			}
			if (currentSignutureIndex == lastIndexInVideoPart) {
				return false;
			}
			currentSearchInfo.searchWindow.leftIndex = currentSignutureIndex;
		}
		currentSearchInfo.currentSignatureIndex = currentSearchInfo.searchWindow.leftIndex + 1;
		size_t lastsignatureIndex = videoPartVec[currentSearchInfo.videoPartIndex]->lastSignatureIndex;
		if (streamSignature->difference(*signatureVec[lastsignatureIndex]) <= c_2) {
			int currentSignutureIndex = lastsignatureIndex;
			while (currentSignutureIndex + 1 <= signatureVec.size() - 1) {
				if (streamSignature->difference(*signatureVec[currentSignutureIndex + 1]) > c_2) {
					break;
				}
				currentSignutureIndex++;
			}
			currentSearchInfo.searchWindow.rightIndex = currentSignutureIndex;
			if (currentSearchInfo.searchWindow.rightIndex - currentSearchInfo.searchWindow.leftIndex <= 1) {
				if (currentSearchInfo.searchWindow.rightIndex + 1 <= signatureVec.size() - 1) {
					currentSearchInfo.searchWindow.rightIndex++;
				}
			}
		}
		else {
			int currentSignutureIndex = lastsignatureIndex;
			size_t edgeIndex = currentSearchInfo.searchWindow.leftIndex;
			while (currentSignutureIndex - 1 >= edgeIndex) {
				if (streamSignature->difference(*signatureVec[currentSignutureIndex - 1]) <= c_2) {
					currentSignutureIndex--;
					break;
				}
				currentSignutureIndex--;
			} 
			currentSearchInfo.searchWindow.rightIndex = currentSignutureIndex;
			if (currentSignutureIndex == edgeIndex) {
				currentSearchInfo.currentSignatureIndex = currentSignutureIndex;
				currentSearchInfo.searchWindow.leftIndex = currentSignutureIndex - 1;
				currentSearchInfo.searchWindow.rightIndex = currentSignutureIndex - 1;
			}
		}
		return true;
	}
private:
	DataStorage* dataStorage;
	std::vector<std::shared_ptr<SignatureHandler>> signatureHandlerStorageVec;
	std::list<SearchInfo> searchInfoList;
	double c_2;
};

