//
// Created by Saif Ahmed on 26.09.21.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "../../src/NeuronPop/HeteroNeuronPop.hpp"

class MockHeteroNeuronPop: public HeteroNeuronPop {
public:
    MockHeteroNeuronPop(GlobalSimInfo * info, int id): HeteroNeuronPop(info, id) {}

    MOCK_METHOD(void, advect, (std::vector<double> * synaptic_dV), (override));
    MOCK_METHOD(std::string, GetType, (), (override));
};

class HeteroNeuronPopTest : public ::testing::Test {
protected:
    HeteroNeuronPop* heteroNeuronPop{};
    GlobalSimInfo info;
    std::vector<std::string> inputs = {
            "neurons_0_noNeurons 20",
            "neurons_0_dendtric_length  4 μm",
            "neurons_0_synaptic_gap  1 μm",
            "neurons_0_heterosynaptic_theta_decay  25.0",
            "neurons_0_intersynapse_distance_decay  50.0",
            "neurons_0_intersynapse_spike_timing_decay 1.0",
            "neurons_0_mu_LTP 0.013",
            "neurons_0_sigma_LTP 0.005",
            "neurons_0_alpha_LTP 1.0",
            "neurons_0_mu_LTD 0.007",
            "neurons_0_sigma_LTD 0.005",
            "neurons_0_beta_LTD 20.0"
    };

    void SetUp() override {
        heteroNeuronPop = new MockHeteroNeuronPop(&info, 0);
        heteroNeuronPop->LoadParameters(&inputs);
    }

    void TearDown() override {
        delete heteroNeuronPop;
    }
};


TEST_F(HeteroNeuronPopTest, HeteroNeuronPostSpikeInitTest) {
    // post spikes initialized to LAST_SPIKE_INIT_VAL
    unsigned long numNeurons = heteroNeuronPop->GetNoNeurons();
    for (unsigned long n = 0; n < numNeurons; n++) {
        ASSERT_EQ(heteroNeuronPop->getLastNeuronalSpikeTime(n), LAST_SPIKE_INIT_VAL);
    }
}

TEST_F(HeteroNeuronPopTest, HeteroNeuronPostSpikeSetterTest) {
    // post spikes initialized to LAST_SPIKE_INIT_VAL
    info.dt = 0.01;
    info.time_step = 20;

    heteroNeuronPop->recordSpiker(0);

    ASSERT_EQ(heteroNeuronPop->getLastNeuronalSpikeTime(0), info.dt * info.time_step);
}

TEST_F(HeteroNeuronPopTest, HeteroNeuronSpikerSynapseSetterTest) {
    ASSERT_TRUE(heteroNeuronPop->getSpikerSynapses(0).empty());

    SynExtSharedPtr mockSynExt1 = std::make_shared<SynapseExtension>();
    heteroNeuronPop->recordSpikerSynapse(0, mockSynExt1);

    ASSERT_EQ(heteroNeuronPop->getSpikerSynapses(0).size(), 1);
    ASSERT_TRUE(heteroNeuronPop->getSpikerSynapses(0).find(mockSynExt1) != heteroNeuronPop->getSpikerSynapses(0).end());

    SynExtSharedPtr mockSynExt2 = std::make_shared<SynapseExtension>();
    heteroNeuronPop->recordSpikerSynapse(0, mockSynExt2);

    ASSERT_EQ(heteroNeuronPop->getSpikerSynapses(0).size(), 2);
    ASSERT_TRUE(heteroNeuronPop->getSpikerSynapses(0).find(mockSynExt2) != heteroNeuronPop->getSpikerSynapses(0).end());
}

TEST_F(HeteroNeuronPopTest, HeteroNeuronSpikerSynapseClearTest) {
    ASSERT_TRUE(heteroNeuronPop->getSpikerSynapses(0).empty());

    SynExtSharedPtr mockSynExt1;
    heteroNeuronPop->recordSpikerSynapse(0, mockSynExt1);
    SynExtSharedPtr mockSynExt2;
    heteroNeuronPop->recordSpikerSynapse(0, mockSynExt2);
    heteroNeuronPop->clearSpikerSynapses();

    ASSERT_EQ(heteroNeuronPop->getSpikerSynapses(0).size(), 0);
}

TEST_F(HeteroNeuronPopTest, HeteroNeuronInitialAllocationStateTest) {
    unsigned long numNeurons = heteroNeuronPop->GetNoNeurons();
    for (unsigned long n = 0; n < numNeurons; n++) {
        SynAllocStateSharedPtr allocationState = heteroNeuronPop->getAllocationState(n);
        ASSERT_EQ(allocationState->lastAllocatedPosition, 0.0);
    }
}

TEST_F(HeteroNeuronPopTest, HeteroNeuronUpdateAllocationStateTest) {
    auto synAllocationScheme = [&](unsigned long preId, unsigned long postId, const SynAllocStateSharedPtr& currentAllocState) -> SynExtSharedPtr {
        SynExtSharedPtr newSynapse = std::make_shared<SynapseExtension>();
        newSynapse->distToSoma = currentAllocState->lastAllocatedPosition + 90;
        return newSynapse;
    };

    heteroNeuronPop->getNewSynapseAllocation(0, 0, synAllocationScheme);

    SynAllocStateSharedPtr allocationState = heteroNeuronPop->getAllocationState(0);
    ASSERT_EQ(allocationState->lastAllocatedPosition, 90.0);

    heteroNeuronPop->getNewSynapseAllocation(0, 0, synAllocationScheme);
    allocationState = heteroNeuronPop->getAllocationState(0);
    ASSERT_EQ(allocationState->lastAllocatedPosition, 180.0);
}