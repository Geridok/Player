//
// Created by Vladimir Mustafin on 22.02.2021.
//

#include "PartsSearcher.h"

struct fakeStream {
    CSignature *getNextFrame() {
        CSignature *signaturePtr = new CSignature();
        return signaturePtr;
    }

    bool isEmpty() {
        return false;
    }
};

struct dataBase {
    std::pair<std::vector<VideoPart>, std::vector<double>> find() {
        return std::make_pair(std::vector<VideoPart>{}, std::vector<double>{});
    }
};

void PartsSearcher::startSearching() {

    fakeStream stream;
    dataBase base;
    while (!stream.isEmpty()) {
        auto streamSignature = stream.getNextFrame();
        auto partsAndDiff = base.find();
        if (!partsAndDiff.first.empty()) {
            auto part = partsAndDiff.first;
            auto diff = partsAndDiff.second;
            size_t i = 0;
            while (i != partsAndDiff.first.size() - 1 || i != partsAndDiff.second.size() - 1) {
                VideoPart curPart = part[i];
                auto currentSearchWindow = std::find_if(searchInfoList.begin(), searchInfoList.end(),
                                                        [curPart](SearchInfo &SI) {
                                                            return *SI.videoPart == curPart;
                                                        });
                if (currentSearchWindow != searchInfoList.end()) {
                    workWithSearchInfo(*currentSearchWindow, streamSignature);
                } else {
                    if (diff[i] < c_2) {
                        searchInfoList.emplace_back(
                                SearchInfo(1, 1, std::make_shared<VideoPart>(part[i]), part[i].mainSignatureIndex));

                    } else if (diff[i] <= currentSearchWindow->videoPart->parent->getSceneChangeThreshold()) {
                        SearchInfo newSearchInfo = SearchInfo(1, 1, std::make_shared<VideoPart>(part[i]),
                                                              part[i].mainSignatureIndex);
                        auto isdefined = defineSearchWindow(newSearchInfo,
                                                            streamSignature);// - псевдо реализация ниже.
                        //Данное условие говорит о том, что кадры похожи, но недостаточно, чтобы быть уверенным, что это искомый кадр. Мы предполагаем, что кадр может находится внутри ячейки VideoPart.
                        if (isdefined) {
                            searchInfoList.emplace_back(newSearchInfo);
                        }

                    }
                }
                i++;
            }
        }
        for (auto &curSearchInfo:searchInfoList) {
            if (!curSearchInfo.isWorkWith) {
                workWithSearchInfo(curSearchInfo, streamSignature);
            }
        }
    }
}

void PartsSearcher::workWithSearchInfo(SearchInfo &currentSearchInfo, CSignature *streamSign) {
    currentSearchInfo.isWorkWith = true;
    auto curDiff = streamSign->difference(
            *currentSearchInfo.videoPart->parent->getSignatureByIndex(currentSearchInfo.sigIndex));
    if (curDiff < c_2) {
        //считаем что данный кадр и есть искомый.
        currentSearchInfo.sigIndex += 1;
    } else {
        auto isFind = checkSignInWindow(
                currentSearchInfo, streamSign);
        if (isFind) {
            //errorVector.pushback(currentIndex); TODO: подумать про вектор ошибок и как с ними раотать
            currentSearchInfo.sigIndex += 1;
        } else {
            searchInfoList.remove(currentSearchInfo);
            return;
        }
    }
    if (currentSearchInfo.sigIndex == currentSearchInfo.videoPart->lastSignatureIndex) {
        /*Данный блок означает, что кадры из текущего VideoPart закончились, мы ожидаем следующий VideoPart из текущего SignatureHandler.
                Массив структур VideoPart, так же будет храниться в SignatureHandler, поэтому пытаемся получить следующий. Если не получилось, закрываем промежуток:
        Иначе продолжаем работу.*/
        matchedVideoParts.push_back(currentSearchInfo.videoPart);
        auto nextVideoPart = currentSearchInfo.videoPart->parent->getNextVideoPart(
                currentSearchInfo.videoPart->selfIndex);
        if (nextVideoPart != currentSearchInfo.videoPart) {
            currentSearchInfo.videoPart = nextVideoPart;
            currentSearchInfo.sigIndex++;
            currentSearchInfo.searchWindow.leftLimit = 1;
            currentSearchInfo.searchWindow.rightLimit = 1;
        } else {
            searchInfoList.remove(currentSearchInfo);
        }
    }


}

bool PartsSearcher::checkSignInWindow(SearchInfo &currentSearchInfo, CSignature *streamSig) {
    size_t leftIndex = currentSearchInfo.sigIndex - currentSearchInfo.searchWindow.leftLimit;
    size_t rightIndex = currentSearchInfo.sigIndex + currentSearchInfo.searchWindow.rightLimit;
    std::vector<size_t> matchedSignatures;
    for (int sigIndex = leftIndex; sigIndex <= rightIndex ; ++sigIndex) {
        if(streamSig->difference(*currentSearchInfo.videoPart->parent->signatures[leftIndex]) < c_2){
            matchedSignatures.push_back(sigIndex);
        }
    }
    //TODO: подумать про вектор ошибок
    if (matchedSignatures.empty()) { return false; }
    chooseSignIndex(matchedSignatures,currentSearchInfo);
    currentSearchInfo.sigIndex += 1;
    currentSearchInfo.isWorkWith = true;
    return true;
    //при проходе по окну, можно так же брать кадры которые меньше c_2 и таким образом подрезать окно, а не прыгать туда сюда
}

bool PartsSearcher::defineSearchWindow(SearchInfo &currentSearchInfo, CSignature *streamSig) {
    std::vector<size_t> matchedSignatures;

    for (size_t ind = currentSearchInfo.videoPart->mainSignatureIndex;
         ind < currentSearchInfo.videoPart->lastSignatureIndex; ++ind) {
        if (streamSig->difference(*currentSearchInfo.videoPart->parent->signatures[ind]) <= c_2) {
            matchedSignatures.push_back(ind);
        }
    }

    if (matchedSignatures.empty()) { return false; }
    chooseSignIndex(matchedSignatures,currentSearchInfo);
    currentSearchInfo.isWorkWith = true;
    return true;
}

void PartsSearcher::chooseSignIndex(std::vector<size_t> &matchedSignatures, SearchInfo &currentSearchInfo) {
    if (matchedSignatures.size() > 1) {
        currentSearchInfo.searchWindow.leftLimit = 0;
        currentSearchInfo.searchWindow.rightLimit = matchedSignatures[matchedSignatures.size() - 1];
        currentSearchInfo.sigIndex = matchedSignatures[0];
    } else {
        currentSearchInfo.searchWindow.leftLimit = 1;
        currentSearchInfo.searchWindow.rightLimit = 1;
        currentSearchInfo.sigIndex = matchedSignatures[0];
    }
}
