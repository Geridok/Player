#pragma once
#include "WorkWithFrames.h"
#include <vector>

extern std::vector<std::chrono::microseconds> compare;

class SignatureComparator
{
public:
	static std::vector<double> compareSignatures(WorkWithSignatures* signatureHandler_1, WorkWithSignatures* signatureHandler_2) {
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

};
