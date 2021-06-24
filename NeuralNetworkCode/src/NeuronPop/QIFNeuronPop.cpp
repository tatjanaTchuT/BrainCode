#include "QIFNeuronPop.hpp"

/*QIFNeuronPop::QIFNeuronPop(double tm, double vr, double vc, double s, double vt, double t) :
NeuronPop()
{
    tau_m = tm;
    v_reset = vr;
    v_critical = vc;
    sharpness = s;
    v_thresh = vt;
    dt = t;
}*/

void QIFNeuronPop::advect(std::vector<double> * synaptic_dV)
{
    double dt = info->dt;
    //int totalNeurons = GetTotalNeurons();
    // int pop;

    ClearSpiker();

    for(unsigned long i = 0 ; i < noNeurons; i++)
    {
        potential[i] += potential[i] *dt/tau_m * sharpness * (potential[i] - v_critical ) + synaptic_dV->at(i);
        if(this->potential[i] > v_thresh)
        {
            spiker.push_back(i);
            while(this->potential[i] > v_thresh)
                this->potential[i]-= v_thresh-v_reset;
        }
    }
}


void QIFNeuronPop::SaveParameters(std::ofstream * stream){

    NeuronPop::SaveParameters(stream);
    *stream <<  "#\t\tQIF neuron (UNDER CONSTRUCTION): dV/dt = V /tau_m * sharpness * (V - v_critical) + RI/tau_m\n";
    *stream <<  "#\t\treset: v = v_reset + (v - v_thresh)\n";
    *stream <<  "#\t\tUNDER CONSTRUCTION: sharpness/ v_critical not defined\n";

}
