
#ifndef AdvancedRecorder_h
#define AdvancedRecorder_h
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <iostream>
#include <cstring>
#include <fstream>
#include <valarray>
#include <algorithm>
#include <memory>

#include "Recorder.hpp"
#include "GlobalFunctions.hpp"
#define min_(a,b) (((a)<(b)) ? (a):(b))

struct bin_data {
    std::valarray<double>  potential;       // used to compute average potential of neurons [per population]
    std::valarray<double>  spiker_ratio;      // number of spiked neurons [per population]

    std::valarray<double>  externalCurrent; // average external input current [to population P]
	std::valarray<std::valarray<double>>  synapticCurrents; //average current [to population P1][from population P2]
    //std::valarray<double>  totalCurrent_mean;             // mean of the total input current     [to population P]

    std::vector<std::vector<double>> totalCurrent_mean_N;          // mean of the total input current     [to neuron N]
    std::valarray<double>            totalCurrentSquared_mean;     // mean of the squared total input current to each neuron [averaged over population Ps]
    std::valarray<int>               spiker_pop;


    /** synaptic statistics per time step for every postsynaptic and every
     * presynaptic population and every synapse specific data column.
     */
    std::valarray<std::valarray<std::valarray<double>>>	synapticState;  // synaptic data of synapses [to population P1][from population P2][data entry j]
	std::valarray<std::valarray<double>>				Heatmap;      // firing rate [of population i][in each pixel]
    std::valarray<std::valarray<long long>>             no_recordedSynapses;
} ;

class AdvancedRecorder : public Recorder
{
protected:

    bool trackSynapses; //, writeHistogram;
	int Heatmap;
	int current_t_0 = 0;
	int raster_t_0 = 0;
    std::valarray<unsigned long>			notrackNeuronPotentials;
    std::valarray<unsigned long>		    noRasterPlotNeurons;
	std::valarray<long>						noCorrNeurons;
	std::valarray<long>						CurrentContributions;
	std::valarray<std::valarray<double>>	Densimap;      // Number of neurons [of population i][in each pixel]
	std::valarray<double>					CurrentContrBin;
    // statistics per time step
    bin_data currentBin;
    std::valarray<unsigned long> noTrackHeteroSynapsePerTrackedNeuron;

    RecorderOpenStreams FileStreams;

    unsigned long stepCount;
    unsigned long heteroRecordingPerSteps;

    bool neuronTrackingInitialized;

    void reset_statistics(); //Resets all containers.

    void Record_Potential();
    void Record_Rasterplot();
    void Record_Currents(std::vector<std::vector<double>> * synaptic_dV);
    void Record_Averages();
    void Record_SynapseStates(int header = 0);
	void Record_Heatmap();
//    void Record_Histogram(std::vector<std::vector<double>> * synaptic_dV);
    void Record_Correlations(std::vector<std::vector<double>> * synaptic_dV);
	void Record_CurrentContributions(std::vector<std::vector<double>> * synaptic_dV);
	void Record_HeteroSynapses();
    void Record_HeteroSynapsesOverall();

    void WriteDataHeader_Currents();
    void WriteDataHeader_Rasterplot();
    void WriteDataHeader_Averages();
    void WriteDataHeader_Potential();
//    void WriteDataHeader_Histogram();
    void WriteDataHeader_Correlations();
	void WriteDataHeader_Heatmap();
	void WriteDataHeader_CurrentsContribution();
	void WriteDataHeader_HeteroSynapses();
    void WriteDataHeader_HeteroSynapsesOverall();


    void SetNoRasterplotNeurons(std::vector<std::string> *values);
    void SetNoTrackNeuronPotentials(std::vector<std::string> *values);
    void SetNoCorrNeurons(std::vector<std::string> *values);
	void SetNoCurrentContribution(std::vector<std::string> *values);
    void SetNoTrackHeteroSynapseProfilesPerTrackedNeuronPerPop(std::vector<std::string> *values);


public:

    AdvancedRecorder(NeuronPopSample *ns,SynapseSample *syn,Stimulus *sti,std::string baseDir,std::vector<std::string> *input,std::string str_t,GlobalSimInfo * info);
    ~AdvancedRecorder() {}

    void WriteDataHeader();
    void writeFinalDataFile(double comp_time);
    void Record(std::vector<std::vector<double>> * synapticInput);

    std::string GetRasterplotFilename() {return this->directoryPath + title + "_Rasterplot.dat";}
    std::string GetCurrentsFilename()   {return this->directoryPath + title + "_Currents.dat";}
    std::string GetPotentialFilename()  {return this->directoryPath + title + "_Potential.dat";}
    std::string GetSynapseStateFilename()  {return this->directoryPath + title + "_Synapses.dat";}
//    std::string GetHistogramFilename()  {return this->directoryPath + title + "_Histogram.dat";}
    std::string GetMeanCorrelationsFilename() {return this->directoryPath + title + "_Correlations.dat";}
    std::string GetPairCorrelationsFilename() {return this->directoryPath + title + "_BinCorrelations" + std::to_string(info->time_step) + ".dat";}
	std::string GetCurrentCrontributionFilename() { return this->directoryPath + title + "_CurrentContribution.dat"; }
    std::string GetHeteroSynapseStateFilename() { return this->directoryPath + title + "_HeteroSynapses.dat"; }
    std::string GetOverallHeteroSynapseStateFilename() { return this->directoryPath + title + "_OverallHeteroSynapses.dat"; }
    std::string GetType()   {return str_advancedRecorder;}

    std::vector<std::vector<std::vector<double>>> savecurrents;

    void SaveParameters(std::ofstream * stream);
    void LoadParameters(std::vector<std::string> *input);
};

#endif /* AdvancedRecorder_h */
