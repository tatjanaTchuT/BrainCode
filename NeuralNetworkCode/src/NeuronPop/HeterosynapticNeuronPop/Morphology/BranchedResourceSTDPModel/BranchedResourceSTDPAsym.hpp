#ifndef _SIMPLE_PLASTICITY_ONLY_BRANCHES_
#define _SIMPLE_PLASTICITY_ONLY_BRANCHES_

#include "../BranchedMorphology.hpp"
#include "../SynapseSpines/ResourceSynapseSpine.hpp"
#include <numeric>


class BranchedMorphology;

class BranchedResourceSTDPAsymmetric : public BranchedMorphology {
//This class models a behaviour based on wi=beta*(alfai/(omega+sum(alfai))), where alfai represents the spine's resources as (Ks*expdt+Kbasal)/(Ns*expdt+Nbasal) with bumps on Ks and Ns
protected:
    //Branch variables
    size_t timestepWindowSize{};
    //Space-time kernel
    double timeKernelConstant{};//Strong decay needs small constants
    double spaceKernelConstant{};
    double expDecayTime{};//Have to depend on dt and gap!
    double expDecaySpace{};

    //STDP and STDD parameters (MAKE SURE TO TUNE STDD, AS NEGATIVE WEIGHTS MUST BE AVOIDED)
    double PotentiationDepressionRatio{1.05};//Diagnose later negative weights
    //Counters
    int STDPDepressionCount{};
    int maxCount{100}; //dependent on dt?, default 10 ms assuming dt=1e-4
    int& maxCountSTDPPotentiation = maxCount;
    int& maxCountTrigger = maxCount;
    int& maxCountSTDPDepression = maxCount;
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

    void SetUpHashTableKernel(); //Has to set up both time and space from the exp constants
    
    //Advect methods
    void advect() override;//Here I have to take into account the circular behaviour: from t=10 s=1 to s=10 t=1, sort of symmetric. (Each syn gap you move, you lose 10 dt of time)
        //If STDPDEpression count is not 10, immediately trigger STDP on spine trigger (while also flagging)
        //If first precount is bigger than twice the STDP depression count, depression. Otherwise potentiation.
    void STDPPotentiation();//If post spike, apply all stimms on positive mode.
    void STDPDepression(); //If count <10, apply stimms in negative mode.
    void DetectPairing(int synapseSpineIDinMorpho);
    void SetEffect();//Here we calculate and store the effects of pairing (no matter the post state)
    std::pair<double, double> SpaceTimeKernel(int synapseID);//Here for every synapse inside the synapse's kernel we get the time kernel and then apply the space kernel
    //Take unto account the synaptic GAP and DT!!! 
    std::pair<double, double> TimeKernel(int synapseID, int distanceToCenter);//Here we tabulate for every distance from center a equivalency between counts and effects (reverse as counts go up to maxCount)

    void TickCounts();
    void ApplyEffects();//Here we increase the plasticity count of synapse and branch
    void DeleteEffects();//Here, if counter==countMax, erase in that index the element of every vector (first store index, then REVERSE remove the removelist indexes with .rbegin and .rend instead of .begin and .end)
        //Container should be ordered by definition, but std::sort(array.begin(), array.end()) would ensure so.
    void Reset() override; //Wrapper plus clearing some of the vectors. Last Reset method to run in chronological order, where we call the ticks and the general upkeep
    //These methods have to be done per branch
    void UpdateAlphaSum(std::shared_ptr<ResourceBranch> branch);//Run in LP
    void RecalcAlphas(std::shared_ptr<ResourceBranch> branch);//Run in LP
    void RecalcWeights(std::shared_ptr<ResourceBranch> branch);//Run in LP
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