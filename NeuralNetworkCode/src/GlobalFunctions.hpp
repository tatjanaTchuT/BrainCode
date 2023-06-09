//
//  GlobalFunctions.h
//  NeuralNetworkCode
//
//  Created by Andreas Nold on 13/01/2017.
//  Copyright © 2017 Andreas Nold. All rights reserved.
//

#ifndef GlobalFunctions_h
#define GlobalFunctions_h


#include <random>
#include <fstream>
#include <assert.h>
#include <valarray>
#include <numeric>
#include <deque>

#define assertm(exp, msg) assert(((void)msg, exp))

struct GlobalSimInfo {

    std::default_random_engine globalGenerator{};
    std::string pathTo_inputFile{};
    int		globalSeed{};
    long    time_step{};//long is long int
    int     waiting_index{};
    double  dt{};
    int     density{};
    double  Lx{}; //Always a positive quantity (number of neurons/dimensions)
    double  Ly{}; //Always a positive quantity (number of neurons/dimensions)
    int     Dimensions{};
    double  simulationTime{};
    double  networkScaling_synStrength{};
    int     networkScaling_extern{}; // 0: default (=1)
    long    N{};
} ;

struct FileEntry {
    std::string name;
    std::vector<std::string> values;

    FileEntry(std::string n, std::vector<std::string> v) : name(std::move(n)), values(std::move(v)) {
    }
    FileEntry(){}
};

struct IterableFileEntry : FileEntry {
    std::string iterate_id;
    IterableFileEntry(std::string it_id, std::string n, std::vector<std::string> v) :  FileEntry(n, v), iterate_id(it_id){
    }
};



struct RecorderOpenStreams {
    std::ofstream heatmapFileStream;
    std::ofstream averagesFileStream;
    std::ofstream rasterplotFileStream;
    std::ofstream potentialFileStream;
    std::ofstream currentsFileStream;
    std::ofstream cCurrentsFileStream;
    std::ofstream histogramFileStream;
    std::ofstream meanCorrFileStream;
    std::ofstream pairCorrFileStream;
    std::ofstream synStatesFileStream;
    std::ofstream heteroSynapsesFileStream;
    std::ofstream hSOverallFileStream;
    std::ofstream heteroBSynapsesFileStream;
    //std::vector<std::ofstream> neuronOuputFileStreams;
};

struct SubRegion{
    const char regionID;
    const std::vector<int> branchesInRegion; //I will have to read this from the morphology LP, every subregion is a line, first input is ID, rest is branchIDs. Then in Synapse you put the subregion where the synapse goes. 
    SubRegion(char regionID, std::vector<int> branchesInRegion);
};

struct BranchTargeting{
    int targetBranch{};
    bool setTargetBranch{false};
    bool randomTargetBranch{false};
    bool orderedTargetBranch{false};
    char subRegion{'0'};
};

struct Branch{
    //ID
    const int branchId{};
    //For setup
    const std::vector<int> anteriorBranches{}; 

    //For now these are identical to the morphology ones, but we will see in the future
    const int synapticGap{};
    const int branchLength{};

    std::deque<int> openSynapsesSlots{};//We will pop_back() from here the position of the synapse being setup. Will push_back() in setup, so no default size
    //The idea is to .pop_front() ids that have been ordered inside here
    //For the actual checks
    std::vector<bool> spikedSyn{};//Here, with the size of the branch discrete positions, we will store the bool indicating if the preneuron fired in the timestep
    //Because of how the kernelized version of plasticity works, I will  use the branch ID and relative position ID from the SynapseExtBranched, so the next two vectors are unnecessary
    std::vector<int> synapseSlotClosedIndex{}; // Here we store the used indexes in the vector, to check for stuff faster and only go to indexes
    std::vector<int> morphoSynapseIDs{};// REMEMBER TO PUT IN CONSTRUCTOR Unique ID for the poisition in the dendritic tree. Probably going to be for allocation only
    //Maybe more vectors will be needed. Not in use commented out. 
    Branch()=default;
    Branch(int gap, int branchLength, std::vector<int>anteriorBranches, int branchId);
};

const std::string str_adjacencyMatrixConnectivity {"AdjacencyMatrixConnectivity"};
const std::string str_randomConnectivity{"RandomConnectivity"};
const std::string str_binaryrandomConnectivity{"BinaryRandomConnectivity"};
const std::string str_distanceConnectivity{"DistanceConnectivity"};
const std::string str_heteroRandomConnectivity{"HeteroRandomConnectivity"};
//const std::string str_localConnectivity("LocalConnectivity");


const std::string str_currentSynapse{"CurrentSynapse"};
const std::string str_conductanceSynapse{"ConductanceSynapse"};
const std::string str_mongilloSynapse{"MongilloSynapse"};
const std::string str_mongilloSynapseContinuous{"MongilloSynapseContinuous"};
const std::string str_probabilisticCurrentSynapse{"ProbabilisticCurrentSynapse"};
const std::string str_prgSynapseContinuous{"PRGSynapseContinuous"};

const std::string str_exponentialCurrentSynapse{"ExponentialCurrentSynapse"};
const std::string str_powerlawsynapse{"PowerLawSynapse"};
const std::string str_exponentialConductanceSynapse{"ExponentialConductanceSynapse"};
const std::string str_exponentialMongilloSynapse{"ExponentialMongilloSynapse"};
const std::string str_heteroSynapse{"HeteroCurrentSynapse"};

const std::string str_exponentialSynapseAddon{"ExponentialSynapseAddon"};

const std::string str_LIFNeuron{"LIFNeuron"};
const std::string str_QIFNeuron{"QIFNeuron"};
const std::string str_EIFNeuron{"EIFNeuron"};
const std::string str_PoissonNeuron{"PoissonNeuron"};
const std::string str_InputNeuron{"InputNeuron"};
const std::string str_HeteroLIFNeuron{"HeteroLIFNeuron"};
const std::string str_HeteroPoissonNeuron{"HeteroPoissonNeuron"};

const std::string str_NOPNormalization{"NOPNormalization"};
const std::string str_HardNormalization{"HardNormalization"};
const std::string str_SoftMaxNormalization{"SoftMaxNormalization"};

const std::string str_leanRecorder{"LeanRecorder"};
const std::string str_advancedRecorder{"AdvancedRecorder"};

const std::string str_uncorrelatedStimulus{"UncorrelatedStimulus"};
const std::string str_whitenoiseStimulus{"WhiteNoiseStimulus"};
const std::string str_whitenoiseRescaled{"WhiteNoiseRescaled"};
const std::string str_Tims_sin_Stimulus{"TimsSinStimulus"};
const std::string str_whitenoiseLinear{"WhiteNoiseLinear"};
const std::string str_spatialgaussianStimulus{"SpatialGaussianStimulus"};
const std::string str_spatialpoissonStimulus{"SpatialPoissonStimulus"};

const std::string str_MonoDendriteSTDPTazerart{"MonoDendriteSTDPTazerart"};
const std::string str_MonoDendriteSTDPTazerartRelative{"MonoDendriteSTDPTazerartRelative"};
const std::string str_MonoDendriteSTDPBiWindow{"MonoDendriteSTDPBiWindow"};
const std::string str_MonoDendriteSTDPBiExponential{"MonoDendriteSTDPBiExponential"};

const std::string str_SimplePlasticityOnlyBranches{"SimplePlasticityOnlyBranches"};

void multiply_vector (std::vector<unsigned long> &vector, unsigned long value);
void multiply_vector (std::vector<double> &vector, double value);

void TestWritingFile(std::string filename);

void FilterStringEntries(std::vector<FileEntry> *str_full,std::string token,std::vector<FileEntry> *str_filtered);
void FilterStringVector(std::vector<std::string> *str_full,std::string token,std::vector<std::string> *str_filtered);
void SplitString(std::string *full_str,std::string *name,std::vector<std::string> *values);
void SplitString(std::string *full_str, std::string *iterate_id, std::string *name, std::vector<std::string> *values);
void SplitString(std::string* full_str, std::vector<std::string>* values);

std::string getPathToInputFile(std::string* inputFile, bool Windows);

void SaveDoubleFile(std::ofstream *file,double val,int precision);
void SaveTupleOfDoublesFile(std::ofstream *file, std::valarray<double>, int precision);

bool is_double(const std::string& s);

FileEntry stringToFileEntry(std::string);
IterableFileEntry stringToIterableFileEntry(std::string);

void checkConsistencyOfIterationParameters(const std::vector<IterableFileEntry>&);

struct noAllocatableSynapseException : std::exception {
    char const* what() const noexcept override {
        return "No synapses available on dendrite for new allocation.";
    }
};

void RemoveCommentInString(std::vector<std::string>* string, char commentCharacter='#');
//int INTMAX = 2;
//double PI = 3.14159265359;

#endif /* GlobalFunctions_h */
