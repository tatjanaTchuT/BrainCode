#define CATCH_CONFIG_MAIN

#include "../external/catch.hpp"
#include "../../src/Synapse/HeteroCurrentSynapse.hpp"

bool equal(const double a, const double b, double epsilon=0.0001);

std::vector<std::string> inputsPopA = {
        "neurons_0_noNeurons 3",
        "neurons_0_type HeteroNeuron",
        "neurons_0_tauM 0.020000 seconds",
        "neurons_0_vReset 10.000000 mV",
        "neurons_0_vThresh 20.000000 mV",
        "neurons_0_refractoryTime 0.002000 seconds",
        "neurons_0_seedInitialPotentials 0",
        "neurons_0_seedInitialPrevSpike 0",
        "neurons_0_morphology MonoDendriteSTDPTazerart",
        "neurons_0_dendritic_length 8 μm",
        "neurons_0_synaptic_gap 0.5 μm",
        "neurons_0_heterosynaptic_theta_decay 50.0",
        "neurons_0_intersynapse_distance_decay 50.0",
        "neurons_0_intersynapse_spike_timing_decay 1.0",
        "neurons_0_mu_LTP 0.013",
        "neurons_0_sigma_LTP 0.005",
        "neurons_0_alpha_LTP 1.0",
        "neurons_0_mu_LTD 0.007",
        "neurons_0_sigma_LTD 0.005",
        "neurons_0_beta_LTD 20.0"
};

std::vector<std::string> inputsPopB = {
        "neurons_1_noNeurons 3",
        "neurons_1_type HeteroNeuron",
        "neurons_1_tauM 0.020000 seconds",
        "neurons_1_vReset 10.000000 mV",
        "neurons_1_vThresh 20.000000 mV",
        "neurons_1_refractoryTime 0.002000 seconds",
        "neurons_1_seedInitialPotentials 0",
        "neurons_1_seedInitialPrevSpike 0",
        "neurons_1_morphology MonoDendriteSTDPTazerart",
        "neurons_1_dendritic_length 8 μm",
        "neurons_1_synaptic_gap 0.5 μm",
        "neurons_1_heterosynaptic_theta_decay 50.0",
        "neurons_1_intersynapse_distance_decay 50.0",
        "neurons_1_intersynapse_spike_timing_decay 1.0",
        "neurons_1_mu_LTP 0.013",
        "neurons_1_sigma_LTP 0.005",
        "neurons_1_alpha_LTP 1.0",
        "neurons_1_mu_LTD 0.007",
        "neurons_1_sigma_LTD 0.005",
        "neurons_1_beta_LTD 20.0"
};

std::vector<std::string> inputsSynapse = {
        "synapses_x_x_type HeteroCurrentSynapse",
        "synapses_x_x_D_min 0.000000 seconds",
        "synapses_x_x_D_max 0.000000 seconds",
        "synapses_x_x_J 0.100000 dmV/Spike",
        "synapses_x_x_J_pot 0.000000 dmV/Spike",
        "synapses_x_x_P_pot 0.000000",
        "synapses_x_x_connectivity_type HeteroRandomConnectivity",
        "synapses_x_x_connectivity_ConnectionProba 1.0",
        "synapses_x_x_connectivity_seed	0"
};

TEST_CASE("ConnectNeurons creates connections between 2 Neuron Populations", "[HeteroCurrentSynapse]") {
    std::unique_ptr<GlobalSimInfo> info(new GlobalSimInfo());
    info->dt = 0.0001;

    // prepare neuron populations
    std::shared_ptr<NeuronPop> popA(new HeteroLIFNeuronPop(info.get(), 0));
    popA->LoadParameters(&inputsPopA);

    std::shared_ptr<NeuronPop> popB(new HeteroLIFNeuronPop(info.get(), 1));
    popB->LoadParameters(&inputsPopB);

    // prepare synapse
    std::vector<std::pair<unsigned long, unsigned long>> targets;
    std::vector<SynapseExt> synapseData;

    // Test Synapse 0-0 #########################################################
    std::unique_ptr<HeteroCurrentSynapse> synapse_0_0(new HeteroCurrentSynapse(popA.get(), popA.get(), info.get()));
    synapse_0_0->LoadParameters(&inputsSynapse);
    synapse_0_0->ConnectNeurons();

    synapseData = getSynapseData(*synapse_0_0);

    unsigned long postNeuronId, localSynapseId;

    // ########################
    targets = getSynapticTargets(*synapse_0_0, 0);

    REQUIRE(targets.size() == 2);

    postNeuronId = targets[0].first;
    localSynapseId = targets[0].second;
    REQUIRE(postNeuronId == 1);
    REQUIRE(localSynapseId == 2);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].globalId == 0);

    postNeuronId = targets[1].first;
    localSynapseId = targets[1].second;
    REQUIRE(postNeuronId == 2);
    REQUIRE(localSynapseId == 4);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].globalId == 0);


    // ########################
    targets = getSynapticTargets(*synapse_0_0, 1);

    REQUIRE(targets.size() == 2);

    postNeuronId = targets[0].first;
    localSynapseId = targets[0].second;
    REQUIRE(postNeuronId == 0);
    REQUIRE(localSynapseId == 1);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].globalId == 1);

    postNeuronId = targets[1].first;
    localSynapseId = targets[1].second;
    REQUIRE(postNeuronId == 2);
    REQUIRE(localSynapseId == 5);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].globalId == 1);

    // ########################
    targets = getSynapticTargets(*synapse_0_0, 2);

    REQUIRE(targets.size() == 2);

    postNeuronId = targets[0].first;
    localSynapseId = targets[0].second;
    REQUIRE(postNeuronId == 0);
    REQUIRE(localSynapseId == 0);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].globalId == 0);

    postNeuronId = targets[1].first;
    localSynapseId = targets[1].second;
    REQUIRE(postNeuronId == 1);
    REQUIRE(localSynapseId == 3);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].globalId == 1);

    // Test Synapse 0-1 #########################################################
    std::unique_ptr<HeteroCurrentSynapse> synapse_0_1(new HeteroCurrentSynapse(popB.get(), popA.get(), info.get()));
    synapse_0_1->LoadParameters(&inputsSynapse);
    synapse_0_1->ConnectNeurons();

    synapseData = getSynapseData(*synapse_0_1);

    // ########################
    targets = getSynapticTargets(*synapse_0_1, 0);

    REQUIRE(targets.size() == 3);

    postNeuronId = targets[0].first;
    localSynapseId = targets[0].second;
    REQUIRE(postNeuronId == 0);
    REQUIRE(localSynapseId == 2);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].globalId == 2);

    postNeuronId = targets[1].first;
    localSynapseId = targets[1].second;
    REQUIRE(postNeuronId == 1);
    REQUIRE(localSynapseId == 3);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].globalId == 0);

    postNeuronId = targets[2].first;
    localSynapseId = targets[2].second;
    REQUIRE(postNeuronId == 2);
    REQUIRE(localSynapseId == 7);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].globalId == 1);

    // ########################
    targets = getSynapticTargets(*synapse_0_1, 1);

    REQUIRE(targets.size() == 3);

    postNeuronId = targets[0].first;
    localSynapseId = targets[0].second;
    REQUIRE(postNeuronId == 0);
    REQUIRE(localSynapseId == 1);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].globalId == 1);

    postNeuronId = targets[1].first;
    localSynapseId = targets[1].second;
    REQUIRE(postNeuronId == 1);
    REQUIRE(localSynapseId == 5);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].globalId == 2);

    postNeuronId = targets[2].first;
    localSynapseId = targets[2].second;
    REQUIRE(postNeuronId == 2);
    REQUIRE(localSynapseId == 8);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].globalId == 2);


    // ########################
    targets = getSynapticTargets(*synapse_0_1, 2);

    REQUIRE(targets.size() == 3);

    postNeuronId = targets[0].first;
    localSynapseId = targets[0].second;
    REQUIRE(postNeuronId == 0);
    REQUIRE(localSynapseId == 0);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].globalId == 0);

    postNeuronId = targets[1].first;
    localSynapseId = targets[1].second;
    REQUIRE(postNeuronId == 1);
    REQUIRE(localSynapseId == 4);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].globalId == 1);

    postNeuronId = targets[2].first;
    localSynapseId = targets[2].second;
    REQUIRE(postNeuronId == 2);
    REQUIRE(localSynapseId == 6);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].globalId == 0);

    // Test Synapse 0-1 #########################################################
    std::unique_ptr<HeteroCurrentSynapse> synapse_1_0(new HeteroCurrentSynapse(popA.get(), popB.get(), info.get()));
    synapse_1_0->LoadParameters(&inputsSynapse);
    synapse_1_0->ConnectNeurons();

    synapseData = getSynapseData(*synapse_1_0);

    // ########################
    targets = getSynapticTargets(*synapse_1_0, 0);

    REQUIRE(targets.size() == 3);

    postNeuronId = targets[0].first;
    localSynapseId = targets[0].second;
    REQUIRE(postNeuronId == 0);
    REQUIRE(localSynapseId == 2);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].globalId == 4);

    postNeuronId = targets[1].first;
    localSynapseId = targets[1].second;
    REQUIRE(postNeuronId == 1);
    REQUIRE(localSynapseId == 3);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].globalId == 2);

    postNeuronId = targets[2].first;
    localSynapseId = targets[2].second;
    REQUIRE(postNeuronId == 2);
    REQUIRE(localSynapseId == 7);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].globalId == 3);

    // ########################
    targets = getSynapticTargets(*synapse_1_0, 1);

    REQUIRE(targets.size() == 3);

    postNeuronId = targets[0].first;
    localSynapseId = targets[0].second;
    REQUIRE(postNeuronId == 0);
    REQUIRE(localSynapseId == 1);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].globalId == 3);

    postNeuronId = targets[1].first;
    localSynapseId = targets[1].second;
    REQUIRE(postNeuronId == 1);
    REQUIRE(localSynapseId == 5);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].globalId == 4);

    postNeuronId = targets[2].first;
    localSynapseId = targets[2].second;
    REQUIRE(postNeuronId == 2);
    REQUIRE(localSynapseId == 8);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].globalId == 4);

    // ########################
    targets = getSynapticTargets(*synapse_1_0, 2);

    REQUIRE(targets.size() == 3);

    postNeuronId = targets[0].first;
    localSynapseId = targets[0].second;
    REQUIRE(postNeuronId == 0);
    REQUIRE(localSynapseId == 0);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].globalId == 2);

    postNeuronId = targets[1].first;
    localSynapseId = targets[1].second;
    REQUIRE(postNeuronId == 1);
    REQUIRE(localSynapseId == 4);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].globalId == 3);

    postNeuronId = targets[2].first;
    localSynapseId = targets[2].second;
    REQUIRE(postNeuronId == 2);
    REQUIRE(localSynapseId == 6);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].globalId == 2);

    // Test Synapse 1-1 #########################################################
    std::unique_ptr<HeteroCurrentSynapse> synapse_1_1(new HeteroCurrentSynapse(popB.get(), popB.get(), info.get()));
    synapse_1_1->LoadParameters(&inputsSynapse);
    synapse_1_1->ConnectNeurons();

    synapseData = getSynapseData(*synapse_1_1);

    // ########################
    targets = getSynapticTargets(*synapse_1_1, 0);

    REQUIRE(targets.size() == 2);

    postNeuronId = targets[0].first;
    localSynapseId = targets[0].second;
    REQUIRE(postNeuronId == 1);
    REQUIRE(localSynapseId == 2);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].globalId == 3);

    postNeuronId = targets[1].first;
    localSynapseId = targets[1].second;
    REQUIRE(postNeuronId == 2);
    REQUIRE(localSynapseId == 4);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].globalId == 3);

    // ########################
    targets = getSynapticTargets(*synapse_1_1, 1);

    REQUIRE(targets.size() == 2);

    postNeuronId = targets[0].first;
    localSynapseId = targets[0].second;
    REQUIRE(postNeuronId == 0);
    REQUIRE(localSynapseId == 1);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].globalId == 4);

    postNeuronId = targets[1].first;
    localSynapseId = targets[1].second;
    REQUIRE(postNeuronId == 2);
    REQUIRE(localSynapseId == 5);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].globalId == 4);

    // ########################
    targets = getSynapticTargets(*synapse_1_1, 2);

    REQUIRE(targets.size() == 2);

    postNeuronId = targets[0].first;
    localSynapseId = targets[0].second;
    REQUIRE(postNeuronId == 0);
    REQUIRE(localSynapseId == 0);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 0);
    REQUIRE(synapseData[localSynapseId].globalId == 3);

    postNeuronId = targets[1].first;
    localSynapseId = targets[1].second;
    REQUIRE(postNeuronId == 1);
    REQUIRE(localSynapseId == 3);
    REQUIRE(synapseData[localSynapseId].preNeuronId == 2);
    REQUIRE(synapseData[localSynapseId].postNeuronId == 1);
    REQUIRE(synapseData[localSynapseId].globalId == 4);
}

TEST_CASE("Advect correctly records synaptic spikes", "[HeteroCurrentSynapse]") {
    std::unique_ptr<GlobalSimInfo> info(new GlobalSimInfo());
    info->dt = 0.0001;

    // prepare neuron populations
    std::shared_ptr<HeteroLIFNeuronPop> popA(new HeteroLIFNeuronPop(info.get(), 0));
    popA->LoadParameters(&inputsPopA);

    std::shared_ptr<HeteroLIFNeuronPop> popB(new HeteroLIFNeuronPop(info.get(), 1));
    popB->LoadParameters(&inputsPopB);

    std::unique_ptr<HeteroCurrentSynapse> synapse_0_0(new HeteroCurrentSynapse(popA.get(), popA.get(), info.get()));
    std::unique_ptr<HeteroCurrentSynapse> synapse_0_1(new HeteroCurrentSynapse(popB.get(), popA.get(), info.get()));
    std::unique_ptr<HeteroCurrentSynapse> synapse_1_0(new HeteroCurrentSynapse(popA.get(), popB.get(), info.get()));
    std::unique_ptr<HeteroCurrentSynapse> synapse_1_1(new HeteroCurrentSynapse(popB.get(), popB.get(), info.get()));

    synapse_0_0->LoadParameters(&inputsSynapse);
    synapse_0_0->ConnectNeurons();
    synapse_0_0->SetDistributionJ();
    synapse_0_0->SetDistributionD();
    synapse_0_1->LoadParameters(&inputsSynapse);
    synapse_0_1->ConnectNeurons();
    synapse_0_1->SetDistributionJ();
    synapse_0_1->SetDistributionD();
    synapse_1_0->LoadParameters(&inputsSynapse);
    synapse_1_0->ConnectNeurons();
    synapse_1_0->SetDistributionJ();
    synapse_1_0->SetDistributionD();
    synapse_1_1->LoadParameters(&inputsSynapse);
    synapse_1_1->ConnectNeurons();
    synapse_1_1->SetDistributionJ();
    synapse_1_1->SetDistributionD();

    std::vector<long> spikersA;
    std::vector<long> spikersB;
    std::vector<double> synapticDvPopA(popA->GetNoNeurons());
    std::vector<double> synapticDvPopB(popB->GetNoNeurons());

    std::vector<unsigned long> spikedSynapses;

    // stimulate 1 neuron in popA
    synapticDvPopA[1] = 21.0;
    popA->advect(&synapticDvPopA);
    popB->advect(&synapticDvPopB);

    spikersA = *popA->GetSpikers();
    spikersB = *popB->GetSpikers();
    REQUIRE(spikersA.size() == 1);
    REQUIRE(spikersA[0] == 1);
    REQUIRE(spikersB.empty());

    // advect synapse_0_0
    synapse_0_0->advect(&synapticDvPopA);
    spikedSynapses = getSpikedSynapses(*popA, 0);
    REQUIRE(spikedSynapses.size() == 1);
    REQUIRE(spikedSynapses[0] == 1);
    spikedSynapses = getSpikedSynapses(*popA, 1);
    REQUIRE(spikedSynapses.empty());
    spikedSynapses = getSpikedSynapses(*popA, 2);
    REQUIRE(spikedSynapses.size() == 1);
    REQUIRE(spikedSynapses[0] == 1);

    // advect synapse_0_1
    synapse_0_1->advect(&synapticDvPopB);
    spikedSynapses = getSpikedSynapses(*popB, 0);
    REQUIRE(spikedSynapses.size() == 1);
    REQUIRE(spikedSynapses[0] == 1);
    spikedSynapses = getSpikedSynapses(*popB, 1);
    REQUIRE(spikedSynapses.size() == 1);
    REQUIRE(spikedSynapses[0] == 2);
    spikedSynapses = getSpikedSynapses(*popB, 2);
    REQUIRE(spikedSynapses.size() == 1);
    REQUIRE(spikedSynapses[0] == 2);

    // advect synapse_1_0
    synapse_1_0->advect(&synapticDvPopA);
    spikedSynapses = getSpikedSynapses(*popA, 0);
    REQUIRE(spikedSynapses.size() == 1);
    REQUIRE(spikedSynapses[0] == 1);
    spikedSynapses = getSpikedSynapses(*popA, 1);
    REQUIRE(spikedSynapses.empty());
    spikedSynapses = getSpikedSynapses(*popA, 2);
    REQUIRE(spikedSynapses.size() == 1);
    REQUIRE(spikedSynapses[0] == 1);

    // advect synapse_1_1
    synapse_1_1->advect(&synapticDvPopB);
    spikedSynapses = getSpikedSynapses(*popB, 0);
    REQUIRE(spikedSynapses.size() == 1);
    REQUIRE(spikedSynapses[0] == 1);
    spikedSynapses = getSpikedSynapses(*popB, 1);
    REQUIRE(spikedSynapses.size() == 1);
    REQUIRE(spikedSynapses[0] == 2);
    spikedSynapses = getSpikedSynapses(*popB, 2);
    REQUIRE(spikedSynapses.size() == 1);
    REQUIRE(spikedSynapses[0] == 2);
}

TEST_CASE("Advect correctly populates synapticDv", "[HeteroCurrentSynapse]") {
    std::unique_ptr<GlobalSimInfo> info(new GlobalSimInfo());
    info->dt = 0.0001;

    // prepare neuron populations
    std::shared_ptr<HeteroLIFNeuronPop> popA(new HeteroLIFNeuronPop(info.get(), 0));
    popA->LoadParameters(&inputsPopA);

    std::shared_ptr<HeteroLIFNeuronPop> popB(new HeteroLIFNeuronPop(info.get(), 1));
    popB->LoadParameters(&inputsPopB);

    std::unique_ptr<HeteroCurrentSynapse> synapse_0_0(new HeteroCurrentSynapse(popA.get(), popA.get(), info.get()));
    std::unique_ptr<HeteroCurrentSynapse> synapse_0_1(new HeteroCurrentSynapse(popB.get(), popA.get(), info.get()));
    std::unique_ptr<HeteroCurrentSynapse> synapse_1_0(new HeteroCurrentSynapse(popA.get(), popB.get(), info.get()));
    std::unique_ptr<HeteroCurrentSynapse> synapse_1_1(new HeteroCurrentSynapse(popB.get(), popB.get(), info.get()));

    synapse_0_0->LoadParameters(&inputsSynapse);
    synapse_0_0->ConnectNeurons();
    synapse_0_0->SetDistributionJ();
    synapse_0_0->SetDistributionD();
    synapse_0_1->LoadParameters(&inputsSynapse);
    synapse_0_1->ConnectNeurons();
    synapse_0_1->SetDistributionJ();
    synapse_0_1->SetDistributionD();
    synapse_1_0->LoadParameters(&inputsSynapse);
    synapse_1_0->ConnectNeurons();
    synapse_1_0->SetDistributionJ();
    synapse_1_0->SetDistributionD();
    synapse_1_1->LoadParameters(&inputsSynapse);
    synapse_1_1->ConnectNeurons();
    synapse_1_1->SetDistributionJ();
    synapse_1_1->SetDistributionD();

    std::vector<long> spikersA;
    std::vector<long> spikersB;
    std::vector<double> synapticDvPopA(popA->GetNoNeurons());
    std::vector<double> synapticDvPopB(popB->GetNoNeurons());

    std::vector<unsigned long> spikedSynapses;

    // stimulate 1 neuron in popA
    synapticDvPopA[1] = 21.0;
    synapticDvPopB[0] = 21.0;
    popA->advect(&synapticDvPopA);
    popB->advect(&synapticDvPopB);

    std::fill(synapticDvPopA.begin(), synapticDvPopA.end(), 0);
    std::fill(synapticDvPopB.begin(), synapticDvPopB.end(), 0);

    spikersA = *popA->GetSpikers();
    spikersB = *popB->GetSpikers();
    REQUIRE(spikersA.size() == 1);
    REQUIRE(spikersA[0] == 1);
    REQUIRE(spikersB.size());
    REQUIRE(spikersB[0] == 0);

    // advect synapse_0_0
    synapse_0_0->advect(&synapticDvPopA);
    REQUIRE(equal(synapticDvPopA[0], 0.1, 0.1));
    REQUIRE(equal(synapticDvPopA[1], 0.0, 0.1));
    REQUIRE(equal(synapticDvPopA[2], 0.1, 0.1));

    // advect synapse_0_1
    synapse_0_1->advect(&synapticDvPopB);
    REQUIRE(equal(synapticDvPopB[0], 0.1, 0.1));
    REQUIRE(equal(synapticDvPopB[1], 0.1, 0.1));
    REQUIRE(equal(synapticDvPopB[2], 0.1, 0.1));

    // advect synapse_1_0
    synapse_1_0->advect(&synapticDvPopA);
    REQUIRE(equal(synapticDvPopA[0], 0.2, 0.1));
    REQUIRE(equal(synapticDvPopA[1], 0.1, 0.1));
    REQUIRE(equal(synapticDvPopA[2], 0.2, 0.1));

    // advect synapse_1_1
    synapse_1_1->advect(&synapticDvPopB);
    REQUIRE(equal(synapticDvPopB[0], 0.1, 0.1));
    REQUIRE(equal(synapticDvPopB[1], 0.2, 0.1));
    REQUIRE(equal(synapticDvPopB[2], 0.2, 0.1));

}
