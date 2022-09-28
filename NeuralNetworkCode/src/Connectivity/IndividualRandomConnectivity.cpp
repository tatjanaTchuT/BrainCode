#include "IndividualRandomConnectivity.hpp"
#include "../Synapse/Synapse.hpp"

IndividualRandomConnectivity::IndividualRandomConnectivity(Synapse* syn, GlobalSimInfo* info) :Connectivity(syn, info) {
    noSourceNeurons = 0;
    SetSeed(0);

    unsigned long    nPost = syn->GetNoNeuronsPost();
    unsigned long    nPre = syn->GetNoNeuronsPre();

    std::string idStr = syn->GetIdStr();

    connectivity_matrix.resize(nPost);
    for (unsigned long i = 0; i < nPost; i++)
        connectivity_matrix[i].resize(nPre);

    for (unsigned long target = 0; target < nPost; target++) {
        for (unsigned long source = 0; source < nPre; source++) {
            connectivity_matrix[target][source] = 0;
        }
    }
    connectionProbFile = info->pathTo_inputFile + "synapses_connection_"+idStr+".txt";
}

void IndividualRandomConnectivity::SaveParameters(std::ofstream* stream, std::string id_str) {
    Connectivity::SaveParameters(stream, id_str);
    //*stream << id_str << "connectivity_noSourceNeurons " << std::to_string(this->noSourceNeurons) << "\n";
    //*stream << id_str << "connectivity_ConnectionProba\t" << std::to_string(this->GetConnectionProbability()) << "\n";
    //*stream << "#" << id_str << "connectivity_noSourceNeurons " << std::to_string(this->noSourceNeurons) << "\n";
    *stream << "#\t\t" << str_individualRandomConnectivity << ": Each pre and post neurons have individual connection probability.\n";
}

void IndividualRandomConnectivity::LoadParameters(std::vector<std::string>* input) {
    Connectivity::LoadParameters(input);

    std::string              name;
    std::vector<std::string> values;
    
    for (std::vector<std::string>::iterator it = (*input).begin(); it != (*input).end(); ++it) {
        SplitString(&(*it), &name, &values);
        if (name.find("seed") != std::string::npos) {
            SetSeed(std::stoi(values.at(0)));
        }
        else if (name.find("noSourceNeurons") != std::string::npos) {
            SetNoSourceNeurons(std::stoi(values.at(0)));
        }

    }
    GetConnectionWeightsFromFile(connectionProbFile);
}

void IndividualRandomConnectivity::SetNoSourceNeurons(unsigned long noSN) {
    if (noSN < 0)
        noSourceNeurons = 0;
    else if (noSN > synapse->GetNoNeuronsPre())
        noSourceNeurons = synapse->GetNoNeuronsPre();
    else if ((noSN == synapse->GetNoNeuronsPre()) && (synapse->IsRecurrent()))
        //noSourceNeurons = 0;
        noSourceNeurons = synapse->GetNoNeuronsPre() - 1;
    else
        noSourceNeurons = noSN;

}

double IndividualRandomConnectivity::GetConnectionProbability() {
    if (synapse->GetNoNeuronsPre() == 0)
        return 0;
    else
        return ((double)noSourceNeurons) / ((double)synapse->GetNoNeuronsPre());
}

void IndividualRandomConnectivity::GetConnectionWeightsFromFile(std::string filepath) {

    char line[8192];
    std::vector<std::string> full_strs, values;
    std::string str_line;

    std::ifstream stream(filepath);
    std::cout << filepath << std::endl;

    std::string prefix;
    unsigned long source = 0;
    unsigned long    nPre = synapse->GetNoNeuronsPre();
    unsigned long    nPost = synapse->GetNoNeuronsPost();

    std::vector<std::vector<double>> tmp_matrix;

    while (stream.getline(line, 8192)) {
        if (line[0] == '#')
            continue;

        full_strs.push_back(line);
        str_line = line;

        std::vector<std::string> values;
        SplitString(&str_line, &values);

        for (unsigned long target = 0; target < nPost; target++) {
            connectivity_matrix[target][source] = std::stoi(values.at(target));
        }

        source++;
    }
    stream.close();
}

void IndividualRandomConnectivity::SetDistributionJ() {

    char line[2048];
    std::vector<std::string> full_strs, values;
    std::string str_line;

    std::ifstream stream(connectionProbFile);
    std::cout << connectionProbFile << std::endl;

    std::string prefix;
    while (stream.getline(line, 256)) {
        if (line[0] == '#')
            continue;

        full_strs.push_back(line);
        str_line = line;

        std::vector<std::string> values;
        SplitString(&str_line, &values);

        int pre_synaptic_index = std::stoi(values.at(0));
        int post_synaptic_index = std::stoi(values.at(1));
        double probability = std::stod(values.at(2));
        double synapticStr = std::stod(values.at(3));

        J_distribution[pre_synaptic_index][post_synaptic_index] = synapticStr;
    }
    stream.close();
}
void IndividualRandomConnectivity::ConnectNeurons()
{

    unsigned long    source, countedSourceNeurons;
    unsigned long    nPost = synapse->GetNoNeuronsPost();
    unsigned long    nPre = synapse->GetNoNeuronsPre();

    long    output_Interval = nPost / 10;
    if (output_Interval == 0)
        output_Interval = 1;

    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    countedSourceNeurons = 0;

    //Iterate through all target neurons
    for (unsigned long target = 0; target < nPost; target++) {
        for (unsigned long source = 0; source < nPre; source++) {

            int connection = connectivity_matrix[target][source];

            if (connection == 1) {
                target_id[source].push_back(target);
                countedSourceNeurons++;
            }
        }

        if ((target) % output_Interval == 0)
            std::cout << 100 * (target) / nPost << "%-";
    }
    std::cout << "100%\n";
    SetNoSourceNeurons(int(countedSourceNeurons/nPost));
}
