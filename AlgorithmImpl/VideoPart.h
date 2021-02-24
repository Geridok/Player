//
// Created by Vladimir Mustafin on 21.02.2021.
//

#ifndef ALGORITHMIMPL_VIDEOPART_H
#define ALGORITHMIMPL_VIDEOPART_H

#include "WorkWithFrames.h"
#include <stdio.h>
bool compare_shared_ptr(const std::shared_ptr<T>&a,const std::shared_ptr<U>&b)
{
    if(a == b) return true;
    if(a && b) return *a == *b;
    return false;
}

struct VideoPart {
    VideoPart(size_t mainSignatureIndex, size_t lastSignatureIndex, std::shared_ptr<SignatureHandler> parent)
            : mainSignatureIndex{mainSignatureIndex},lastSignatureIndex{lastSignatureIndex}, parent{std::move(parent)} {}
    VideoPart(std::shared_ptr<SignatureHandler> parent): parent{std::move(parent)} {};
    size_t mainSignatureIndex;
    size_t lastSignatureIndex;
    std::shared_ptr<SignatureHandler> parent;
    size_t selfIndex;
    bool operator==(const VideoPart&a) const{
        return (a.mainSignatureIndex == mainSignatureIndex && a.lastSignatureIndex == lastSignatureIndex && parent == a.parent);
    }

};


#endif //ALGORITHMIMPL_VIDEOPART_H
