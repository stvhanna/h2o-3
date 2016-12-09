#ifndef H2O_UTILS_H
#define H2O_UTILS_H 1

#include <sys/stat.h>
#include <string>
#include <vector>
#include <map>
#include <cassert>

namespace h2o {

typedef std::vector<uint8_t> VectorOfBytes;

inline int fileExists(const char *path) {
    struct stat fileStat;
    if (stat(path, &fileStat)) {
        return 0;
    }
    if (!S_ISREG(fileStat.st_mode)) {
        return 0;
    }
    return 1;
}

inline int dirExists(const char *path) {
    struct stat fileStat;
    if (stat(path, &fileStat)) {
        return 0;
    }
    if (!S_ISDIR(fileStat.st_mode)) {
        return 0;
    }
    return 1;
}

inline off_t fileSize(const char *path) {
    struct stat fileStat;
    if (stat(path, &fileStat)) {
        std::string errmsg("fileSize: file does not exist: ");
        errmsg += path;
        throw std::invalid_argument(errmsg);
    }
    if (!S_ISREG(fileStat.st_mode)) {
        std::string errmsg("fileSize: not a file: ");
        errmsg += path;
        throw std::invalid_argument(errmsg);
    }
    return fileStat.st_size;
}

inline std::string strerrorString(int errnum) {
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    int rv = strerror_r(errnum, buf, sizeof(buf)-1);
    if (rv) {
        // You've got to be kidding.
        std::string errmsg("strerrorString failed for errno: ");
        errmsg += errnum;
        throw std::invalid_argument(errmsg);
    }
    return buf;
}

template<class T>
inline bool mapContains(std::map<std::string,T> map, const std::string &name) {
    if (map.find(name) == map.end()) {
        // not found
        return false;
    }

    // found
    return true;
}

inline double longBitsToDouble(uint64_t bits) {
    assert(0);
}

}

#endif
