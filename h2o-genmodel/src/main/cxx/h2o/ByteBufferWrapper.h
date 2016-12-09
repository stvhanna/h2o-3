#ifndef H2O_BYTEBUFFERWRAPPER_H
#define H2O_BYTEBUFFERWRAPPER_H 1

#include "h2o/util.h"
#include <cstdint>

namespace h2o {

class ByteBufferWrapper {
public:
    const VectorOfBytes &_v;

public:
    ByteBufferWrapper(const VectorOfBytes &v) :
        _v(v)
    {}

    int get1U() {
        assert(0);
        // & 0xff;
    }

    uint16_t get2() {
        assert(0);
    }

    uint32_t get3() {
        assert(0);
    }

    uint32_t get4() {
        assert(0);
    }

    float get4f() {
        assert(0);
    }

    void skip(int n) {
        assert(0);
    }
};

}

#endif
