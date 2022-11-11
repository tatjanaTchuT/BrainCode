#include <iostream>
#include <vector>
#include <random>
#include <typeinfo>
#include <cstring>
#include <valarray>
#include "../GlobalFunctions.hpp"
#include "../NeuronPop/NeuronPop.hpp"
#include "../Connectivity/Connectivity.hpp"
#include "../Connectivity/RandomConnectivity.hpp"
#include "../Connectivity/BinaryRandomConnectivity.hpp"
#include "../Connectivity/DistanceConnectivity.hpp"
#include "../Connectivity/IndividualRandomConnectivity.hpp"
#ifndef SYNAPSE_HPP
#define SYNAPSE_HPP

//class RandomConnectivity;
//class Connectivity;

/*
 * class Synapse is a virtual base class for the population of synapses
 * from one neural population to another
 */

class Synapse
{
protected:

    NeuronPop       * neuronsPre,* neuronsPost; // conductance based synapses need access to membrane potential

    Connectivity    * geometry;
    GlobalSimInfo   * info;


    //Connection strength of neurons
    double        J;
    double        SigmaJ;
    double        J_pot;
    double        P_pot;
	//Synaptic delay
    int                 D_min;
    int                 D_max;
	std::vector<std::vector<double>> waiting_matrix;

    //Cumulated synaptic currents
    double cumulatedDV; // collects all synaptic currents in one time step

    //double GetCouplingStrength();
    double GetCouplingStrength(long preNeuronId, long postNeuronId);

    //virtual void advect_finalize(std::vector<double> * synaptic_dV, std::vector<std::vector<std::vector<double>>> * waiting_matrix) = 0;
	void ReadWaitingMatrixEntry(std::vector<std::vector<double>>* waiting_matrix, std::vector<double>* synaptic_dV);
	virtual void advect_finalize(std::vector<std::vector<double>> * waiting_matrix) = 0;
    virtual void advect_spikers (std::vector<double> * currents, long spiker) = 0;

    void FillWaitingMatrix(long spiker, std::vector<double> * currents, std::vector<std::vector<double>> * waiting_matrix);
	virtual void resetcumulatedDV();



public:
    Synapse(NeuronPop * postNeurons,NeuronPop * preNeurons,GlobalSimInfo * info);
    virtual ~Synapse() {}

    virtual void InitConnect(){}
    //*****************************
    //******* Get Functions *******
    //*****************************
    /**
     * Returns the sum of the coupling strength of an individual neuron
     */
    virtual int         GetNumberOfDataColumns() = 0;
    /**
     * Returns a string that can be used for the data file with the individual
     * name of each column and column number, where data_column is the first
     * column number in the data file.
     */
    virtual std::string				GetDataHeader(int data_column) = 0;
	virtual std::string				GetUnhashedDataHeader() = 0;
    virtual std::valarray<double>	GetSynapticState(int pre_neuron) = 0;

	std::valarray<double>			GetCurrentcontribution(int pre_neuron);
    unsigned long					GetNumberOfPostsynapticTargets(int pre_neuron);
	double							GetrecurrentInput(int post_neuron);
    double							GetCumulatedDV() { return cumulatedDV;}
    int								GetMaxD() {return D_max;}
    int								GetMinD() {return D_min;}
    double							GetJBase() {return J;}
    double							GetSigmaJ() { return SigmaJ; }
    double							GetJPot() {return J_pot;}
    double							GetPPot() {return P_pot;}
	double							*GetJpointer() { return &J; }
	int								*GetDpointer() { return &D_max; }
    virtual std::string				GetTypeStr() = 0;


   //*****************************
    //******* Set Functions *******
    //*****************************
    //virtual void advect(std::vector<double> * synaptic_dV, std::vector<std::vector<std::vector<double>>> * waiting_matrix);

	virtual void ResetWaitingMatrixEntry();
    virtual void advect(std::vector<double> *  synaptic_dV);
    virtual void LoadParameters(std::vector<std::string> *input);
    virtual void SaveParameters(std::ofstream * stream,std::string id_str);

	virtual void SetSeed(std::default_random_engine *generator);

    bool IsRecurrent(){ return (neuronsPre == neuronsPost);}

    unsigned long GetNoNeuronsPre()	{return this->neuronsPre->GetNoNeurons();}
    unsigned long GetNoNeuronsPost()	{return this->neuronsPost->GetNoNeurons();}
	double X_pre(long i)	{ return this->neuronsPre->get_Xpos(i); }
	double Y_pre(long i)	{ return this->neuronsPre->get_Ypos(i); }
	double X_post(long i)	{ return this->neuronsPost->get_Xpos(i); }
	double Y_post(long i)	{ return this->neuronsPost->get_Ypos(i); }
    int  GetSeed()			{return this->geometry->GetSeed();}

    virtual void ConnectNeurons();

    void SetDistributionD();
    void SetDistributionJ(); // For distribution of Js

    void WriteConnectivity(std::string filename,int noNeuronsConnectivity){this->geometry->WriteConnectivity(filename,noNeuronsConnectivity);}
    void WriteDistributionD(std::string filename,int noNeuronsDelay){this->geometry->WriteDistributionD(filename,noNeuronsDelay);}
    void WriteDistributionJ(std::string filename,int noNeuronsJPot){this->geometry->WriteDistributionJ(filename,noNeuronsJPot);}
    void Test(){geometry->Test();}
    std::string GetIdStr();
};

#endif // SYNAPSE_HPP
