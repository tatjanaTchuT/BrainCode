#ifndef _SYNAPSE_SPINE_BRANCHED_CLASS_HEADER_
#define _SYNAPSE_SPINE_BRANCHED_CLASS_HEADER_

#include "BaseSynapseSpine.hpp"

class BranchedSynapseSpine : public BaseSynapseSpine {

    protected:
    int branchId{}; //This has to be discrete
    int branchPositionId{}; //This has to be discrete
    int distanceFromNode{}; //This will probably be discrete too, as the distance is id*gap, and gap will be 1 um.
    //int uniqueTreeId{}; Useful only in the indexing of the triangular matrix if implemented
    bool isBranchedBool{true};

    public:
    BranchedSynapseSpine()=default;
    //BranchedSynapseSpine(int distanceFromNode, double lastSpike, double weight, int branchId, int branchPositionId);
    //getters
    int GetBranchId() const {return branchId;};
    int GetBranchPositionId() const {return branchPositionId;};
    int GetDistanceFromNode() const {return distanceFromNode;};
    bool GetBranchedBool() const { return isBranchedBool;}
    //setters
    void SetBranchId(int idIn){branchId=idIn;};
    void SetBranchPositionId(int positionId){branchPositionId=positionId;};
    void SetDistanceFromNode(int distance){distanceFromNode=distance;};

    //Profile function
    std::valarray<double> GetIndividualSynapticProfile() const override;
    std::string GetIndividualSynapticProfileHeaderInfo() const override;
};

#endif