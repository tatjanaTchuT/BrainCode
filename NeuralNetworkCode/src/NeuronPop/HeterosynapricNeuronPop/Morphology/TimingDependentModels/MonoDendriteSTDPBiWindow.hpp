//
// Created by Saif Ahmed on 15.07.21.
//

#ifndef NEURALNETWORK_MONODENDRITESYMMETRICSTDP_H
#define NEURALNETWORK_MONODENDRITESYMMETRICSTDP_H

#include "MonoDendriteSTDP.hpp"

class MonoDendriteSTDPBiWindow: public MonoDendriteSTDP {

protected:
    void updateLTP(unsigned long synId) override;
    void updateLTD(unsigned long synId) override;

    double gLTP(double deltaT) const override;
    double gLTD(double deltaT) const override;

    double aLTP(double theta) const override;
    double aLTD(double theta) const override;

    double getDistanceEffects(const SynapseExt* synA, const SynapseExt* synB) const override;
    double getTimingEffects(const SynapseExt* synA, const SynapseExt* synB) const override;

    double tauLTP{};
    double alpha{};

    double tauLTD{};
    double beta{};

public:
    explicit MonoDendriteSTDPBiWindow(GlobalSimInfo* info);
    ~MonoDendriteSTDPBiWindow() override = default;
    std::string getType() override;

    void SaveParameters(std::ofstream * stream) override;
    void LoadParameters(std::vector<std::string> *input) override;
};


#endif //NEURALNETWORK_MONODENDRITESYMMETRICSTDP_H
