#pragma once
#include "SignatureHandler.h"
#include "AdditionalStructures.h"
#include "DataStorage.h"
#include <list>

extern double c_3;
extern double c_2;



class SearchAlgoritm
{
public:
	SearchAlgoritm(){
		dataStorage = DataStorage::getInstance();
		signatureHandlerStorageVec = dataStorage->getDataBase();
	}


public:
	//Use VideoPart in Search
	std::vector<SearchResult> VPSearchInBase(std::shared_ptr<SignatureHandler> signatureHandler) {
		auto signaturesVec = signatureHandler->getSignatures();
		for (auto singleSignature : signaturesVec) {
			auto result = SearchInBase(singleSignature);
			for (auto receivedSearchInfo = result.begin(); receivedSearchInfo != result.end(); receivedSearchInfo++) {
				if (!defineSearchWindow(*receivedSearchInfo, singleSignature)) {
					result.erase(receivedSearchInfo);
				}
			}
			for (auto selfSearchInfo = searchInfoList.begin(); selfSearchInfo != searchInfoList.end(); selfSearchInfo++) {
				if (!workWithSearchInfo(*selfSearchInfo, singleSignature)) {
					searchInfoList.erase(selfSearchInfo);
				}
			}

			for (auto receivedSearchInfo : result) {
				signatureHandlerStorageVec[receivedSearchInfo.signatureHandlerIndex]->getVideoParts()[receivedSearchInfo.videoPartIndex]->isActive = true;
				searchInfoList.emplace_back(receivedSearchInfo);
			}
		}
	}

private:
	std::list<SearchInfo> SearchInBase(CSignature* streamSignature) {
		std::list<SearchInfo> resultVector;

		for (auto signatureHandler : signatureHandlerStorageVec) {
			auto signaturesVec = signatureHandler->getSignatures();
			auto videoPartVec = signatureHandler->getVideoParts();
			for (auto singleVideoPart : videoPartVec) {
				if (!videoPartVec[singleVideoPart->isActive]) {
					if (streamSignature->difference(*signaturesVec[singleVideoPart->mainSignatureIndex]) <= c_3) {
						resultVector.push_back(SearchInfo(signatureHandler->selfIndex, singleVideoPart->selfIndex));
					}
				}
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
				currentSearchInfo.searchWindow.rightIndex = currentSignutureIndex + 1;
			}
		}
		return true;
	}
	bool workWithSearchInfo(SearchInfo& currentSearchInfo, CSignature* streamSignature) {
		auto signatureVec = signatureHandlerStorageVec[currentSearchInfo.signatureHandlerIndex]->getSignatures();

		if (streamSignature->difference(*signatureVec[currentSearchInfo.currentSignatureIndex + 1]) <= c_2) {
			currentSearchInfo.currentSignatureIndex++;
			return true;
		}
		else {
			if (!checkSignatureInWindow(currentSearchInfo, streamSignature)) {
				signatureHandlerStorageVec[currentSearchInfo.signatureHandlerIndex]->getVideoParts()[currentSearchInfo.videoPartIndex]->isActive = false;
				return false;
			}else {
				if (currentSearchInfo.searchWindow.rightIndex == currentSearchInfo.currentSignatureIndex) {
					signatureHandlerStorageVec[currentSearchInfo.signatureHandlerIndex]->getVideoParts()[currentSearchInfo.videoPartIndex]->isActive = false;
					return false;
				}
			}
			return true;;
		}

	}

	bool checkSignatureInWindow(SearchInfo& currentSearchInfo, CSignature* streamSignature) {
		auto signatureVec = signatureHandlerStorageVec[currentSearchInfo.signatureHandlerIndex]->getSignatures();

		if (!(streamSignature->difference(*signatureVec[currentSearchInfo.searchWindow.leftIndex]) <= c_2)) {
			auto lastIndex = currentSearchInfo.searchWindow.rightIndex;
			auto currentIndex = currentSearchInfo.searchWindow.leftIndex + 1;
			bool succesful = false;
			while (currentIndex + 1 <= lastIndex) {
				if (streamSignature->difference(*signatureVec[currentIndex + 1]) <= c_2) {
					currentIndex++;
					succesful = true;
					break;
				}
				currentIndex++;
			}
			if (!succesful) {
				return false;
			}
			currentSearchInfo.searchWindow.leftIndex = currentIndex;
			if (currentSearchInfo.currentSignatureIndex < currentSearchInfo.searchWindow.leftIndex) {
				currentSearchInfo.currentSignatureIndex = currentIndex;
			}
			else {
				currentSearchInfo.currentSignatureIndex++;
			}
			if (currentIndex == lastIndex) {
				if (lastIndex + 1 <= signatureVec.size() - 1) {
					currentSearchInfo.searchWindow.rightIndex = currentIndex + 1;
				}
				else {
					currentSearchInfo.searchWindow.rightIndex = currentIndex;
				}
				currentSearchInfo.searchWindow.leftIndex = currentIndex - 1;
				currentSearchInfo.currentSignatureIndex = currentIndex;
			}

		}else { currentSearchInfo.currentSignatureIndex++; }

		if (!(streamSignature->difference(*signatureVec[currentSearchInfo.searchWindow.rightIndex]) <= c_2)) {

			auto currentIndex = currentSearchInfo.searchWindow.rightIndex;
			bool succesful = false;
			while (currentIndex - 1 >= currentSearchInfo.searchWindow.leftIndex) {
				if (streamSignature->difference(*signatureVec[currentIndex - 1]) <= c_2) {
					succesful = true;
					currentIndex--;
					break;
				}
				currentIndex--;
			}
			currentSearchInfo.searchWindow.rightIndex = currentIndex;
			if (!succesful) {
				if (currentIndex - 1 < 0) {
					currentSearchInfo.searchWindow.leftIndex = currentIndex;
				}
				else {
					currentSearchInfo.searchWindow.leftIndex = currentIndex - 1;
				}
				currentSearchInfo.searchWindow.rightIndex = currentIndex + 1;
				currentSearchInfo.currentSignatureIndex = currentIndex;
			}
			
		}else {
			if (currentSearchInfo.searchWindow.rightIndex - currentSearchInfo.searchWindow.leftIndex <= 1) {
				if (currentSearchInfo.searchWindow.rightIndex + 1 <= signatureVec.size() - 1) {
					currentSearchInfo.searchWindow.rightIndex++;
				}
			}
		}
		return true;

	}
private:
	DataStorage* dataStorage;
	std::vector<std::shared_ptr<SignatureHandler>> signatureHandlerStorageVec;
	std::list<SearchInfo> searchInfoList;
};

