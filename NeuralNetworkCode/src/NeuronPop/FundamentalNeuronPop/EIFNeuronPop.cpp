#include "EIFNeuronPop.hpp"


void EIFNeuronPop::advect(std::vector<double> * synaptic_dV)
{

    //int   totalNeurons = this->GetTotalNeurons();
    //int   pop;
    double dt = info->dt;

    ClearSpiker();

    for(unsigned long i = 0 ; i < noNeurons; i++){
		//(a) wait for refractory period
		if (info->time_step - previous_spike_step[i] <= refractorySteps)
			continue;
		//(b)advect
		potential[i] += dt / tau_m * (-(potential[i] - Vleak) + dt / tau_m * sharpness * exp((potential[i] - v_critical) / sharpness)) + synaptic_dV->at(i);
		potential[i] = fmax(V_lowerbound, potential[i]);
		//(c)determine if neuron has spiked
		if (this->potential[i] > v_thresh) {
			spiker.push_back(i);
			potential[i] = v_reset;
		}
        //    while(this->potential[i] > v_thresh)
        //        this->potential[i] -= v_thresh-v_reset;
        //}
    }
}

void EIFNeuronPop::LoadParameters(std::vector<std::string> *input) {

	NeuronPop::LoadParameters(input);

	std::string              name, token;
	std::vector<std::string> values;

	for (std::vector<std::string>::iterator it = (*input).begin(); it != (*input).end(); ++it) {
		SplitString(&(*it), &name, &values);
		if (name.find("V_Crit") != std::string::npos) {
			v_critical= std::stod(values.at(0));
		}
		if (name.find("sharpness") != std::string::npos) {
			sharpness = std::stod(values.at(0));
		}
		if (name.find("V_lowerbound") != std::string::npos) {
			V_lowerbound = std::stod(values.at(0));
		}
		if (name.find("V_leak") != std::string::npos) {
			Vleak = std::stod(values.at(0));
		}
	}
}

void EIFNeuronPop::SaveParameters(std::ofstream * stream){

	std::string id = "neurons_" + std::to_string(GetId());

	NeuronPop::SaveParameters(stream);
	*stream << id + "_V_Crit                      " << std::to_string(v_critical) << " mV\n";
	*stream << id + "_sharpness                   " << std::to_string(sharpness) << "\n";
	*stream << id + "_V_lowerbound                " << std::to_string(V_lowerbound) << " mV\n";
	*stream << id + "_V_leak                      " << std::to_string(Vleak) << " mV\n";
    *stream <<  "#\t\tEIF neuron : dV/dt = -(V-Vleak)/tau_m + sharpness/tau_m * exp((V-V_Crit)/sharpness) + RI/tau_m \n";
	*stream <<  "#\t\tVcannot be lower than V_lowerbound";
	*stream <<  "#\t\treset: v = v_reset + (v - v_thresh)\n";
}

/*EIFNeuronPop::EIFNeuronPop(double tm, double vr, double vc, double s, double vt, double t) :
NeuronPop()
{
    this->tau_m = tm;
    this->v_reset = vr;
    this->v_critical = vc;
    this->sharpness = s;
    this->v_thresh = vt;
    this->dt = t;
}*/
