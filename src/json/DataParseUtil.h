/**
 * json parse
 * author: hongbin.wang
 * */
#pragma once
#ifndef DATAPARSEUTIL_H
#define DATAPARSEUTIL_H
#include "json/json.h"
namespace lego_comm
{
    class DataParseUtil
    {
    public:
       static nlohmann::json parseData(std::string data);

       static std::string getLevelStr(int level);
       static std::string formatJson(std::string json);

       static int getInt(nlohmann::json object, std::string key);
       static unsigned int getUnsignedInt(nlohmann::json object, std::string key);
       static long getLong(nlohmann::json object, std::string key);
       static long long getLongLong(nlohmann::json object, std::string key);
       static double getDouble(nlohmann::json object, std::string key);
       static bool getBool(nlohmann::json object, std::string key, bool defaultVal);
       static std::string getString(nlohmann::json object, std::string key);
       static std::string getData(nlohmann::json object, std::string key);

       static std::map<std::string, std::string> convertMap(std::string data);
    };
}
#endif // COREDATAPARSEUTIL_H
