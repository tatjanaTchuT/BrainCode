#ifndef QIFNeuronPop_HPP
#define QIFNeuronPop_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include "../NeuronPop.hpp"

// sharpness > 0
// 0 < v_critical < v_thresh


class QIFNeuronPop : public NeuronPop
{
protected:
    double v_critical;
    double sharpness;
public:
    //QIFNeuronPop(double tm, double vr, double vc, double s, double vt, double t);
    QIFNeuronPop(GlobalSimInfo * info,int id) : NeuronPop(info,id) {}

    void advect(std::vector<double> * synaptic_dV);
    void SaveParameters(std::ofstream * stream);
    std::string GetType(){return str_QIFNeuron;}
};

#endif // QIFNeuronPop_HPP
