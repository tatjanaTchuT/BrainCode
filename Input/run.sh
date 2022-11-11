#!/bin/bash

# Test 1: 
# Basics of neuron dynamics
./../NeuralNetworkCode/main ../Test_Results/Test1/ Test1_Parameters.txt

# Test 2: 
#Reproduces 4 panels of Fig. 8 in Brunel (2000) "Dynamics of Sparsely Connected Networks of Excitatory and Inhibitory Spiking Neurons" J Comp Neurosci 8, 183
./../NeuralNetworkCode/main ../Test_Results/Test2/ Test2A_Parameters.txt
./../NeuralNetworkCode/main ../Test_Results/Test2/ Test2B_Parameters.txt
./../NeuralNetworkCode/main ../Test_Results/Test2/ Test2C_Parameters.txt
./../NeuralNetworkCode/main ../Test_Results/Test2/ Test2D_Parameters.txt

#Test 3;
#Reproduces Bistability Figure (Fig3) from Ekelmans et al. manuscript; in preparation
./../NeuralNetworkCode/main ../Test_Results/Test3/ Test3_Parameters.txt

# Test 4:
# Reproduces Fig.2 B of Mongillo, G., Barak, O. and Tsodyks, M., 2008. Synaptic theory of working memory. Science, 319(5869), pp.1543-1546.
./../NeuralNetworkCode/main ../Test_Results/Test4/ Test4_Parameters.txt

# Test 5:
# Reproduces Fig. 2 a (2) of Becker, S., Nold, A. and Tchumatchenko, T., 2020. Formation and synaptic control of active transient working memory representations. bioRxiv.
./../NeuralNetworkCode/main ../Test_Results/Test5/ Test5_Parameters.txt

# Test 6:
# Compares simulation for N = 10000...60000 with mean-field limit and balanced state results
./../NeuralNetworkCode/main ../Test_Results/Test6/ Test6_LIFNetwork_N_10000.txt
./../NeuralNetworkCode/main ../Test_Results/Test6/ Test6_LIFNetwork_N_20000.txt
./../NeuralNetworkCode/main ../Test_Results/Test6/ Test6_LIFNetwork_N_40000.txt
./../NeuralNetworkCode/main ../Test_Results/Test6/ Test6_LIFNetwork_N_60000.txt

# Test 7:
# Reproduces Fig 4 A of Mongillo, G., Hansel, D., & Van Vreeswijk, C. (2012). "Bistability and spatiotemporal irregularity in neuronal networks with nonlinear synaptic transmission". PRL, 108(15), 158101.
./../NeuralNetworkCode/main ../Test_Results/Test7/ Test7_Parameters.txt

# Test 8:
# Reproduces Fig. 3B and 4B of Rosenbaum, R., Smith, M.A., Kohn, A., Rubin, J.E. and Doiron, B., 2017. The spatial structure of correlated neuronal variability. Nature neuroscience, 20(1), pp.107-114.
./../NeuralNetworkCode/main ../Test_Results/Test8/ Test8A_Parameters.txt
./../NeuralNetworkCode/main ../Test_Results/Test8/ Test8B_Parameters.txt

# Test 9:
# Reproduces contrast invariant selectivity and nonlinear sensitivity from Bernaez Timon et al. manuscript (Fig7); in preparation
./../NeuralNetworkCode/main ../Test_Results/Test9/ Test9A_Parameters.txt
./../NeuralNetworkCode/main ../Test_Results/Test9/ Test9B_Parameters.txt
