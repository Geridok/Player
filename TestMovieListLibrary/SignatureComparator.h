#pragma once
#include "WorkWithFrames.h"
#include <vector>
#include <thread>

extern std::vector<std::chrono::microseconds> compare;

class SignatureComparator
{
public:
	static std::vector<double> compareSignatures(SignatureHandler* signatureHandler_1, SignatureHandler* signatureHandler_2) {
		if (signatureHandler_1 == nullptr || signatureHandler_2 == nullptr) { return std::vector<double>{}; }
		std::vector<double> diffVec;

		auto sigVec_1 = signatureHandler_1->getSignatures();
		auto sigVec_2 = signatureHandler_2->getSignatures();
		auto amount = sigVec_1.size();

		amount = (amount < sigVec_2.size()) ? amount : sigVec_2.size();

		for (size_t i = 0; i < amount; i++)
		{
			auto start = std::chrono::high_resolution_clock::now();
			double diff = sigVec_1[i]->difference(*sigVec_2[i]);
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
			compare.push_back(duration);
			diffVec.push_back(diff);
		}

		return diffVec;
	}

	std::pair<std::pair<uint64_t,uint64_t>,std::pair<uint64_t,uint64_t>> comparePair(SignatureHandler* stream, uint64_t streamIndex, SignatureHandler* videoClip, uint64_t videoClipIndex, double threshold) {
		if (stream == nullptr || videoClip == nullptr) { return std::make_pair(std::make_pair(0.0,0.0), std::make_pair(0.0, 0.0)); }
		if (streamIndex >= stream->getSigAmount() || videoClipIndex >= videoClip->getSigAmount()) { return std::make_pair(std::make_pair(0.0, 0.0), std::make_pair(0.0, 0.0)); }
		if (threshold < 0.0 || threshold > 1.0) { return std::make_pair(std::make_pair(0.0, 0.0), std::make_pair(0.0, 0.0)); }
		double diff = stream->getSignatures()[streamIndex]->difference(*(videoClip->getSignatures()[videoClipIndex]));
		if (diff > threshold) { return std::make_pair(std::make_pair(0.0, 0.0), std::make_pair(0.0, 0.0)); }
		compareRight(stream, streamIndex, videoClip, videoClipIndex, threshold);
		compareLeft(stream, streamIndex, videoClip, videoClipIndex, threshold);
		return std::make_pair(leftCompare.second, rightCompare.second);
	}

private: 

	void compareRight(SignatureHandler* stream,uint64_t streamIndex, SignatureHandler* videoClip,uint64_t videoClipIndex, double threshold) {
		uint64_t currentStreamIndex = streamIndex + 1;
		uint64_t currentVideoClipIndex = videoClipIndex + 1;
		uint64_t streamSize = stream->getSigAmount();
		uint64_t videoClipSize = videoClip->getSigAmount();
		auto sigVecStream = stream->getSignatures();
		auto sigVecVideoClip = videoClip->getSignatures();
		std::vector<double> diffVec;
		if (currentStreamIndex >= streamSize || currentVideoClipIndex >= videoClipSize) { rightCompare = std::make_pair(std::vector<double> {}, std::make_pair(0.0, 0.0)); return; }
		double diff = 0;
		do {
			auto start = std::chrono::high_resolution_clock::now();
			diff = sigVecStream[currentStreamIndex]->difference(*sigVecVideoClip[currentVideoClipIndex]);
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
			compare.push_back(duration);
			if (diff >= threshold) {
				break;
			}
			diffVec.push_back(diff);
			currentStreamIndex += 1;
			currentVideoClipIndex += 1;
		} while (currentStreamIndex < streamSize && currentVideoClipIndex < videoClipSize);
		rightCompare = std::make_pair(diffVec, std::make_pair(streamIndex, currentStreamIndex));
	}

	void compareLeft(SignatureHandler* stream, uint64_t streamIndex, SignatureHandler* videoClip, uint64_t videoClipIndex, double threshold) {
		uint64_t currentStreamIndex = streamIndex - 1;
		uint64_t currentVideoClipIndex = videoClipIndex - 1;
		uint64_t streamSize = stream->getSigAmount();
		uint64_t videoClipSize = videoClip->getSigAmount();
		auto sigVecStream = stream->getSignatures();
		auto sigVecVideoClip = videoClip->getSignatures();
		std::vector<double> diffVec;
		if (currentStreamIndex >= streamSize || currentVideoClipIndex >= videoClipSize) { leftCompare = std::make_pair(std::vector<double> {}, std::make_pair(0.0, 0.0)); return; }
		double diff = 0;
		do {
			auto start = std::chrono::high_resolution_clock::now();
			diff = sigVecStream[currentStreamIndex]->difference(*sigVecVideoClip[currentVideoClipIndex]);
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
			compare.push_back(duration);
			if (diff >= threshold) {
				break;
			}
			diffVec.push_back(diff);
			currentStreamIndex -= 1;
			currentVideoClipIndex -= 1;
		} while (currentStreamIndex >= 0 && currentVideoClipIndex >= 0);
		leftCompare = std::make_pair(diffVec, std::make_pair(currentStreamIndex, streamIndex));
	}

	std::pair<std::vector<double>, std::pair<uint64_t, uint64_t>> rightCompare;
	std::pair<std::vector<double>, std::pair<uint64_t, uint64_t>> leftCompare;
};
