#pragma once
#include "VideoPart.h"
#include <stdio.h>
#include "WorkWithFrames.h"
struct SearchWindow {
	SearchWindow() {
		leftIndex = 0;
		rightIndex = 0;
	}
	SearchWindow(int leftIndex, int rightIndex) {
		if (leftIndex < 0) {
			this->leftIndex = 0;
		}
		else {
			this->leftIndex = leftIndex;
		}
		if (rightIndex <+ leftIndex) {
			ATLASSERT("Index error");
		}
		this->rightIndex = rightIndex;
	}
	size_t leftIndex;
	size_t rightIndex;
};

struct SearchInfo {
	SearchInfo(SearchWindow searchWindow, size_t signatureHandlerIndex, size_t currentSignatureIndex) :searchWindow{ searchWindow }, signatureHandlerIndex{ signatureHandlerIndex }, currentSignatureIndex{ currentSignatureIndex }{}
	SearchInfo(SearchWindow searchWindow, size_t signatureHandlerIndex, size_t videoPartIndex, size_t currentSignatureIndex) :searchWindow{ searchWindow }, signatureHandlerIndex{ signatureHandlerIndex }, videoPartIndex{ videoPartIndex }, currentSignatureIndex{ currentSignatureIndex }{}
	SearchInfo(size_t signatureHandlerIndex, size_t videoPartIndex) :signatureHandlerIndex{ signatureHandlerIndex }, videoPartIndex{ videoPartIndex }{}
	SearchWindow searchWindow;
	size_t signatureHandlerIndex;
	size_t videoPartIndex;
	size_t currentSignatureIndex = SIZE_MAX;
	bool isWorkWith = false;
};

struct SearchResult {
	size_t signatureHandlerIndex;
	std::vector < std::pair < size_t, size_t >> beginEndIndex;
	std::vector <size_t> errorAmount;
};