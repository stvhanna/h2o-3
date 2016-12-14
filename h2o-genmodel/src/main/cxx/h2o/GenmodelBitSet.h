#ifndef H2O_GENMODELBITSET_H
#define H2O_GENMODELBITSET_H 1

namespace h2o {

class GenmodelBitSet {
private:
    const VectorOfBytes *_v;
    int _byteoff;
    int _nbits;
    int _bitoff;

public:
    GenmodelBitSet() {}

    void fill(const VectorOfBytes &v, int byteoff, int nbits, int bitoff) {

        if(nbits < 0) throw std::invalid_argument("nbits < 0: " + std::to_string(nbits));
        if(byteoff < 0) throw std::invalid_argument("byteoff < 0: "+ std::to_string(byteoff));
        if(bitoff < 0) throw new std::invalid_argument("bitoff < 0: " + std::to_string(bitoff));

        assert(byteoff + ((nbits-1) >> 3) + 1 <= v.size());
        _v = &v;
        _nbits = nbits;
        _bitoff = bitoff;
        _byteoff = byteoff;
    }

    void fill2(const VectorOfBytes &tree, ByteBufferWrapper &ab) {
        assert(0);
    }

    void fill3(const VectorOfBytes &tree, ByteBufferWrapper &ab) {
        assert(0);
    }

    bool contains(int bitNumber) {
        if(bitNumber < 0) throw "index out of bounds";
        bitNumber -= _bitoff;
        if(bitNumber < 0 || bitNumber >= _nbits) return false;
        int byteId = bitNumber >> 3;
        int offset = byteId & 7;
        uint8_t b = _v->at(byteId);
        return (b & (1 << offset)) != 0;
    }
};

}

#endif
