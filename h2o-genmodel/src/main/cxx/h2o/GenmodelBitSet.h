#ifndef H2O_GENMODELBITSET_H
#define H2O_GENMODELBITSET_H 1

namespace h2o {

class GenmodelBitSet {
private:
    // TODO: don't make a copy.
    VectorOfBytes _val;  // Holder of the bits, perhaps also holding other unrelated data

    int _byteoff; // Number of bytes skipped before starting to count bits
    int _nbits;   // Number of bits in this bitset
    int _bitoff;  // Number of bits discarded from beginning (inclusive min)

    static int bytes(int nbits) {
        return ((nbits-1) >> 3) + 1;
    }

public:
    GenmodelBitSet(int nbits) {
        GenmodelBitSet(nbits, 0);
    }

    GenmodelBitSet(int nbits, int bitoff) {
        // For small bitsets, just use a no-offset fixed-length format
        if (bitoff + nbits <= 32) {
            bitoff = 0;
            nbits = 32;
    }

        // TODO: don't stack allocate and copy this in fill().
        VectorOfBytes v;
        if (nbits <= 0) {
            v.resize(0);
    }
        else {
            v.resize(bytes(nbits));
        }

        fill(v, 0, nbits, bitoff);
    }

    void fill(const VectorOfBytes &v, int byteoff, int nbits, int bitoff) {
        if (nbits < 0) throw std::invalid_argument("nbits < 0: " /* + nbits */);
        if (byteoff < 0) throw std::invalid_argument("byteoff < 0: " /* + byteoff */);
        if (bitoff < 0) throw std::invalid_argument("bitoff < 0: " /* + bitoff */);
        bool t = ((v.size() == 0) || byteoff + ((nbits-1) >> 3) + 1 <= v.size());
        if (!t) throw std::invalid_argument("GenmodelBitSet fill:  assertion check failed");
        _val = v;
        _nbits = nbits;
        _bitoff = bitoff;
        _byteoff = byteoff;
    }

    void fill2(const VectorOfBytes &bits, ByteBufferWrapper &ab) {
        fill(bits, ab.position(), 32, 0);
        ab.skip(4);  // Skip inline bitset
    }

    void fill3(const VectorOfBytes &bits, ByteBufferWrapper &ab) {
        int bitoff = ab.get2();
        int nbytes = ab.get2();
        fill(bits, ab.position(), nbytes<<3, bitoff);
        ab.skip(nbytes);  // Skip inline bitset
    }

    bool contains(int idx) {
        if (idx < 0) throw std::invalid_argument("idx < 0: " /* + idx */);
        idx -= _bitoff;
        return (idx >= 0) && (idx < _nbits) &&
               (_val[_byteoff + (idx >> 3)] & ((uint8_t)1 << (idx & 7))) != 0;
    }
};

}

#endif
