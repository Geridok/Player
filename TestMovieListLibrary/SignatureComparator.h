#pragma once
#include "WorkWithFrames.h"
#include <vector>
class SignatureComparator
{
	static std::vector<double> compareSignatures(WorkWithSignatures* signatureHandler_1, WorkWithSignatures* signatureHandler_2,size_t start = 1) {
		if (signatureHandler_1 == nullptr || signatureHandler_2 == nullptr) { return std::vector<double>{}; }
		std::vector<double> diffVec;
		size_t nextShot = start;
		double amount = 0;
		auto sigVec_1 = signatureHandler_1->getSignatures();
		auto sigVec_2 = signatureHandler_2->getSignatures();
		amount = sigVec_1.size();

		amount = (amount < sigVec_2.size()) ? amount : sigVec_2.size();

		for (size_t i = 0; i < amount; i++)
		{
			double diff = sigVec_1[i]->difference(*sigVec_2[nextShot]);
			diffVec.push_back(diff);
			nextShot++;
		}

		return diffVec;
	}

};
