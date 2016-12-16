#ifndef H2O_GBMMOJOMODEL_H
#define H2O_GBMMOJOMODEL_H 1

#include "h2o/SharedTreeMojoModel.h"
#include "h2o/DistributionFamily.h"
#include "h2o/MojoReaderBackend.h"

#include <cassert>

namespace h2o {

class GbmMojoModel : public SharedTreeMojoModel {
public:

private:
    DistributionFamily *_family;
    double _init_f;

    // Disable copy constructor for now since _family is a pointer.
    GbmMojoModel (const GbmMojoModel &rhs);

public:
    GbmMojoModel() :
        _family(NULL)
    {}

    virtual ~GbmMojoModel() {
        if (_family != NULL) {
            delete _family;  _family = NULL;
        }
    }

    virtual void read(MojoReaderBackend &be) {
        SharedTreeMojoModel::readTreeCommon(be);
        std::string familyName = safeGetStringProperty("distribution");
        _family = DistributionFamily::valueOf(familyName);
        _init_f = safeGetDoubleProperty("init_f");
    }

    std::vector<double> &score0(const std::vector<double> &row, double offset, std::vector<double> &preds) const {
        scoreAllTrees(row, preds);
        if (_family->family() == DistributionFamily::bernoulli || _family->family() == DistributionFamily::modified_huber) {
            double f = preds[1] + _init_f + offset;
            preds[2] = _family->linkInv(f);
            preds[1] = 1.0 - preds[2];
        } else if (_family->family() == DistributionFamily::multinomial) {
            if (nclasses() == 2) { // 1-tree optimization for binomial
                preds[1] += _init_f + offset; //offset is not yet allowed, but added here to be future-proof
                preds[2] = -preds[1];
            }
            assert(0);
            // GenModel.GBM_rescale(preds);
        } else { // Regression
            double f = preds[0] + _init_f + offset;
            preds[0] = _family->linkInv(f);
            return preds;
        }
        if (balanceClasses()) {
            assert(0);
            // GenModel.correctProbabilities(preds, _priorClassDistrib, _modelClassDistrib);
        }
        preds[0] = GenModel::getPrediction(preds, priorClassDistrib(), row, defaultThreshold());
        return preds;
    }

    virtual std::vector<double> &score0(const std::vector<double> &row, std::vector<double> &preds) const {
        return score0(row, 0.0, preds);
    }
};

}

#endif