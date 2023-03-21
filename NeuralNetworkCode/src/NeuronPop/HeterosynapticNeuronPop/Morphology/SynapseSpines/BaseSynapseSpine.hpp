#ifndef _SYNAPSE_SPINE_CLASS_HEADER
#define _SYNAPSE_SPINE_CLASS_HEADER

#include <valarray>
#include <string>


class BaseSynapseSpine {

    protected:
    //Legacy variables
    unsigned long preNeuronId{};
    unsigned long postNeuronId{};
    double weight{}; //The negative weight comes from J, weight is just a factor to multiply (for now)
    double lastSpike{};
    unsigned long idInMorpho{}; // id for synapse within its population
    unsigned long idInHCS{}; // id for synapse within its a synapse collection
    //Branched variables
    bool isBranchedBool{false};// For now it is not useful

    //Counters
    long plasticityEvents{};
    public:
    //Constructors
    BaseSynapseSpine()=default;
    //BaseSynapseSpine(double weight, double lastSpike);// Not currently in use
    //Methods
    //Getters
    unsigned long GetPreNeuronId() const {return preNeuronId;};
    unsigned long GetPostNeuronId() const {return postNeuronId;};
    double GetWeight() const {return weight;};
    unsigned long GetIdInMorpho() const {return idInMorpho;};
    unsigned long GetIdInHCS() const {return idInHCS;};
    bool GetBranchedBool() const { return isBranchedBool;}
    double GetLastSpike() const {return lastSpike;};
    long GetPlasticityEvents() const {return plasticityEvents;}
    //Setters
    void SetPreNeuronId(unsigned long neuronId){preNeuronId=neuronId;};
    void SetPostNeuronId(unsigned long neuronId){postNeuronId=neuronId;};
    void SetWeight(double weightIn){weight=weightIn;};
    void SetIdInMorpho(unsigned long idIn){idInMorpho=idIn;};
    void SetIdInHCS(unsigned long idIn){idInHCS=idIn;};
    void SetLastSpike(double time){lastSpike=time;};
    //Misc
    void AddToWeight(double change){weight+=change;}

    //Recorder functions
    virtual std::valarray<double> GetIndividualSynapticProfile() const = 0;
    virtual std::string GetIndividualSynapticProfileHeaderInfo() const = 0;
};

#endif