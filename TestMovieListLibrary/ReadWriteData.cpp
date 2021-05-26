#include "stdafx.h"
#include "ReadWriteData.h"

const std::string ReadWriteData::dataFileName = "Data/dataBase.json";
const std::string ReadWriteData::baseName = "DataBase";

ReadWriteData* ReadWriteData::instance = 0;

ReadWriteData* ReadWriteData::getInstance()
{
    if (instance == 0){
        instance = new ReadWriteData();
    }

    return instance;
}

ReadWriteData::ReadWriteData()
{}