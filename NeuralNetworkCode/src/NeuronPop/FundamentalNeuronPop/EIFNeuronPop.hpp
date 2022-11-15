#ifndef EIFNeuronPop_HPP
#define EIFNeuronPop_HPP


#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include "../NeuronPop.hpp"

// sharpness > 0
// 0 < v_critical < v_thresh

class EIFNeuronPop : public NeuronPop
{
protected:    
    double v_critical;
    double sharpness;
	double V_lowerbound;
	double Vleak;
public:
    //EIFNeuronPop(double tm, double vr, double vc, double s, double vt, double t);
    EIFNeuronPop(GlobalSimInfo * info,int id) : NeuronPop(info,id) {}
    
    void advect(std::vector<double> * synaptic_dV);
	void LoadParameters(std::vector<std::string>* input);
    void SaveParameters(std::ofstream * stream);

    std::string GetType(){return str_EIFNeuron;}
};

#endif // EIFNeuronPop_HPP