////
//// Created by Saif Ahmed on 19.07.21.
////
//
//#ifndef NEURALNETWORK_MONODENDRITESTDPBIEXPONENTIAL_H
//#define NEURALNETWORK_MONODENDRITESTDPBIEXPONENTIAL_H
//
//#include "MonoDendriteSTDP.hpp"
//
//class MonoDendriteSTDPBiExponential: public MonoDendriteSTDP {
//
//protected:
//    void updateLTP(unsigned long synId) override;
//    void updateLTD(unsigned long synId) override;
//
//    double gLTP(double deltaT) const override;
//    double gLTD(double deltaT) const override;
//
//    double aLTP(double theta) const override;
//    double aLTD(double theta) const override;
//
//public:
//    explicit MonoDendriteSTDPBiExponential(GlobalSimInfo* info);
//    ~MonoDendriteSTDPBiExponential() override = default;
//    std::string getType() override;
//
//};
//
//
//#endif //NEURALNETWORK_MONODENDRITESTDPBIEXPONENTIAL_H
