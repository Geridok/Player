//
// Created by Vladimir Mustafin on 22.02.2021.
//

#ifndef ALGORITHMIMPL_SEARCHINFO_H
#define ALGORITHMIMPL_SEARCHINFO_H

#include <cstdlib>
#include "VideoPart.h"

struct SearchWindow {
    SearchWindow(size_t leftLimit, size_t rightLimit) : leftLimit{leftLimit}, rightLimit{rightLimit} {}

    size_t leftLimit;
    size_t rightLimit;
};

struct SearchInfo {
    SearchInfo(size_t leftLimit, size_t rightLimit, std::shared_ptr<VideoPart> videoPart, size_t sigIndex)
            : searchWindow{leftLimit, rightLimit}, videoPart{std::move(videoPart)}, sigIndex{sigIndex} {
        isWorkWith = true;
    }

    SearchWindow searchWindow;
    std::shared_ptr<VideoPart> videoPart;
    size_t sigIndex;
    bool isWorkWith;

//    bool operator==(const SearchInfo&a) const{
//        return a.videoPart == videoPart;
//    }
};

#endif //ALGORITHMIMPL_SEARCHINFO_H
