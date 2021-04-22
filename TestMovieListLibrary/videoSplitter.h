#pragma once

#include <utility>

#include "VideoPart.h"
#include "SignatureCalculator/Data/Signature.h"

static std::vector<std::shared_ptr<VideoPart>> splitVideo(double c_1,double c_3, std::vector<CSignature*> signatures){
    size_t currentIndex = 0;
    size_t partVector = 0;
    bool endVideoPart = false; //булевая переменная, означающая что работа с текущим VideoPart окончена.
    auto newVideoPart = std::make_shared<VideoPart>();
    newVideoPart->mainSignatureIndex = currentIndex;
    newVideoPart->selfIndex = partVector;
    std::vector<std::shared_ptr<VideoPart>> videoPartHandler;
    while(currentIndex + 1 != signatures.size()){

        auto diff = signatures[currentIndex]->difference(*signatures[currentIndex + 1]);
        if(diff <= c_1){
            if(signatures[currentIndex]->difference(*signatures[newVideoPart->mainSignatureIndex]) > c_3) {
                endVideoPart = true;
            }
        }else{
            endVideoPart = true;
        }
        if(endVideoPart){
            newVideoPart->lastSignatureIndex = currentIndex;
            videoPartHandler.push_back(newVideoPart);
            partVector ++;
            newVideoPart = std::make_shared<VideoPart>(currentIndex + 1,0, partVector);
            endVideoPart = false;
        }
        currentIndex ++;
    }
    return videoPartHandler;
}

