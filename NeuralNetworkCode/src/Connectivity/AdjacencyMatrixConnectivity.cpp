#include "AdjacencyMatrixConnectivity.hpp"
#include "../Synapse/Synapse.hpp"

AdjacencyMatrixConnectivity::AdjacencyMatrixConnectivity(Synapse* syn, GlobalSimInfo* info) :Connectivity(syn, info) {
    noSourceNeurons = 0;
    SetSeed(0);

    unsigned long    nPost = syn->GetNoNeuronsPost();
    unsigned long    nPre = syn->GetNoNeuronsPre();

    std::string idStr = syn->GetIdStrWithULine();

    connectivity_matrix.resize(nPost);
    for (unsigned long i = 0; i < nPost; i++)
        connectivity_matrix[i].resize(nPre);

    for (unsigned long target = 0; target < nPost; target++) {
        for (unsigned long source = 0; source < nPre; source++) {
            connectivity_matrix[target][source] = 0;
        }
    }
    connectionProbFile = info->pathTo_inputFile + "Connectivity_Matrix_"+idStr+".txt";
}

void AdjacencyMatrixConnectivity::SaveParameters(std::ofstream* stream, std::string id_str) {
    Connectivity::SaveParameters(stream, id_str);
    //*stream << id_str << "connectivity_noSourceNeurons " << std::to_string(this->noSourceNeurons) << "\n";
    //*stream << id_str << "connectivity_ConnectionProba\t" << std::to_string(this->GetConnectionProbability()) << "\n";
    //*stream << "#" << id_str << "connectivity_noSourceNeurons " << std::to_string(this->noSourceNeurons) << "\n";
    *stream << "#\t\t" << str_adjacencyMatrixConnectivity << ": Pre and post population has adjacency matrix.\n";
}

void AdjacencyMatrixConnectivity::LoadParameters(std::vector<std::string>* input) {
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

void AdjacencyMatrixConnectivity::SetNoSourceNeurons(unsigned long noSN) {
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

double AdjacencyMatrixConnectivity::GetConnectionProbability() {
    if (synapse->GetNoNeuronsPre() == 0)
        return 0;
    else
        return static_cast<double>(noSourceNeurons) / static_cast<double>(synapse->GetNoNeuronsPre());
}

void AdjacencyMatrixConnectivity::GetConnectionWeightsFromFile(std::string filepath) {

    std::vector<std::string> values;
    std::string str_line;

    std::ifstream stream(filepath);
    std::cout << filepath << std::endl;

    unsigned long source = 0;
    unsigned long    nPre = synapse->GetNoNeuronsPre();
    unsigned long    nPost = synapse->GetNoNeuronsPost();

    std::vector<std::vector<double>> tmp_matrix;
    try
    {
        bool tooManyColumnException = false;

        while (std::getline(stream, str_line))
        {
            std::vector<std::string> values;
            SplitString(&str_line, &values);

            if (source >= nPre) {
                throw 4;
            }
            if (values.size() < nPost) {
                throw 1;
            }
            if (values.size() > nPost) {
                tooManyColumnException = true;
            }

            for (std::vector<int>::size_type target = 0; target != nPost; target++) {
                connectivity_matrix[target][source] = std::stoi(values.at(target));
            }

            source++;
        }
        if (source < nPre) {
            throw 2;
        }
        if (tooManyColumnException) {
            throw 3;
        }
    }
    catch (int exception)
    {
        stream.close();
        if (exception == 1) {
            std::cout << "*************************\n";
            std::cout << "Not enough columns of synaptic connection file!\n";
            std::cout << "*************************\n";
            exit(1);
        }
        else if (exception == 2) {
            std::cout << "*************************\n";
            std::cout << "Not enough rows of synaptic connection file!\n";
            std::cout << "*************************\n";
            exit(1);
        }
        else if (exception == 3) {
            std::cout << "*************************\n";
            std::cout << "Warning: Too many columns of synaptic connection file\n";
            std::cout << "*************************\n";
        }
        else if (exception == 4) {
            std::cout << "*************************\n";
            std::cout << "Warning: Too many rows of synaptic connection file\n";
            std::cout << "*************************\n";
        }
    }

    stream.close();
}

void AdjacencyMatrixConnectivity::ConnectNeurons()
{

    unsigned long    countedSourceNeurons;
    unsigned long    nPost = synapse->GetNoNeuronsPost();
    unsigned long    nPre = synapse->GetNoNeuronsPre();

    long    output_Interval = nPost / 10;
    if (output_Interval == 0)
        output_Interval = 1;

    countedSourceNeurons = 0;

    //Iterate through all target neurons
    for (unsigned long target = 0; target < nPost; target++) {
        for (unsigned long source = 0; source < nPre; source++) {

            int connection = connectivity_matrix[target][source];

            if (connection != 0) {
                target_id[source].push_back(target);
                countedSourceNeurons++;
            }
        }

        if ((target) % output_Interval == 0)
            std::cout << 100 * (target) / nPost << "%-";
    }
    std::cout << "100%\n";
    SetNoSourceNeurons(int(countedSourceNeurons / nPost));
}
