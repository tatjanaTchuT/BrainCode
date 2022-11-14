#include "../../external/catch.hpp"
#include "../../../src/NeuronPop/Morphology/TimingDependentModels/MonoDendriteSTDPTazerart.hpp"

bool equal(const double a, const double b, const double epsilon=0.01) {
    return abs(a-b) < epsilon;
}

TEST_CASE("Synapses are allocated from opposite ends of the dendrite in alternating manner", "[MonoDendriteSTDP]") {
    std::vector<std::string> inputs = {
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

    std::unique_ptr<Morphology> monoDendrite(new MonoDendriteSTDPTazerart(nullptr));
    monoDendrite->LoadParameters(&inputs);

    std::shared_ptr<SynapseExt> allocation = monoDendrite->allocateNewSynapse();
    REQUIRE(allocation->distToSoma == 1.0);

    allocation = monoDendrite->allocateNewSynapse();
    REQUIRE(allocation->distToSoma == 2.0);

    allocation = monoDendrite->allocateNewSynapse();
    REQUIRE(allocation->distToSoma == 3.0);

    allocation = monoDendrite->allocateNewSynapse();
    REQUIRE(allocation == nullptr); // no available synapse
}

TEST_CASE("Last spike times initialized to large negative value", "[MonoDendriteSTDPTazerart]") {
    std::vector<std::string> inputs = {
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

    std::unique_ptr<Morphology> monoDendrite(new MonoDendriteSTDPTazerart(nullptr));
    monoDendrite->LoadParameters(&inputs);

    std::shared_ptr<SynapseExt> allocation = monoDendrite->allocateNewSynapse();
    REQUIRE(equal(allocation->lastSpike, -200.0));

    allocation = monoDendrite->allocateNewSynapse();
    REQUIRE(equal(allocation->lastSpike, -200.0));
}

TEST_CASE("Last spike times correctly updated", "[MonoDendriteSTDPTazerart]") {
    std::vector<std::string> inputs = {
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

    std::unique_ptr<GlobalSimInfo> info(new GlobalSimInfo());
    info->dt = 0.01;

    std::unique_ptr<Morphology> monoDendrite(new MonoDendriteSTDPTazerart(info.get()));
    monoDendrite->LoadParameters(&inputs);

    std::shared_ptr<SynapseExt> allocationA = monoDendrite->allocateNewSynapse();
    std::shared_ptr<SynapseExt> allocationB = monoDendrite->allocateNewSynapse();


    info->time_step = 10;
    monoDendrite->recordPreSpike(0); // at time 0.1
    REQUIRE(allocationA->lastSpike == 0.1);

    info->time_step = 20;
    monoDendrite->recordPreSpike(0); // at time 0.2
    REQUIRE(allocationA->lastSpike == 0.2);

}

TEST_CASE("Cooperativity is distributed two synapses", "[MonoDendriteSTDPTazerart]") {
    std::vector<std::string> inputs = {
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

    std::unique_ptr<GlobalSimInfo> info(new GlobalSimInfo());
    info->dt = 0.001;

    std::unique_ptr<Morphology> monoDendrite(new MonoDendriteSTDPTazerart(info.get()));
    monoDendrite->LoadParameters(&inputs);

    std::shared_ptr<SynapseExt> allocationA = monoDendrite->allocateNewSynapse();
    std::shared_ptr<SynapseExt> allocationB = monoDendrite->allocateNewSynapse();

    info->time_step = 1205;
    monoDendrite->recordPreSpike(0);  // at time 1.205
    monoDendrite->advect();
    REQUIRE(equal(allocationA->theta, 0.0));
    REQUIRE(equal(allocationB->theta, 0.0));

    info->time_step = 1305;
    monoDendrite->recordPreSpike(1);  // at time 1.305
    monoDendrite->advect();
    REQUIRE(equal(allocationA->theta, allocationB->theta));
    REQUIRE(equal(allocationA->theta, 0.88692));
}

TEST_CASE("Advect decays heffects", "[MonoDendriteSTDPTazerart]") {
    std::vector<std::string> inputs = {
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

    std::unique_ptr<GlobalSimInfo> info(new GlobalSimInfo());
    info->dt = 0.01;

    std::unique_ptr<Morphology> monoDendrite(new MonoDendriteSTDPTazerart(info.get()));
    monoDendrite->LoadParameters(&inputs);

    std::shared_ptr<SynapseExt> allocationA = monoDendrite->allocateNewSynapse();
    std::shared_ptr<SynapseExt> allocationB = monoDendrite->allocateNewSynapse();

    info->time_step = 1205;
    monoDendrite->recordPreSpike(0);  // at time 12.05
    monoDendrite->recordPreSpike(1);  // at time 12.05
    monoDendrite->advect();
    REQUIRE(equal(allocationA->theta, allocationB->theta));
    REQUIRE(equal(allocationA->theta, 0.98019867));

    for (int i = 0; i < 100; ++i) {
        monoDendrite->advect(); // advect dt*100 = 1s
    }

    REQUIRE(equal(allocationA->theta, allocationB->theta));
    REQUIRE(equal(allocationA->theta, 0.94176454));
}

TEST_CASE("Cooperativity is distributed across multiple synapses", "[MonoDendriteSTDPTazerart]") {
    std::vector<std::string> inputs = {
            "neurons_0_dendtric_length  6 μm",
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

    std::unique_ptr<GlobalSimInfo> info(new GlobalSimInfo());
    info->dt = 0.01;

    std::unique_ptr<Morphology> monoDendrite(new MonoDendriteSTDPTazerart(info.get()));
    monoDendrite->LoadParameters(&inputs);

    std::shared_ptr<SynapseExt> allocationA = monoDendrite->allocateNewSynapse();
    std::shared_ptr<SynapseExt> allocationB = monoDendrite->allocateNewSynapse();
    std::shared_ptr<SynapseExt> allocationC = monoDendrite->allocateNewSynapse();

    for (long step = 205; step <= 406; step++) {
        info->time_step = step;

        if (step == 205) { // Synapse 0 spikes at 2.05 sec
            monoDendrite->recordPreSpike(0);  // notify spike
            REQUIRE(equal(allocationA->theta, 0.0));
            REQUIRE(equal(allocationB->theta, 0.0));
            REQUIRE(equal(allocationC->theta, 0.0));
        }

        if (step == 305) { // Synapse 1 spikes at 3.05 sec
            monoDendrite->recordPreSpike(1);  // notify spike
        }

        if (step == 306) { // assert hetero effects + decay from Synapse 1 spike 1.305
            REQUIRE(equal(allocationA->theta, allocationB->theta));
            REQUIRE(equal(allocationA->theta, 0.36059494017307814));
            REQUIRE(equal(allocationC->theta, 0.0));

        }

        if (step == 405) { // Synapse 2 spikes at 4.05 sec
            monoDendrite->recordPreSpike(2);  // notify spike
        }

        if (step == 406) { // assert hetero effects + decay from Synapse 1 spike 4.06
            REQUIRE(equal(allocationA->theta, 0.47648452120848406));
            REQUIRE(equal(allocationB->theta, 0.70705075050313659));
            REQUIRE(equal(allocationC->theta, 0.49062365105150441));
        }

        monoDendrite->advect();
    }
}

TEST_CASE("Pre-Post stimulation induces timing-dependent LTP in single synapse (delta at peak of window)", "[MonoDendriteSTDPTazerart]") {
    std::vector<std::string> inputs = {
            "neurons_0_dendtric_length  6 μm",
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

    std::unique_ptr<GlobalSimInfo> info(new GlobalSimInfo());
    info->dt = 0.001;

    std::unique_ptr<Morphology> monoDendrite(new MonoDendriteSTDPTazerart(info.get()));
    monoDendrite->LoadParameters(&inputs);

    std::shared_ptr<SynapseExt> allocationA = monoDendrite->allocateNewSynapse();

    for (long step = 205; step <= 219; step++) {
        info->time_step = step;

        if (step == 205) { // Synapse 0 spikes at 0.205 sec
            monoDendrite->recordPreSpike(0);  // notify spike
            REQUIRE(equal(allocationA->theta, 0.0));
        }

        if (step == 218) { // Post-spike at 0.218
            monoDendrite->recordPostSpike();
        }

        if (step == 219) { // check LTP in step after post spike
            REQUIRE(equal(allocationA->weight, 2));
        }

        monoDendrite->advect();
    }
}

TEST_CASE("Pre-Post stimulation induces timing-dependent LTP in synapse (delta at lower than peak of window)", "[MonoDendriteSTDPTazerart]") {
    std::vector<std::string> inputs = {
            "neurons_0_dendtric_length  6 μm",
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

    std::unique_ptr<GlobalSimInfo> info(new GlobalSimInfo());
    info->dt = 0.001;

    std::unique_ptr<Morphology> monoDendrite(new MonoDendriteSTDPTazerart(info.get()));
    monoDendrite->LoadParameters(&inputs);

    std::shared_ptr<SynapseExt> allocationA = monoDendrite->allocateNewSynapse();

    for (long step = 205; step <= 219; step++) {
        info->time_step = step;

        if (step == 205) { // Synapse 0 spikes at 0.205 sec
            monoDendrite->recordPreSpike(0);  // notify spike
            REQUIRE(equal(allocationA->theta, 0.0));
        }

        if (step == 210) { // Post-spike at 0.210
            monoDendrite->recordPostSpike();
        }

        if (step == 211) { // check LTP in step after post spike
            REQUIRE(equal(allocationA->weight, 1.27804));
        }

        monoDendrite->advect();
    }
}

TEST_CASE("Pre-Post stimulation induces timing-dependent LTP in synapse (delta at higher than peak of window)", "[MonoDendriteSTDPTazerart]") {
    std::vector<std::string> inputs = {
            "neurons_0_dendtric_length  6 μm",
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

    std::unique_ptr<GlobalSimInfo> info(new GlobalSimInfo());
    info->dt = 0.001;

    std::unique_ptr<Morphology> monoDendrite(new MonoDendriteSTDPTazerart(info.get()));
    monoDendrite->LoadParameters(&inputs);

    std::shared_ptr<SynapseExt> allocationA = monoDendrite->allocateNewSynapse();

    for (long step = 205; step <= 227; step++) {
        info->time_step = step;

        if (step == 205) { // Synapse 0 spikes at 0.205 sec
            monoDendrite->recordPreSpike(0);  // notify spike
            REQUIRE(equal(allocationA->theta, 0.0));
        }

        if (step == 226) { // Post-spike at 0.226
            monoDendrite->recordPostSpike();
        }

        if (step == 227) { // check LTP in step after post spike
            REQUIRE(equal(allocationA->weight, 1.27804));
        }

        monoDendrite->advect();
    }
}

TEST_CASE("Post-Pre stimulation induces timing-dependent LTD in synapse (delta at peak of window)", "[MonoDendriteSTDPTazerart]") {
    std::vector<std::string> inputs = {
            "neurons_0_dendtric_length  6 μm",
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

    std::unique_ptr<GlobalSimInfo> info(new GlobalSimInfo());
    info->dt = 0.001;

    std::unique_ptr<Morphology> monoDendrite(new MonoDendriteSTDPTazerart(info.get()));
    monoDendrite->LoadParameters(&inputs);

    std::shared_ptr<SynapseExt> allocationA = monoDendrite->allocateNewSynapse();

    for (long step = 205; step <= 219; step++) {
        info->time_step = step;

        if (step == 205) { // Post-spike at 0.226
            monoDendrite->recordPostSpike();
        }

        if (step == 212) { // Synapse 0 spikes at 0.205 sec
            monoDendrite->recordPreSpike(0);  // notify spike
            REQUIRE(equal(allocationA->theta, 0.0));
        }

        if (step == 213) { // check LTP in step after post spike
            REQUIRE(equal(allocationA->weight, 0.0));
        }

        monoDendrite->advect();
    }
}

TEST_CASE("Post-Pre stimulation induces timing-dependent LTD in synapse (delta at lower than peak of window)", "[MonoDendriteSTDPTazerart]") {
    std::vector<std::string> inputs = {
            "neurons_0_dendtric_length  6 μm",
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

    std::unique_ptr<GlobalSimInfo> info(new GlobalSimInfo());
    info->dt = 0.001;

    std::unique_ptr<Morphology> monoDendrite(new MonoDendriteSTDPTazerart(info.get()));
    monoDendrite->LoadParameters(&inputs);

    std::shared_ptr<SynapseExt> allocationA = monoDendrite->allocateNewSynapse();

    for (long step = 205; step <= 219; step++) {
        info->time_step = step;

        if (step == 205) { // Post-spike at 0.226
            monoDendrite->recordPostSpike();
        }

        if (step == 208) { // Synapse 0 spikes at 0.205 sec
            monoDendrite->recordPreSpike(0);  // notify spike
            REQUIRE(equal(allocationA->theta, 0.0));
        }

        if (step == 209) { // check LTP in step after post spike
            REQUIRE(equal(allocationA->weight, 0.27385));
        }

        monoDendrite->advect();
    }
}

TEST_CASE("Post-Pre stimulation induces timing-dependent LTD in synapse (delta at higher than peak of window)", "[MonoDendriteSTDPTazerart]") {
    std::vector<std::string> inputs = {
            "neurons_0_dendtric_length  6 μm",
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

    std::unique_ptr<GlobalSimInfo> info(new GlobalSimInfo());
    info->dt = 0.001;

    std::unique_ptr<Morphology> monoDendrite(new MonoDendriteSTDPTazerart(info.get()));
    monoDendrite->LoadParameters(&inputs);

    std::shared_ptr<SynapseExt> allocationA = monoDendrite->allocateNewSynapse();

    for (long step = 205; step <= 217; step++) {
        info->time_step = step;

        if (step == 205) { // Post-spike at 0.226
            monoDendrite->recordPostSpike();
        }

        if (step == 216) { // Synapse 0 spikes at 0.205 sec
            monoDendrite->recordPreSpike(0);  // notify spike
            REQUIRE(equal(allocationA->theta, 0.0));
        }

        if (step == 217) { // check LTP in step after post spike
            REQUIRE(equal(allocationA->weight, 0.27385));
        }

        monoDendrite->advect();
    }
}

TEST_CASE("Heterosynaptic effects enhance LTP", "[MonoDendriteSTDPTazerart]") {
    std::vector<std::string> inputs = {
            "neurons_0_dendtric_length  6 μm",
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

    std::unique_ptr<GlobalSimInfo> info(new GlobalSimInfo());
    info->dt = 0.001;

    std::unique_ptr<Morphology> monoDendrite(new MonoDendriteSTDPTazerart(info.get()));
    monoDendrite->LoadParameters(&inputs);

    std::shared_ptr<SynapseExt> allocationA = monoDendrite->allocateNewSynapse();
    allocationA->theta = 0.5;

    for (long step = 205; step <= 219; step++) {
        info->time_step = step;

        if (step == 205) { // Synapse 0 spikes at 0.205 sec
            monoDendrite->recordPreSpike(0);  // notify spike
        }

        if (step == 210) { // Post-spike at 0.210
            monoDendrite->recordPostSpike();
        }

        if (step == 211) { // check LTP in step after post spike
            REQUIRE(equal(allocationA->weight, 1.3874));
        }

        monoDendrite->advect();
    }
}

TEST_CASE("Heterosynaptic effects suppress LTD", "[MonoDendriteSTDPTazerart]") {
    std::vector<std::string> inputs = {
            "neurons_0_dendtric_length  6 μm",
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

    std::unique_ptr<GlobalSimInfo> info(new GlobalSimInfo());
    info->dt = 0.001;

    std::unique_ptr<Morphology> monoDendrite(new MonoDendriteSTDPTazerart(info.get()));
    monoDendrite->LoadParameters(&inputs);

    std::shared_ptr<SynapseExt> allocationA = monoDendrite->allocateNewSynapse();

    for (long step = 205; step <= 217; step++) {
        info->time_step = step;

        if (step == 205) { // Post-spike at 0.226
            monoDendrite->recordPostSpike();
        }

        if (step == 216) { // Synapse 0 spikes at 0.205 sec
            monoDendrite->recordPreSpike(0);  // notify spike
            allocationA->theta = 0.25;
        }

        if (step == 217) { // check LTD in step after post spike
            REQUIRE(equal(allocationA->weight, 0.99510626764572851));
        }

        monoDendrite->advect();
    }
}
