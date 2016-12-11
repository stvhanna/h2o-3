#ifndef H2O_SHAREDTREEMOJOMODEL_H
#define H2O_SHAREDTREEMOJOMODEL_H 1

#include "h2o/util.h"
#include "h2o/MojoModel.h"
#include "h2o/ByteBufferWrapper.h"
#include "h2o/GenmodelBitSet.h"
#include <cmath>

namespace h2o {

class SharedTreeMojoModel : public MojoModel {
private:
    enum NaSplitDir {
        None = 0,

        NsdNaVsRest = 1,
        NsdNaLeft = 2,
        NsdNaRight = 3,

        NsdLeft = 4
    };

    int _ntree_groups;
    int _ntrees_per_group;
    std::vector<VectorOfBytes> _compressed_trees;

    int treeIndex(int groupIndex, int classIndex) const {
        return classIndex * _ntree_groups + groupIndex;
    }

    static double scoreTree(const VectorOfBytes &tree, const std::vector<double> &row, int nclasses, bool computeLeafAssignment) {
        ByteBufferWrapper ab = ByteBufferWrapper(tree);
        GenmodelBitSet bs;
        long bitsRight = 0;
        int level = 0;
        while (true) {
            int nodeType = ab.get1U();
            int colId = ab.get2();
            if (colId == 65535) return ab.get4f();
            int naSplitDir = ab.get1U();
            bool naVsRest = naSplitDir == NsdNaVsRest;
            bool leftward = naSplitDir == NsdNaLeft || naSplitDir == NsdLeft;
            int lmask = (nodeType & 51);
            int equal = (nodeType & 12);  // Can be one of 0, 8, 12
            if (equal == 4) {
                throw std::invalid_argument("scoreTree: illegal equal of 4");
            }

            float splitVal = -1;
            if (!naVsRest) {
                // Extract value or group to split on
                if (equal == 0) {
                    // Standard float-compare test (either < or ==)
                    splitVal = ab.get4f();  // Get the float to compare
                } else {
                    // Bitset test
//                    if (bs == null) bs = new GenmodelBitSet(0);
                    if (equal == 8)
                        bs.fill2(tree, ab);
                    else
                        bs.fill3(tree, ab);
                }
            }

            double d = row[colId];
            if (std::isnan(d) ? !leftward : !naVsRest && (equal == 0? d >= splitVal : bs.contains((int)d))) {
                // go RIGHT
                switch (lmask) {
                    case 0:  ab.skip(ab.get1U());  break;
                    case 1:  ab.skip(ab.get2());  break;
                    case 2:  ab.skip(ab.get3());  break;
                    case 3:  ab.skip(ab.get4());  break;
                    case 16: ab.skip(nclasses < 256? 1 : 2);  break;  // Small leaf
                    case 48: ab.skip(4);  break;  // skip the prediction
                    default:
                        char message[1024];
                        snprintf(message, sizeof(message)-1, "scoreTree: illegal lmask value 0x%x in tree", lmask);
                        throw std::invalid_argument(message);
                }
                if (computeLeafAssignment && level < 64) bitsRight |= 1 << level;
                lmask = (nodeType & 0xC0) >> 2;  // Replace leftmask with the rightmask
            } else {
                // go LEFT
                if (lmask <= 3)
                    ab.skip(lmask + 1);
            }

            level++;
            if ((lmask & 16) != 0) {
                if (computeLeafAssignment) {
                    bitsRight |= 1 << level;  // mark the end of the tree
                    return longBitsToDouble(bitsRight);
                } else {
                    return ab.get4f();
                }
            }
        }
    }

    static double scoreTree(const VectorOfBytes &tree, const std::vector<double> &row, int nclasses) {
        return scoreTree(tree, row, nclasses, false);
    }

protected:
    void readTreeCommon(MojoReaderBackend &be) {
        MojoModel::readCommon(be);

        _ntree_groups = safeGetIntProperty("n_trees");
        _ntrees_per_group = safeGetIntProperty("n_trees_per_class");
        _compressed_trees.resize(static_cast<unsigned int>(_ntree_groups));

        for (int j = 0; j < _ntree_groups; j++) {
            for (int i = 0; i < _ntrees_per_group; i++) {
                char buf[1024];
                snprintf(buf, sizeof(buf)-1, "trees/t%02d_%03d.bin", i, j);
                VectorOfBytes &one_compressed_tree = _compressed_trees[treeIndex(j, i)];
                be.getBinaryFile(buf, one_compressed_tree);
            }
        }
    }

    void scoreAllTrees(const std::vector<double> &row, std::vector<double> &preds) const {
        for (int i = 0; i < preds.size(); i++) {
            preds[i] = 0;
        }

        for (int i = 0; i < _ntrees_per_group; i++) {
            int k = nclasses() == 1 ? 0 : i + 1;
            for (int j = 0; j < _ntree_groups; j++) {
                int itree = treeIndex(j, i);
                preds[k] += scoreTree(_compressed_trees[itree], row, nclasses());
            }
        }
    }

public:
    virtual ~SharedTreeMojoModel() {}
};

}

#endif