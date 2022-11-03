//
// Created by saiftyfirst on 03.12.21.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "../../src/Synapse/HeteroSynapse.hpp"

class MockNeuronPop: public NeuronPop {
public:
    MockNeuronPop(GlobalSimInfo * info, int id): NeuronPop(info, id) {}

    MOCK_METHOD(void, advect, (std::vector<double> * synaptic_dV), (override));
    MOCK_METHOD(std::string, GetType, (), (override));
};

class MockHeteroNeuronPop: public HeteroNeuronPop {
public:
    MockHeteroNeuronPop(GlobalSimInfo * info, int id): HeteroNeuronPop(info, id) {}

    MOCK_METHOD(void, advect, (std::vector<double> * synaptic_dV), (override));
    MOCK_METHOD(std::string, GetType, (), (override));
};

class MockHeteroConnectivity: public HeteroConnectivity {

public:
    MockHeteroConnectivity(Synapse *syn,GlobalSimInfo *info): HeteroConnectivity(syn, info) {
        setNoSourceNeurons(syn->GetNoNeuronsPre());
    }

    MOCK_METHOD(std::string, GetTypeStr, (), (override));

    SynExtSharedPtr allocationScheme(unsigned long preId, unsigned long postId, SynAllocStateSharedPtr currAlloc) override {
        SynExtSharedPtr syn = std::make_shared<SynapseExtension>();
        syn->distToSoma = currAlloc->lastAllocatedPosition + 0.5;
        syn->weight = 1;
        syn->postNeuronId = postId;
        syn->preNeuronId = preId;
        syn->lastSpike = LAST_SPIKE_INIT_VAL;
        syn->theta = 0;
        return syn;
    }

    unsigned long GetNumberAverageSourceNeurons() override {
        return synapse->GetNoNeuronsPre();
    }
};

class MockHeteroSynapse: public HeteroSynapse {
private:
    friend class HeteroSynapseTest;
    FRIEND_TEST(HeteroSynapseTest, PreAdvectUpdatesCooperativityAndAdvectsSynapses);

public:
    MockHeteroSynapse(NeuronPop *postNeurons, NeuronPop *preNeurons, GlobalSimInfo *info) : HeteroSynapse(postNeurons, preNeurons, info) {
        this->geometry = new MockHeteroConnectivity(this, info);
    }
    MOCK_METHOD(int, GetNumberOfDataColumns, (), (override));
    MOCK_METHOD(std::string, GetDataHeader, (int), (override));
    MOCK_METHOD(std::string, GetUnhashedDataHeader, (), (override));
    MOCK_METHOD(std::valarray<double>, GetSynapticState, (int), (override));
    MOCK_METHOD(std::string, GetTypeStr, (), (override));

protected:
    MOCK_METHOD(void, advect_finalize, (std::vector<std::vector<double>> * ), (override));
    MOCK_METHOD(void, advect_spikers, (std::vector<double> * currents, long spiker), (override));
    MOCK_METHOD(void, prepareSynapses, (), (override));
    MOCK_METHOD(void, updateCooperativityAmongSynapses, (), (override));
    MOCK_METHOD(void, advectSynapses, (), (override));
    MOCK_METHOD(void, finalizeSynapses, (), (override));

};

class HeteroSynapseTest : public ::testing::Test {
protected:
    MockHeteroSynapse* heteroSynapse{};
    MockNeuronPop* prePop{};
    MockHeteroNeuronPop* postPop{};
    GlobalSimInfo info;
    std::vector<std::string> preInputs = {
            "neurons_0_noNeurons 5"
    };
    std::vector<std::string> postInputs = {
            "neurons_1_noNeurons 2"
    };
    std::vector<std::string> synInputs = {
            "synapse_0_0_heterosynaptic_theta_decay 0.025",
            "synapses_0_0_connectivity_ConnectionProba 1.00000"
    };

    void SetUp() override {
        prePop = new MockNeuronPop(&info, 0);
        prePop->LoadParameters(&preInputs);
        postPop = new MockHeteroNeuronPop(&info, 1);
        postPop->LoadParameters(&postInputs);

        heteroSynapse = new MockHeteroSynapse(postPop, prePop, &info);
        heteroSynapse->LoadParameters(&synInputs);
        heteroSynapse->ConnectNeurons();
    }

    void TearDown() override {
        delete heteroSynapse;
        delete prePop;
        delete postPop;
    }
};

TEST_F(HeteroSynapseTest, HeteroSynapseFailsWhenPostAsNeuronPop) {
    MockNeuronPop prePopCustom(&info, 0);
    MockNeuronPop postPopCustom(&info, 1);

    EXPECT_EXIT(MockHeteroSynapse(&prePopCustom, &postPopCustom, &info), testing::KilledBySignal(SIGABRT), "");
}

TEST_F(HeteroSynapseTest, HeteroSynapseSuccesssWhenPreAsNeuronPop) {
    EXPECT_NO_FATAL_FAILURE(MockHeteroSynapse(postPop, prePop, &info));
}

TEST_F(HeteroSynapseTest, HeteroSynapseFailureOnNoThetaDecay) {
    std::vector<std::string> preInputsCustom = {
            "neurons_0_noNeurons 1",
    };

    std::vector<std::string> postInputsCustom = {
            "neurons_1_noNeurons 1",
    };

    std::vector<std::string> synInputsCustom = {
    };

    MockNeuronPop prePopCustom(&info, 0);
    prePopCustom.LoadParameters(&preInputsCustom);

    MockHeteroNeuronPop postPopCustom(&info, 1);
    postPopCustom.LoadParameters(&postInputsCustom);

    MockHeteroSynapse mockHeteroSynapse(&postPopCustom, &prePopCustom, &info);
    EXPECT_EXIT(mockHeteroSynapse.LoadParameters(&synInputsCustom), testing::KilledBySignal(SIGABRT), "");
}

TEST_F(HeteroSynapseTest, AdvectDoesNotClearsSynSpikers) {
    info.dt = 0.001;
    info.time_step = 50;

    // mark synapses as spikers
    std::vector<SynExtSharedPtr> synExtensions = heteroSynapse->getSynapseExtensions().at(0);
    postPop->recordSpikerSynapse(0, synExtensions.at(0));
    postPop->recordSpikerSynapse(0, synExtensions.at(2));

    EXPECT_EQ(postPop->getSpikerSynapses(0).size(), 2);

    std::vector<double> synapticDv(postPop->GetNoNeurons());
    heteroSynapse->advect(&synapticDv);

    EXPECT_EQ(postPop->getSpikerSynapses(0).size(), 2);
}

TEST_F(HeteroSynapseTest, PreAdvectUpdatesCooperativityAndAdvectsSynapses) {
//    // mark synapses as spikers
    std::vector<SynExtSharedPtr> synExtensions = heteroSynapse->getSynapseExtensions().at(0);
    postPop->recordSpikerSynapse(0, synExtensions.at(0));
    postPop->recordSpikerSynapse(0, synExtensions.at(2));

    EXPECT_CALL(*heteroSynapse, finalizeSynapses()).Times(1)
        .After(
            EXPECT_CALL(*heteroSynapse, advectSynapses()).Times(1)
                    .After(
                            EXPECT_CALL(*heteroSynapse, updateCooperativityAmongSynapses()).Times(1)
                                    .After(
                                            EXPECT_CALL(*heteroSynapse, prepareSynapses()).Times(1)
                                    )
                    )
            );

    std::vector<double> synapticDv(postPop->GetNoNeurons());
    heteroSynapse->preAdvect();
}

