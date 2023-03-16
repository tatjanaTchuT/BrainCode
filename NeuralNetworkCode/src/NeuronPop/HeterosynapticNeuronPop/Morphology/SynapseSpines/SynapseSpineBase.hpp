#ifndef _SYNAPSE_SPINE_CLASS_HEADER
#define _SYNAPSE_SPINE_CLASS_HEADER

#include <valarray>
#include <string>


class SynapseSpineBase {

    protected:
    //Legacy variables
    unsigned long preNeuronId{};
    unsigned long postNeuronId{};
    double weight{}; //The negative weight comes from J, weight is just a factor to multiply
    double lastSpike{};
    unsigned long idInMorpho{}; // id for synapse within its population
    unsigned long idInHCS{}; // id for synapse within its a synapse collection
    //Branched variables
    bool isBranchedBool{false};// For now it is not useful

    public:
    //Constructors
    SynapseSpineBase()=default;
    SynapseSpineBase(double weight, double lastSpike);;
    //Methods
    //Bool checks
    //bool IsBranchedBool() const  {return IsBranched;}
    //bool SetBranchedTrue() {IsBranched=true;}
    //Getters
    unsigned long getPreNeuronId() const {return preNeuronId;};
    unsigned long getPostNeuronId() const {return postNeuronId;};
    double getWeight() const {return weight;};
    unsigned long getIdInMorpho() const {return idInMorpho;};
    unsigned long getIdInHCS() const {return idInHCS;};
    bool getBranchedBool() const { return isBranchedBool;}
    double getLastSpike() const {return lastSpike;};
    //Setters
    void setPreNeuronId(unsigned long neuronId){preNeuronId=neuronId;};
    void setPostNeuronId(unsigned long neuronId){postNeuronId=neuronId;};
    void setWeight(double weightIn){weight=weightIn;};
    void setIdInMorpho(unsigned long idIn){idInMorpho=idIn;};
    void setIdInHCS(unsigned long idIn){idInHCS=idIn;};
    void setLastSpike(double time){lastSpike=time;};
    //Misc
    void addToWeight(double change){weight+=change;}

    //Recorder functions
    virtual std::valarray<double> getIndividualSynapticProfile() const = 0;
    virtual std::string getIndividualSynapticProfileHeaderInfo() const = 0;
};

#endif