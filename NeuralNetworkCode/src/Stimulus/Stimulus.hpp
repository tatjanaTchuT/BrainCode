#ifndef Stimulus_HPP
#define Stimulus_HPP

#include <algorithm>
#include <string>
#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include "../GlobalFunctions.hpp"
#include "../NeuronPopSample.hpp"

/* class Stimulus is a virtual base class for injecting a determined
 * current into each neuron during each time step.
 * - double current(int neuronId, int populationId) returns the current for the
 *   current time step
 * - void timeStepFinished() needs to be called after one time step is finished.
 *   It updates the stimulus object, if neccessary
 * - get_raw_stimulus(int neuronId, int populationId) returns some non-
 *   normalized version of the input current as specified.
 */
class Stimulus
{
protected:

    GlobalSimInfo   * info;
    NeuronPopSample * neurons;
    double          ** signal_array; //Is this what I think it is??? WTF, OPTIMIZE

public:

    Stimulus(NeuronPopSample * neur,GlobalSimInfo  * info);
    virtual ~Stimulus() { delete [] signal_array;}

    virtual void        Update(std::vector<std::vector<double>> * synaptic_dV) ;
    virtual std::string GetType() = 0;

    virtual void SaveParameters(std::ofstream * stream);
    virtual void LoadParameters(std::vector<std::string> *input){}

    double       GetSignalArray(int p,int i){return signal_array[p][i];}
};
#endif //Stimulus_HPP
