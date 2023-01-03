#include "CoreDataParseUtil.h"
namespace cst_rtccore
{

    std::string CoreDataParseUtil::getLevelStr(int level)
    {
        std::string levelStr = "";
        for (int i = 0; i < level; i++)
        {
            levelStr += "\t"; //这里可以\t换成你所需要缩进的空格数
        }
        return levelStr;

    }

    std::string CoreDataParseUtil::formatJson(std::string json)
    {
        std::string result = "";
        int level = 0;
        for (std::string::size_type index = 0; index < json.size(); index++)
        {
            char c = json[index];

            if (level > 0 && '\n' == json[json.size() - 1])
            {
                result += getLevelStr(level);
            }

            switch (c)
            {
            case '{':
            case '[':
                result = result + c + "\n";
                level++;
                result += getLevelStr(level);
                break;
            case ',':
                result = result + c + "\n";
                result += getLevelStr(level);
                break;
            case '}':
            case ']':
                result += "\n";
                level--;
                result += getLevelStr(level);
                result += c;
                break;
            default:
                result += c;
                break;
            }

        }
        return result;
    }


    nlohmann::json CoreDataParseUtil::parseData(std::string data){
        if(data.size() == 0){
            return nlohmann::json();
        }
        return nlohmann::json::parse(data, nullptr, false, true);
    }


    int CoreDataParseUtil::getInt(nlohmann::json object, std::string key){
        if(object.contains(key) && object[key] != nullptr && object[key].is_number_integer()){
            return object[key].get<int>();
        }
        return 0;
    }

    unsigned int CoreDataParseUtil::getUnsignedInt(nlohmann::json object, std::string key){
        if(object.contains(key) && object[key] != nullptr && object[key].is_number_unsigned()){
            return object[key].get<unsigned int>();
        }
        return 0;
    }

    long CoreDataParseUtil::getLong(nlohmann::json object, std::string key){
        if(object.contains(key) && object[key] != nullptr && object[key].is_number()){
            return object[key].get<long>();
        }
        return 0;
    }

    long long CoreDataParseUtil::getLongLong(nlohmann::json object, std::string key) {
        if (object.contains(key) && object[key] != nullptr && object[key].is_number()) {
            return object[key].get<long long>();
        }
        return 0;
    }

    double CoreDataParseUtil::getDouble(nlohmann::json object, std::string key){
        if(object.contains(key) && object[key] != nullptr && object[key].is_number_float()){
            return object[key].get<double>();
        }
        return 0.0;
    }

    bool CoreDataParseUtil::getBool(nlohmann::json object, std::string key, bool defaultVal){
        if(object.contains(key) && object[key] != nullptr){
            return object[key].get<bool>();
        }
        return defaultVal;
    }

    std::string CoreDataParseUtil::getString(nlohmann::json object, std::string key){
        if(object.contains(key) && object[key] != nullptr){
            return object[key].get<std::string>();
        }
        return "";
    }

    std::string CoreDataParseUtil::getData(nlohmann::json object, std::string key){
        if(object.contains(key) && object[key] != nullptr){
            return object[key].dump();
        }
        return "";
    }

    std::map<std::string, std::string> CoreDataParseUtil::convertMap(std::string data) {
        nlohmann::json json = parseData(data);
        std::map<std::string, std::string> map;
        for (nlohmann::json::iterator it = json.begin(); it != json.end(); ++it) {
            map.emplace(it.key(), it.value());
        }
        return map;
    }

}
