#ifndef H2O_BYTEBUFFERWRAPPER_H
#define H2O_BYTEBUFFERWRAPPER_H 1

#include "h2o/util.h"
#include <cstdint>

namespace h2o {

class ByteBufferWrapper {
private:
    const VectorOfBytes &_v;
    int _position;

public:
    // This should be static_assert(), but that fails to compile under gcc -Wall -Werror.
    // So do this instead every time a mojo is created, which isn't so bad.
    static void performSanityChecks() {
        if (sizeof(float) != 4) {
            throw std::invalid_argument("expected float size to be 4");
        }
    }

    ByteBufferWrapper(const VectorOfBytes &v) :
        _v(v),
        _position(0)
    {}

    uint8_t get1U() {
        int result = _v[_position] & 0xff;
        _position++;
        return result;
    }

    uint16_t get2() {
        return get1U() | (get1U() << 8);
    }

    uint32_t get3() {
        return get1U() | (get1U() << 8) | (get1U() << 16);
    }

    uint32_t get4() {
        return get1U() | (get1U() << 8) | (get1U() << 16) | (get1U() << 24);
    }

    float get4f() {
        uint8_t buf[4];
#if _LIBCPP_BIG_ENDIAN == 1
#error Note:  Big endian never tested.  Remove this error and test.
        buf[3] = get1U();
        buf[2] = get1U();
        buf[1] = get1U();
        buf[0] = get1U();
#elif _LIBCPP_LITTLE_ENDIAN == 1
        buf[0] = get1U();
        buf[1] = get1U();
        buf[2] = get1U();
        buf[3] = get1U();
#else
#error Can not determine endianness.
#endif
        float f = *(reinterpret_cast<float*>(&buf[0]));
        return f;
    }

    void skip(int n) {
        if(_position + n >= _v.size()) {
            throw std::invalid_argument("ByteBufferWrapper skip too big");
        }
        _position += n;
    }

    int position() {
        return _position;
    }
};

}

#endif
