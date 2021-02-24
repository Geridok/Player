//
// Created by Vladimir Mustafin on 21.02.2021.
//

#ifndef ALGORITHMIMPL_VIDEOSPLITTER_H
#define ALGORITHMIMPL_VIDEOSPLITTER_H

#include <utility>

#include "VideoPart.h"
#include "Signature.h"

static std::vector<std::shared_ptr<VideoPart>> splitVideo(double c_1,double c_2, std::vector<CSignature*> signatures,std::shared_ptr<SignatureHandler> parent){
    size_t currentIndex = 0;
    size_t partVector = 0;
    bool endVideoPart = false; //булевая переменная, означающая что работа с текущим VideoPart окончена.
    auto newVideoPart = std::make_shared<VideoPart>(std::move(parent));
    newVideoPart.mainSignatureIndex = currentIndex;
    newVideoPart->selfIndex = partVector;
    std::vector<std::shared_ptr<VideoPart>> videoPartHandler;
    while(currentIndex + 1 != signatures.size()){

        auto diff = signatures[currentIndex]->difference(signatures[currentIndex + 1]);
        if(diff <= c_1){
            if(signatures[currentIndex]->difference(signatures[newVideoPart.mainSignatureIndex])) >= c_2{
                endVideoPart = true;
            }
        }else if(diff == 1){
           // Данный случай необходимо исследовать отдельно, т.к. в курсовой работе [], был отмечен один из недостатков данного алгоритма, а именно: если после абсолютного черного кадра, идет кадр с слегка увеличенной яркостью, данный алгоритм показывает максимальное значение разницы кадров, что не является истинной.
                    if(checkIfBlack(signatures[currentIndex], signatures[currentIndex + 1]){
                //блок обработки особого случая
            }
        }else{
            endVideoPart = true;
        }
        if(endVideoPart){
            newVideoPart.lastFrame = currentIndex;
            videoPartHandler.pushback(newVideoPart);
            partVector ++;
            newVideoPart = std::make_shared<VideoPart>(currentIndex + 1,0,parent);
            newVideoPart->selfIndex = partVector;
            endVideoPart = false;
        }
        currentIndex ++;
    }
    return videoPartHandler;
}

#endif //ALGORITHMIMPL_VIDEOSPLITTER_H
