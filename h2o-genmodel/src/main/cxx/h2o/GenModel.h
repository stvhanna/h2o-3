#ifndef H2O_GENMODEL_H
#define H2O_GENMODEL_H 1

#include <string>
#include <vector>
#include <map>

namespace h2o {

class GenModel {
private:
    std::vector<std::string> _names;
    std::vector<bool> _isCategorical;
    std::map<int, std::vector<std::string> > _domains;
    // int _offsetColumnIdx;

protected:
    static int getPrediction(const std::vector<double> &preds,
                             const std::vector<double> &priorClassDist,
                             const std::vector<double> &data,
                             double threshold) {
        if (preds.size() == 3) {
            return (preds[2] >= threshold) ? 1 : 0; //no tie-breaking
        }

        // TODO: multinomial
        assert(0);
        return -999;
    }

public:
    GenModel()
    //    :
    //    _offsetColumnIdx(-1)
    {}

    virtual ~GenModel() {}

    virtual int nfeatures() const = 0;
    virtual int nclasses() const = 0;
    virtual bool isClassifier() const = 0;
    virtual bool isAutoEncoder() const = 0;
    virtual int getPredsSize() const = 0;
    virtual std::vector<double> &score0(const std::vector<double> &row, std::vector<double> &preds) const = 0;
};

}

#endif