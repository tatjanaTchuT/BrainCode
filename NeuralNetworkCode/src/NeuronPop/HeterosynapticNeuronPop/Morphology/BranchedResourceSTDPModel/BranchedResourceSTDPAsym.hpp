#ifndef _SIMPLE_PLASTICITY_ONLY_BRANCHES_
#define _SIMPLE_PLASTICITY_ONLY_BRANCHES_

#include "../BranchedMorphology.hpp"
#include "../SynapseSpines/ResourceSynapseSpine.hpp"
#include <numeric>
#include <unordered_map>

class BranchedMorphology;

class BranchedResourceSTDPAsymmetric : public BranchedMorphology {
//This class models a behaviour based on wi=beta*(alfai/(omega+sum(alfai))), where alfai represents the spine's resources as (Ks*expdt+Kbasal)/(Ns*expdt+Nbasal) with bumps on Ks and Ns
protected:
    //Branch variables
    size_t timestepWindowSize{};
    //Space-time kernel
    std::unordered_map<std::pair<int, int>, double> kernelHashTable;//This is more efficient than the map, but we need a hash function
    // double baseKStimmulusBump{1.0};//LP and SP
    // double baseNStimmulusBump{1.0};//LP and SP
    double baseAlphaStimmulusBump{1.0};//LP and SP
    //Should the constants be split for N and K 
    double timeKernelConstant{1.0};//Strong decay needs small constants//LP and SP
    double spaceKernelConstant{1.0 };//LP and SP
    double expDecayTime{1.0};//Have to depend on dt and gap!
    double expDecaySpace{1.0};

    int kernelGapNumber{10}; //Number of gaps in the max kernel distance //LP and SP

    //STDP and STDD parameters (MAKE SURE TO TUNE STDD, AS NEGATIVE WEIGHTS MUST BE AVOIDED)
    double PotentiationDepressionRatio{1.05};//Diagnose later negative weights //LP and SP
    //Counters
    int STDPDepressionCount{};
    int maxCount{100}; //dependent on dt?, default 10 ms assuming dt=1e-4 //LP and SP
    int& maxCountSTDPPotentiation = maxCount;//This one is the lasting count for th spines //LP and SP?
    int& maxCountTrigger = maxCount;//LP and SP?
    int& maxCountSTDPDepression = maxCount;//LP and SP?
    std::vector<std::shared_ptr<ResourceSynapseSpine>> synapseDataResource;
    std::vector<std::shared_ptr<ResourceBranch>> resourceBranches;
public:

    //main Methods
    BranchedResourceSTDPAsymmetric()=default;
    explicit BranchedResourceSTDPAsymmetric(GlobalSimInfo * info);
    ~BranchedResourceSTDPAsymmetric() = default;
    
    void LoadParameters(std::vector<std::string> *input) override;//Remember to set all counts to maxCount    
    void SaveParameters(std::ofstream * stream, std::string neuronPreId) override;

    void SetUpBranchings(int remainingBranchingEvents, std::vector<int> anteriorBranches = std::vector<int>()) override;// Here we set up the vector with the branches

    const std::string GetType() override {return str_BranchedResourceSTDPAsymmetric;};

    void SetUpKernelHashTable(); //Has to set up both time and space from the exp constants
    
    //Advect methods
    void advect() override;
    void STDPPotentiation();
    void STDPDepression();
    void DetectPairing(int synapseSpineIDinMorpho);
    void SetEffect();//Here we calculate and store the effects of pairing (no matter the post state)
    double SpaceTimeKernel(int synapseID);
    double CallKernelHashTable(int distanceToCenter, int timeDifference);
    void TickCounts();
    void ApplyEffects();//Here we increase the plasticity count of synapse and branch
    void DeleteEffects();//Here, if counter==countMax, erase in that index the element of every vector (first store index, then REVERSE remove the removelist indexes with .rbegin and .rend instead of .begin and .end)
        //Container should be ordered by definition, but std::sort(array.begin(), array.end()) would ensure so.
    void Reset() override; //Wrapper plus clearing some of the vectors. Last Reset method to run in chronological order, where we call the ticks and the general upkeep
    //These methods have to be done per branch
    void UpdateAlphaSum(std::shared_ptr<ResourceBranch> branch);//Run in LP
    void RecalcAlphas(std::shared_ptr<ResourceBranch> branch);//Run in LP
    void RecalcWeights(std::shared_ptr<ResourceBranch> branch);//Run in LP
    void RecalcAlphaSums(std::shared_ptr<ResourceBranch> branch);//Called inside recalc weights
    //Record methods
    void RecordPostSpike() override;
    void RecordExcitatoryPreSpike(unsigned long synSpikerId) override;//Here set the trigger count to 0

    //Getters
    std::valarray<double> GetIndividualSynapticProfile(unsigned long synapseId) const override;//Remember plasticity events per synapse and something else

    //Allocation methods
    std::shared_ptr<BaseSynapseSpine> AllocateNewSynapse(HeteroCurrentSynapse& synapse) override; //Call the Branched one inside before setting all counters
        //Remember to set all counts to maxCount    
    //For debugging purposes
    void WeightDecay() override {throw;};//This function should never be called 
    void NormalizeWeights() override {return;};
};

#endif