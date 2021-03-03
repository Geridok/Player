//
// Created by Vladimir Mustafin on 22.02.2021.
//

#ifndef ALGORITHMIMPL_PARTSSEARCHER_H
#define ALGORITHMIMPL_PARTSSEARCHER_H

#include <list>
#include "SearchInfo.h"
#include "Signature.h"

class PartsSearcher {
public:
    explicit PartsSearcher(double c_2):c_2{c_2}{};
    void startSearching();
private:
    void workWithSearchInfo(SearchInfo &currentSearchInfo, CSignature* streamSign);
    bool defineSearchWindow(SearchInfo &currentSearchInfo, CSignature *streamSig);

    std::list<SearchInfo> searchInfoList;
    double c_2;
    std::vector<std::shared_ptr<VideoPart>> matchedVideoParts;

    bool checkSignInWindow(SearchInfo &currentSearchInfo, CSignature *streamSig);
    void chooseSignIndex(std::vector<size_t> &matchedSignatures, SearchInfo &currentSearchInfo);
};


#endif //ALGORITHMIMPL_PARTSSEARCHER_H
