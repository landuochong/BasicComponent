#include "mmap_util.h"
#include <stdio.h>
#include <filesystem>
#include <iostream>
#include <fstream>
namespace fs = std::filesystem;

namespace mio {
bool IsMmapFileOpenSucc(const mio::mmap_sink& _mmmap_file) {
    return _mmmap_file.is_open() && _mmmap_file.is_mapped();
}

int handle_error(const std::error_code& error)
{
    const auto& errmsg = error.message();
    std::printf("error mapping file: %s, exiting...\n", errmsg.c_str());
    return error.value();
}

void allocate_file(const std::string& path, const int size)
{
    std::ofstream file(path);
    std::string s(size, '0');
    file << s;
}

bool OpenMmapFile(const char* _filepath, unsigned int _size, mio::mmap_sink& _mmmap_file) {
    if (NULL == _filepath || 0 == strnlen(_filepath, 128) || 0 == _size) {
        return false;
    }

    if (IsMmapFileOpenSucc(_mmmap_file)) {
        CloseMmapFile(_mmmap_file);
    }
    
    if (_mmmap_file.is_open() && _mmmap_file.is_mapped()) {
        return false;
    }

    //check file exsists
    bool file_exist = fs::exists(_filepath);
    if (!file_exist) {
        std::filesystem::path p(_filepath);
        auto parent = p.parent_path();
        if(!fs::exists(parent)){
            fs::create_directories(parent);
        }

        allocate_file(_filepath, _size);
    }else{
        uint32_t size =fs::file_size(_filepath);
        if(size < _size){
            allocate_file(_filepath, _size);
        }
    }

    //start mmap
    std::error_code error;
    _mmmap_file.map(_filepath,0,_size,error);

    bool is_open = IsMmapFileOpenSucc(_mmmap_file);
    return is_open;
}

void CloseMmapFile(mio::mmap_sink& _mmmap_file) {
    if (_mmmap_file.is_open()) {
        std::error_code error;
        _mmmap_file.sync(error);
        if (error) {
            handle_error(error);
            return;
        }
        _mmmap_file.unmap();
    }
}

}
