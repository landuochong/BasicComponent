// Tencent is pleased to support the open source community by making Mars available.
// Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

// Licensed under the MIT License (the "License"); you may not use this file except in 
// compliance with the License. You may obtain a copy of the License at
// http://opensource.org/licenses/MIT

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and
// limitations under the License.

#ifndef COMM_STRUTIL_H_
#define COMM_STRUTIL_H_

#include <string>
#include <vector>

// declaration
namespace strutil {
/////////////////////// string /////////////////////////////
std::string& URLEncode(const std::string& url, std::string& encodeUrl);

std::string& TrimLeft(std::string& str);
std::string& TrimRight(std::string& str);
std::string& Trim(std::string& str);

std::string& ToLower(std::string& str);
std::string& ToUpper(std::string& str);

bool StartsWith(const std::string& str, const std::string& substr);
bool EndsWith(const std::string& str, const std::string& substr);

std::vector<std::string>& SplitToken(const std::string& str,
                                     const std::string& delimiters, std::vector<std::string>& ss);

// T1 is iterator, T2 is string or wstring
template<typename T1, typename T2> bool MergeToken(const T1& begin, const T1& end, const T2& delimiter, T2& result);
/////////////////////// wstring /////////////////////////////
std::wstring& TrimLeft(std::wstring& str);
std::wstring& TrimRight(std::wstring& str);
std::wstring& Trim(std::wstring& str);

bool StartsWith(const std::wstring& str, const std::wstring& substr);
bool EndsWith(const std::wstring& str, const std::wstring& substr);

std::wstring& ToLower(std::wstring& str);
std::wstring& ToUpper(std::wstring& str);

#ifdef WIN32
std::wstring String2WString(const std::string& _src, unsigned int _cp);
std::wstring UTF8String2Wstring(const std::string& _src);
std::string String2UTF8(const std::string& str);
void StringReplace(const std::string& s, const std::string& oldsub,
    const std::string& newsub, bool replace_all,
    std::string* res);
#endif
std::vector<std::wstring>& SplitToken(const std::wstring& str,
                                      const std::wstring& delimiters, std::vector<std::wstring>& ss);

// Tokenizer class
template<class T> struct default_delimiters {};
template<> struct default_delimiters<std::string>  { static const char* value() { return " \t\n\r;:,.?";} };
template<> struct default_delimiters<std::wstring> { static const wchar_t* value() { return L" \t\n\r;:,.?";}};
    
template<class T>
class Tokenizer {
  public:
    Tokenizer(const T& str, const T& delimiters = default_delimiters<T>::value())
        : offset_(0), string_(str),  delimiters_(delimiters) {}

    void Reset() {offset_ = 0;}
    const T GetToken() const {return token_;}
    bool NextToken() {return NextToken(delimiters_);}
    bool NextToken(const T& delimiters) {
        // find the start charater of the next token.
        typename T::size_type i = string_.find_first_not_of(delimiters, offset_);

        if (i == T::npos) {
            offset_ = string_.length();
            return false;
        }

        // find the end of the token.
        typename T::size_type j = string_.find_first_of(delimiters, i);

        if (j == T::npos) {
            token_ = string_.substr(i, string_.length() - i);
            offset_ = string_.length();
            return true;
        }

        // to intercept the token and save current position
        token_ = string_.substr(i, j - i);
        offset_ = j;
        return true;
    }

  private:
    Tokenizer(const Tokenizer&);
    Tokenizer& operator=(const Tokenizer&);

  protected:
    typename T::size_type offset_;

    const T string_;
    T token_;
    T delimiters_;
};

template<typename T1, typename T2> bool MergeToken(const T1& begin, const T1& end, const T2& delimiter, T2& result) {
    if (begin == end) {
        return false;
    }

    if (delimiter.empty()) {
        return false;
    }

    result.clear();

    for (T1 iter = begin; iter != end; ++iter) {
        result += *iter;

        if (iter + 1 != end) {
            result += delimiter;
        }
    }

    return true;
}

template <typename T> //T must be integer type
T to_little_endian(T x) 
{
    size_t n = sizeof(T) / sizeof(uint8_t);//2,4,8

    T res;
    uint8_t* p = (uint8_t*)res;
    T mask = static_cast<T>(0xFF);
    for (size_t i = 0; i < n;i++) {
        int offset = 8 * i;
        p[i] = (x & (mask << offset)) >> offset;
    }
    return res;
}


static inline bool little_endianess(int num = 1) noexcept
{
    return *reinterpret_cast<char*>(&num) == 1;
}



std::string Hex2Str(const char* _str, unsigned int _len);
std::string Str2Hex(const char* _str, unsigned int _len);

std::string ReplaceChar(const char* const input_str, char be_replaced='@', char replace_with='.');
    
std::string GetFileNameFromPath(const char* _path);
    
// find substring (case insensitive)
size_t ci_find_substr(const std::string& str, const std::string& sub, size_t pos);
std::string BufferMD5(const void* buffer, size_t size);
std::string MD5DigestToBase16(const uint8_t digest[16]);
std::string DigestToBase16(const uint8_t *digest, size_t length);


}

#endif	// COMM_STRUTIL_H_
