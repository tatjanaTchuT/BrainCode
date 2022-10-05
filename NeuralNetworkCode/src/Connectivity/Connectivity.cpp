
#include "Connectivity.hpp"
#include "../Synapse/Synapse.hpp"

Connectivity::Connectivity(Synapse * syn,GlobalSimInfo  * info){
    this->info = info;
    synapse   = syn;
    target_id = new std::vector<unsigned long> [synapse->GetNoNeuronsPre()];
    D_distribution = new std::vector<int> [synapse->GetNoNeuronsPre()];
    J_distribution = new std::vector<double> [synapse->GetNoNeuronsPre()];

	HasPot = true;
	HasDdistribution = true;//By default, set as true

    fixSeed = false; //by default, seed can be changed
}

Connectivity::~Connectivity(){
    delete [] target_id;
}

void Connectivity::SaveParameters(std::ofstream * stream,std::string id_str){
     *stream << id_str << "connectivity_type\t\t\t\t" << GetTypeStr() << "\n";
    //if(info->globalSeed == -1){
    *stream << id_str << "connectivity_seed                  " << std::to_string(this->seed)  << "\n";
    //}
}

void Connectivity::Test(){

    for(unsigned long i = 0;i<synapse->GetNoNeuronsPre();i++){
        for(unsigned int j = 0;j<target_id[i].size();j++){
            //std::cout << synapse->GetNoNeuronsPost() << " -- ";
            if(target_id[i].at(j) >= synapse->GetNoNeuronsPost()){
                std::cout << synapse->GetIdStr() << " - ";
                std::cout << std::to_string(target_id[i].at(j)) << "\n";
                return;
            }
        }
    }
    std::cout << synapse->GetIdStr() << " - Test OK \n";
}

void  Connectivity::SetSeed(std::default_random_engine *seedGenerator)  {
    if(!fixSeed){
        std::uniform_int_distribution<int> distribution(0,INT32_MAX);
        SetSeed(distribution(*seedGenerator));
    }
}

void  Connectivity::SetSeed(int s){
    if(!fixSeed){
        seed       = s;
        generator  = std::default_random_engine(seed);
    }
}

void Connectivity::LoadParameters(std::vector<std::string> *input){
    std::string              name;
    std::vector<std::string> values;

    for(std::vector<std::string>::iterator it = (*input).begin(); it != (*input).end(); ++it) {
        SplitString(&(*it),&name,&values);
        if(name.find("seed") != std::string::npos){
            SetSeed(std::stoi(values.at(0)));
            fixSeed = true;
        }
    }

}

void Connectivity::WriteConnectivity(std::string filename,unsigned long noNeuronsConnectivity){

    int count;
    unsigned long i;
    unsigned long noTargetNeurons = synapse->GetNoNeuronsPost();
    unsigned long noSourceNeurons = synapse->GetNoNeuronsPre();

    if(noNeuronsConnectivity <= 0)
        return;

	std::ofstream    stream;
    stream.open(filename + ".txt", std::ofstream::out | std::ofstream::app); //ios::binary


    if(noTargetNeurons > noNeuronsConnectivity)
        noTargetNeurons = noNeuronsConnectivity;

    if(noSourceNeurons > noNeuronsConnectivity)
        noSourceNeurons = noNeuronsConnectivity;

    //iterate through source & target neurons & write connectivity
    for(unsigned long source = 0;source < noSourceNeurons;source++){

        i = 0;
        for(unsigned long target = 0;target<noTargetNeurons;target++){
			if (target_id[source].size() <= i || target_id[source][i] != target)
				stream << "0 ";
			else {
				count = 1;
				i++;
				while (target_id[source].size() > i && target_id[source][i] == target) {
					i++;
					count++;
				}
				stream << std::to_string(count)<<" ";
			}
        }
        stream << "\n";

        /*stream << "Targets for neuron " << source << ":" << target_id[source].size() << " -- ";
        for(auto const& target: (target_id[source])){
            stream << std::to_string(target) << " ";
        }
        stream << "\n";*/
    }
    stream.close();

}

int * Connectivity::GetDistributionD(long preNeuronId, long postNeuronId){
	if (HasDdistribution)
		return &D_distribution[preNeuronId][postNeuronId];
	else {
		return (synapse->GetDpointer());
	}

}

void Connectivity::SetDistributionD(){
    std::uniform_int_distribution<int> uni_distribution(this->synapse->GetMinD(),this->synapse->GetMaxD());
	unsigned long noTarget;

	if (synapse->GetMaxD()== synapse->GetMinD()) {
		HasDdistribution = false;
		//return;
	}
    //std::cout << "printing D_distribution: \n";
    for(unsigned long source_neuron = 0; source_neuron < this->synapse->GetNoNeuronsPre(); source_neuron++){
        if(target_id[source_neuron].size()!=0){
			noTarget = (target_id[source_neuron]).size();
			D_distribution[source_neuron].resize(noTarget);
			for (unsigned long target_neuron = 0; target_neuron < noTarget; target_neuron++) {
                int d = uni_distribution(generator);
                D_distribution[source_neuron][target_neuron]=d;
                //std::cout << std::to_string(d) << " ";
            }
            //std::cout << "\n";
        }
    }
}

double * Connectivity::GetDistributionJ(long preNeuronId, long postNeuronId)
{
	if (HasPot)
		return &J_distribution[preNeuronId][postNeuronId];
	else {
		return (synapse->GetJpointer());
	}

}

void Connectivity::SetDistributionJ(){
    std::uniform_real_distribution<double> real_distribution(0,1);
	unsigned long noTarget;

	if (synapse->GetPPot() == 0 || this->synapse->GetJPot() == this->synapse->GetJBase()) {
		HasPot = false;
		return;
	}
    //std::cout << "printing J_distribution: \n";
    for(unsigned long source_neuron = 0; source_neuron < this->synapse->GetNoNeuronsPre(); source_neuron++){
		noTarget = (target_id[source_neuron]).size();
		J_distribution[source_neuron].resize(noTarget);
		//std::cout <<  "\n";
        for(unsigned long target_neuron = 0; target_neuron < noTarget; target_neuron++){
			double j = real_distribution(generator);
            double jj = this->synapse->GetPPot();
            if(j < jj){
				J_distribution[source_neuron][target_neuron] = (this->synapse->GetJPot());
                //std::cout << std::to_string(this->synapse->GetJPot()) << " ";
            }
            else{
				J_distribution[source_neuron][target_neuron] = (this->synapse->GetJBase());
	            //std::cout << std::to_string(this->synapse->GetJBase()) << " ";
            }
		}
		//std::cout << "\n";
    }
}


void Connectivity::WriteDistributionD(std::string filename,unsigned long noNeuronsDelay){

    int count;
    unsigned long i;
    double delay;
    unsigned long noTargetNeurons = synapse->GetNoNeuronsPost();
    unsigned long noSourceNeurons = synapse->GetNoNeuronsPre();

    if(noNeuronsDelay <= 0)
        return;

    std::ofstream    stream;
    stream.open(filename + ".txt", std::ofstream::out | std::ofstream::app); //ios::binary

    if(noTargetNeurons > noNeuronsDelay)
        noTargetNeurons = noNeuronsDelay;

    if(noSourceNeurons > noNeuronsDelay)
        noSourceNeurons = noNeuronsDelay;

    //iterate through source & target neurons & write connectivity
    for(unsigned long source = 0;source < noSourceNeurons;source++){

        i = 0; // counts the number of connections that have been written for this source neuron
        for(unsigned long target = 0;target<noTargetNeurons;target++){
			if (target_id[source].size() <= i || target_id[source][i] != target) // if all connections have been written OR target id does not match
				stream << "nan ";
			else {
				count = 1;
				if (HasDdistribution) {
					delay = D_distribution[source][i];
				}
				else {
					delay = synapse->GetMaxD();
				}
				i++;
				while (target_id[source].size() > i && target_id[source][i] == target) { // while connections are still to be written and target id matches
					i++;
					count++;
					if (HasDdistribution) {
						delay += D_distribution[source][i];
					}
					else {
						delay += synapse->GetMaxD();
					}
				}
				stream << std::to_string(delay/count)<<" ";
			}
        }
        stream << "\n";

        /*stream << "Targets for neuron " << source << ":" << target_id[source].size() << " -- ";
        for(auto const& target: (target_id[source])){
            stream << std::to_string(target) << " ";
        }
        stream << "\n";*/
    }
    stream.close();

}


void Connectivity::WriteDistributionJ(std::string filename,unsigned long noNeuronsJPot){

    int count;
    unsigned long i;
    double J;
    unsigned long noTargetNeurons = synapse->GetNoNeuronsPost();
    unsigned long noSourceNeurons = synapse->GetNoNeuronsPre();

    if(noNeuronsJPot <= 0)
        return;

    std::ofstream    stream;
    stream.open(filename + ".txt", std::ofstream::out | std::ofstream::app); //ios::binary

    if(noTargetNeurons > noNeuronsJPot)
        noTargetNeurons = noNeuronsJPot;

    if(noSourceNeurons > noNeuronsJPot)
        noSourceNeurons = noNeuronsJPot;

    //iterate through source & target neurons & write connectivity
    for(unsigned long source = 0;source < noSourceNeurons;source++){
        i = 0; // counts the number of connections that have been written for this source neuron
        for(unsigned long target = 0;target<noTargetNeurons;target++){
			if (target_id[source].size() <= i || target_id[source][i] != target) // if all connections have been written OR target id does not match
				stream << "nan ";
			else {
				count = 1;
				if (HasPot) {
					J = J_distribution[source][i];
				}
				else {
					J = synapse->GetJBase();
				}
				i++;
				while (target_id[source].size() > i && target_id[source][i] == target) { // while connections are still to be written and target id matches
					i++;
					count++;
					if (HasPot) {
						J += J_distribution[source][i];
					}
					else {
						J += synapse->GetJBase();
					}
				}
				stream << std::to_string(J/count)<<" ";
			}
        }
        stream << "\n";

        /*stream << "Targets for neuron " << source << ":" << target_id[source].size() << " -- ";
        for(auto const& target: (target_id[source])){
            stream << std::to_string(target) << " ";
        }
        stream << "\n";*/
    }
    stream.close();

}
