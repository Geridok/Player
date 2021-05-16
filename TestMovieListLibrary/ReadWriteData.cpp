#include "stdafx.h"
#include "ReadWriteData.h"

const std::string ReadWriteData::dataFileName = "Data/dataBase_";
const std::string ReadWriteData::baseName = "DataBase";

ReadWriteData* ReadWriteData::instance = nullptr;

ReadWriteData* ReadWriteData::getInstance()
{
    if (nullptr == instance){
        instance = new ReadWriteData();
    }

    return instance;
}

ReadWriteData::ReadWriteData()
{}