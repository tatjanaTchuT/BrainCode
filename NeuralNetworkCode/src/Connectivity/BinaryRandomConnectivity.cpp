#include "BinaryRandomConnectivity.hpp"
#include "../Synapse/Synapse.hpp"

BinaryRandomConnectivity::BinaryRandomConnectivity(Synapse * syn,GlobalSimInfo  * info):Connectivity(syn,info){
    connectionProbability = 0;
    SetSeed(0);
}

void BinaryRandomConnectivity::SaveParameters(std::ofstream * stream, const std::string& id_str){
    Connectivity::SaveParameters(stream,id_str);
    *stream << id_str << "connectivity_ConnectionProba\t" << std::to_string(this->connectionProbability) << "\n";
    if(info->globalSeed == -1){
    *stream << id_str << "connectivity_seed                  " << std::to_string(this->seed)  << "\n";
    }
    *stream << "#\t\t"<< str_binaryrandomConnectivity << ": Erdoes Renyi network. Each neuronal pair is connected with probability connectionProbability. (as used by [Renart et al. (2010)]). \n";
    //*stream << "# "<< str_binaryrandomConnectivity << "as used by [Renart et al. (2010)] \n";
}

void BinaryRandomConnectivity::LoadParameters(std::vector<std::string> *input){
    Connectivity::LoadParameters(input);

    std::string              name;
    std::vector<std::string> values;

    for(auto & it : *input) {
        SplitString(&it,&name,&values);
        if(name.find("seed") != std::string::npos){
            SetSeed(std::stoi(values.at(0)));
        }
        else if (name.find("ConnectionProba") != std::string::npos || name.find("connectionProbability") != std::string::npos) {
            this->connectionProbability = std::stod(values.at(0));
        }
    }
}

void BinaryRandomConnectivity::ConnectNeurons()
{
    double  p;
    long    noPost = synapse->GetNoNeuronsPost();
	long    noPre = synapse->GetNoNeuronsPre();

    long    output_Interval = noPost/10;
    if(output_Interval == 0)
        output_Interval = 1;

    std::uniform_real_distribution<double> distribution(0,1);


    //Iterate through all target neurons
    for(long target = 0; target < noPost; target++){

        //Iterate through all source neurons
        for(long source = 0; source < noPre; source++){

            //Connect with given probability
            p = distribution(generator);
            if(p <= connectionProbability){
                target_id[source].push_back(target);
            }
        }

        if(target%output_Interval == 0)
            std::cout << 100*(target)/noPost << "%-";
    }
    std::cout << "100%\n";
}


unsigned long BinaryRandomConnectivity::GetNumberAverageSourceNeurons(){
    return connectionProbability*this->synapse->GetNoNeuronsPre();
}
