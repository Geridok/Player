#pragma once

#include <utility>

#include "VideoPart.h"
#include "SignatureCalculator/Data/Signature.h"
extern double c_3;
extern size_t partSize;

static void chooseMainSignature(std::shared_ptr<VideoPart>& videoPart, std::vector<CSignature*> signatures) {
    size_t index = videoPart->firstSignatureIndex;
    for (index; index <= videoPart->lastSignatureIndex; index++) {
        videoPart->mainSignatureIndex = index;
        if (!signatures[videoPart->mainSignatureIndex]->isBlack()) {
            return;
        }
    }

}

static std::vector<std::shared_ptr<VideoPart>> splitVideo(std::vector<CSignature*> signatures) {

    ATLASSERT(signatures.size() != 0);
    if (signatures.size() == 0) {
        return {};
    }

    size_t currentIndex = 0;
    size_t partVector = 0;
    bool endVideoPart = false; //булевая переменная, означающая что работа с текущим VideoPart окончена.
    auto newVideoPart = std::make_shared<VideoPart>();
    newVideoPart->mainSignatureIndex = currentIndex;
    newVideoPart->firstSignatureIndex = currentIndex;
    newVideoPart->selfIndex = partVector;
    std::vector<std::shared_ptr<VideoPart>> videoPartHandler;
    currentIndex++;
    size_t signatureAmount = 1;
    bool isSpecial = false;
    while (currentIndex < signatures.size()) {
        auto diff = signatures[newVideoPart->firstSignatureIndex]->difference(*signatures[currentIndex]);
        if (diff > c_3) {
            if (signatureAmount <= partSize) {
                isSpecial = true;
            }
            else {
                endVideoPart = true;
            }
        }
        if (isSpecial && signatureAmount == partSize) endVideoPart = true;
        if (endVideoPart) {
            newVideoPart->lastSignatureIndex = currentIndex - 1;
            chooseMainSignature(newVideoPart, signatures);
            if (isSpecial) newVideoPart->mainSignatureIndex = newVideoPart->lastSignatureIndex;
            videoPartHandler.push_back(newVideoPart);
            partVector++;
            newVideoPart = std::make_shared<VideoPart>(currentIndex, currentIndex, 0, partVector);
            endVideoPart = false;
            isSpecial = false;
            signatureAmount = 0;
        }
        currentIndex++;
        signatureAmount++;
    }
    if (!videoPartHandler.empty()) {
        if (videoPartHandler[videoPartHandler.size() - 1]->lastSignatureIndex != signatures.size() - 1) {
            newVideoPart->lastSignatureIndex = signatures.size() - 1;
            chooseMainSignature(newVideoPart, signatures);
            if (isSpecial) newVideoPart->mainSignatureIndex = newVideoPart->mainSignatureIndex;
            videoPartHandler.push_back(newVideoPart);
        }
    }
    else {
        newVideoPart->lastSignatureIndex = signatures.size() - 1;
        chooseMainSignature(newVideoPart, signatures);
        if (isSpecial) newVideoPart->mainSignatureIndex = newVideoPart->lastSignatureIndex;
        videoPartHandler.push_back(newVideoPart);
    }

    return videoPartHandler;
}

//
//static std::vector<std::shared_ptr<VideoPart>> splitVideo_2(std::vector<CSignature*> signatures) {
//
//    ATLASSERT(signatures.size() != 0);
//    if (signatures.size() == 0) {
//        return {};
//    }
//
//
//    size_t partVector = 0;
//    bool endVideoPart = false; //булевая переменная, означающая что работа с текущим VideoPart окончена.
//    auto newVideoPart = std::make_shared<VideoPart>();
//    newVideoPart->mainSignatureIndex = 0;
//    newVideoPart->firstSignatureIndex = 0;
//    newVideoPart->selfIndex = partVector;
//    std::vector<std::shared_ptr<VideoPart>> videoPartHandler;
//
//    size_t signatureAmount = 0;
//    bool isSpecial = false;
//
//    for (size_t signatureIndex = 0; signatureIndex < signatures.size(); signatureIndex++) {
//        auto diff = signatures[signatureIndex]->difference(*signatures[newVideoPart->firstSignatureIndex]);
//        if (diff > c_3) {
//            if (signatureAmount <= partSize) {
//                isSpecial = true;
//            }
//            else {
//                endVideoPart = true;
//            }
//            if (isSpecial && signatureAmount == partSize) endVideoPart = true;
//            if (endVideoPart) {
//
//            }
//    }
//
//    while (currentIndex + 1 != signatures.size()) {
//        auto diff = signatures[currentIndex]->difference(*signatures[newVideoPart->firstSignatureIndex]);
//        if (diff > c_3) {
//            if (signatureAmount <= partSize) {
//                isSpecial = true;
//            }
//            else {
//                endVideoPart = true;
//            }
//        }
//        if (isSpecial && signatureAmount == partSize) endVideoPart = true;
//        if (endVideoPart) {
//            newVideoPart->lastSignatureIndex = currentIndex;
//            chooseMainSignature(newVideoPart, signatures);
//            if (isSpecial) newVideoPart->mainSignatureIndex = currentIndex;
//            videoPartHandler.push_back(newVideoPart);
//            partVector++;
//            newVideoPart = std::make_shared<VideoPart>(currentIndex + 1, currentIndex + 1, 0, partVector);
//            endVideoPart = false;
//            isSpecial = false;
//            signatureAmount = 0;
//        }
//        currentIndex++;
//        signatureAmount++;
//    }
//    if (!videoPartHandler.empty()) {
//        if (videoPartHandler[videoPartHandler.size() - 1]->lastSignatureIndex != signatures.size() - 1) {
//            newVideoPart->lastSignatureIndex = signatures.size() - 1;
//            chooseMainSignature(newVideoPart, signatures);
//            if (isSpecial) newVideoPart->mainSignatureIndex = currentIndex;
//            videoPartHandler.push_back(newVideoPart);
//        }
//    }
//    else {
//        newVideoPart->lastSignatureIndex = signatures.size() - 1;
//        chooseMainSignature(newVideoPart, signatures);
//        if (isSpecial) newVideoPart->mainSignatureIndex = currentIndex;
//        videoPartHandler.push_back(newVideoPart);
//    }
//
//    return videoPartHandler;
//}