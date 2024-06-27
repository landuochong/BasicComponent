#ifndef MMAP_UTIL_H_
#define MMAP_UTIL_H_

#include "mmap.hpp"
namespace mio {
bool IsMmapFileOpenSucc(const mio::mmap_sink& _mmmap_file);

bool OpenMmapFile(const char* _filepath, unsigned int _size, mio::mmap_sink& _mmmap_file);

void CloseMmapFile(mio::mmap_sink& _mmmap_file);

}

#endif /* MMAP_UTIL_H_ */
