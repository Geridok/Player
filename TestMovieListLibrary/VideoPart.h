//
// Created by Vladimir Mustafin on 21.02.2021.
//
#pragma once

#include <stdio.h>
#include <string>

class VideoPartNames {
public: 
    static std::string firstSignatureIndex;
    static std::string mainSignatureIndex;
    static std::string lastSignatureIndex;
    static std::string selfIndex;
};

struct VideoPart {
    VideoPart(size_t firstSignatureIndex,size_t mainSignatureIndex, size_t lastSignatureIndex,size_t selfIndex)
        : firstSignatureIndex{ firstSignatureIndex }, mainSignatureIndex{ mainSignatureIndex }, lastSignatureIndex{ lastSignatureIndex }, selfIndex{ selfIndex }{}
    VideoPart() {}
    size_t firstSignatureIndex;
    size_t mainSignatureIndex;
    size_t lastSignatureIndex;
    size_t selfIndex;
    bool isActive = false;
    bool operator==(const VideoPart&a) const{
        return (a.mainSignatureIndex == mainSignatureIndex && a.lastSignatureIndex == lastSignatureIndex && selfIndex== a.selfIndex);
    }
};


