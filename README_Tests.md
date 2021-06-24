## Tests 

The parameter files for the tests are found in the "Input"-folder. Run the code as follow:
```
.\main $(path_to_output_folder) $(path_to_parameter_file) 
```
The respective commands are found in ```Input/run.sh```.

The output files can then be analyzed with the R-markdown file that can be found under  ```Test_Results/AnalyseCode``` or with files in the respective subfolders of ```Test_Results/```.

In brackets, we give rough estimates for computation times.

## Test 1: Showcase (6 secs)
Tests the basic functions of the spiking network.

To analyse, run the R-markdown in "Test_Results/Test1". For result, see [here](Test_Results/Test1/Test1Markdown.pdf).

## Test 2: Four regimes of sparse spiking network activity (4 x 1 minute)

Reproduces 4 panels of Fig. 8 in [Brunel (2000) "Dynamics of Sparsely Connected Networks of Excitatory and Inhibitory Spiking Neurons" J Comp Neurosci 8, 183](https://link.springer.com/article/10.1023/A:1008925309027).

To analyse, copy the R-files from ```Test_Results/AnalyseCode``` into the output folder and run the R-markdown file. See results here for cases [a](Test_Results/Test2/Test2A_AnalyseData.pdf), [b](Test_Results/Test2/Test2B_AnalyseData.pdf), [c](Test_Results/Test2/Test2C_AnalyseData.pdf), [d](Test_Results/Test2/Test2D_AnalyseData.pdf).

## Test 3: Small network Bistability (10 min on cluster)
Reproduces Bistability Figure (Fig3) from Ekelmans et al. manuscript; in preparation.

To analyse, run the R-markdown in "Test_Results/Test3". For result, see [here](Test_Results/Test3/Test3Markdown.pdf).

## Test 4: Working Memory (< 1 minute)

This test reproduces Fig.2 B of [Mongillo, G., Barak, O. and Tsodyks, M., 2008. Synaptic theory of working memory. Science, 319(5869), pp.1543-1546.](https://science.sciencemag.org/content/319/5869/1543) The model uses STP synapses.

To analyse, copy the R-files from ```Test_Results/AnalyseCode``` into the output folder and run the R-markdown file. For result, see [here](Test_Results/Test4/Test4_AnalyseData.pdf).


## Test 5: Transient working memory (< 6 minutes)

Reproduces Fig. 2 a (2) of [Becker, S., Nold, A. and Tchumatchenko, T., 2020. Formation and synaptic control of active transient working memory representations. bioRxiv]( https://doi.org/10.1101/2020.08.30.273995). The model employs STP synapses with a retrograde signaling pathway.

To analyse, copy the R-files from ```Test_Results/AnalyseCode``` into the output folder and run the R-markdown file. For result, see [here](Test_Results/Test5/Test5_AnalyseData.pdf).

## Test 6: Convergence to mean-field equations and balanced state (4 x 15 minutes on cluster)

Compares simulation for N = 10000...60000 with mean-field limit and balanced state results. These are four simulations.

To analyse, copy the Matlab-code ```Test_Results/Test6/MatlabAnalysis``` into the Output folder, and run the file ```Run_CheckConvergence_N.m```. For result, see [here](Test_Results/Test6/N_convergence.pdf).

## Test 7: Large network Bistability (37h on cluster)

Reproduces Fig 4 A of [Mongillo, G., Hansel, D., & Van Vreeswijk, C. (2012). "Bistability and spatiotemporal irregularity in neuronal networks with nonlinear synaptic transmission". PRL, 108(15), 158101](https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.108.158101). The model simulates a network of 25000 neurons with STP synapses.

To analyse, copy the R-files from ```Test_Results/AnalyseCode``` into the output folder and run the R-markdown file. For result, see [here](Test_Results/Test7/AnalyseData.pdf).


## Test 8: Current covariance (2 x 2h on cluster)

Reproduces Fig. 3B and 4B of [Rosenbaum, R., Smith, M.A., Kohn, A., Rubin, J.E. and Doiron, B., 2017. The spatial structure of correlated neuronal variability. Nature neuroscience, 20(1), pp.107-114](https://www.nature.com/neuro/journal/v20/n1/abs/nn.4433.html).

To analyse, run the Matlab Live Script in "Test_Results/Test8". For resuts, see [here](Test_Results/Test8/Test8.pdf).

## Test 9: Contrast invariance (2 x 5h on cluster)

Reproduces contrast invariant selectivity and nonlinear sensitivity from Bernaez Timon et al. manuscript (Fig7); in preparation.

To analyse, run the Matlab Live script in "Test_Results/Test9". For resuts, see [here](Test_Results/Test9/Test9.pdf).
