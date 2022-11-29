#include "UncorrelatedPoissonLikeStimulus.hpp"

/* When the stimulus has to be changed, the stimulus value at the end
 * is deleted such that at the end of next_stimulus_step there is the current
 * stimulus value.
 * The last value of next_stimulus_time_step is also deleted such that
 * at the end there is the timestep for the next stimulus change.
 */

UncorrelatedPoissonLikeStimulus::UncorrelatedPoissonLikeStimulus(NeuronPopSample *neur,
                                                                 std::vector<std::string> *input,
                                                                 GlobalSimInfo  * info):Stimulus(neur,info){

    int P             = neurons->GetTotalPopulations();
    noExternalNeurons = 1;
    table_entries     = 10;
    J_X               = new double[P];
    seed              = -1;

    for(int i = 0; i < P; i++)
        J_X[i] = 0.0;

    LoadParameters(input);
    SetTableEntries();
}

UncorrelatedPoissonLikeStimulus::~UncorrelatedPoissonLikeStimulus()
{
    delete [] poisson_value_table;
    delete [] J_X;
}

void UncorrelatedPoissonLikeStimulus::SetTableEntries(){

    double  dt = info->dt;
    distribution        = std::uniform_int_distribution<int>(0,table_entries-1);
    poisson_value_table = new double[table_entries];

    double signal;

    if(next_stimulus_step.size() == 0)
        signal = 0;
    else
        signal = dt*(double)noExternalNeurons*next_stimulus_step.back();

    fill_poisson_value_table(signal);
}

inline void UncorrelatedPoissonLikeStimulus::UpdatePoissonTable()
{
    double  signal;
    double  dt = info->dt;
    if(!next_stimulus_time_step.empty())
    {
       while(next_stimulus_time_step.back() <= info->time_step)
        {
            next_stimulus_step.pop_back();
            next_stimulus_time_step.pop_back();
            signal = dt*static_cast<double>(noExternalNeurons)*next_stimulus_step.back();
            fill_poisson_value_table(signal);

            if(next_stimulus_time_step.empty())
                break;
        }
    }
}

void UncorrelatedPoissonLikeStimulus::Update(std::vector<std::vector<double>> * synaptic_dV)
{
    int P = this->neurons->GetTotalPopulations();
    double c;

    UpdatePoissonTable();

    for(int pop = 0;pop < P;pop++){
        c       = GetExternalCouplingStrength(pop);
        for(unsigned int i = 0;i<neurons->GetNeuronsPop(pop);i++)
            signal_array[pop][i]   = c*poisson_value_table[distribution(generator)];
    }

    Stimulus::Update(synaptic_dV);
}


// fills the poisson_value_table that is a tabular version of the poisson distribution
// with the needed mean that is firing_rate*dt*number_of_neurons
inline void UncorrelatedPoissonLikeStimulus::fill_poisson_value_table(double mu)
{

	int value                    = 0;
	double probability           = exp(-mu);
	double cumulated_probability = exp(-mu);

	for(int i = 0; i < table_entries; ++i)
	{
		if(cumulated_probability < static_cast<double>(i)/static_cast<double>(table_entries))
		{
			++value;
			probability            = probability*mu/static_cast<double>(value);
			cumulated_probability += probability;
		}
		poisson_value_table[i] = static_cast<double>(value);
	}
}

void UncorrelatedPoissonLikeStimulus::AddStimulusStep(int ts,double sS){
    long	Time_step{ static_cast<long>(std::round(ts / info->dt)) };
    if(next_stimulus_step.empty())
    {
      next_stimulus_time_step.push_back(Time_step);
      next_stimulus_step.push_back(sS);
    }
    else
    {
      if(Time_step > next_stimulus_time_step.back())
      {
        std::vector<double> temp_time_step(next_stimulus_time_step);
        std::vector<double> temp_stimulus(next_stimulus_step);

        next_stimulus_time_step.clear();
        next_stimulus_step.clear();

        bool done_flag = false;
        for(unsigned i = 0; i < temp_time_step.size(); i++)
        {
          if(!done_flag && (Time_step > temp_time_step[i]))
          {
            next_stimulus_time_step.push_back(Time_step);
            next_stimulus_step.push_back(sS);
            done_flag = true;
          }
          next_stimulus_time_step.push_back(temp_time_step[i]);
          next_stimulus_step.push_back(temp_stimulus[i]);
        }
        if(next_stimulus_time_step.size() == temp_time_step.size())
        {
          next_stimulus_time_step.push_back(Time_step);
          next_stimulus_step.push_back(sS);
        }
      }
      else
      {
        next_stimulus_time_step.push_back(Time_step);
        next_stimulus_step.push_back(sS);
      }
    }
}

void UncorrelatedPoissonLikeStimulus::SaveParameters(std::ofstream * stream){

    int P        = neurons->GetTotalPopulations();
    Stimulus::SaveParameters(stream);

    *stream <<  "stimulus_noExternalNeurons           " << std::to_string(noExternalNeurons)  << "\n";
    if(info->globalSeed == -1){
        *stream <<  "stimulus_seed                        " << std::to_string(seed)  << "\n";
    }
    *stream <<  "stimulus_PoissonTableEntries         " << std::to_string(GetTable_entries())  << "\n";
    *stream <<  "stimulus_J_X                         ";
    for(int i = 0;i<P;i++)
        *stream << std::to_string(J_X[i]) << "\t";
    *stream << " dmV/Spike\n";

    for(int i = 0; i < GetStimulusNoSteps(); i++)
        *stream <<  "stimulus_step                        " << std::to_string(GetStimulusStep_Time(i)*info->dt)  << "\t"
        << std::to_string(GetStimulusStep(i))  << "\t [t (secs.) -- Hz]\n";

    *stream <<  "#\t\t" << str_uncorrelatedStimulus << ": noExternalNeurons external neurons with (poisson) firing rate defined in stimulus_step are connected with population X with strength J_X.\n";
}

void UncorrelatedPoissonLikeStimulus::LoadParameters(std::vector<std::string> *input){

    Stimulus::LoadParameters(input);
    std::string              name,token;
    std::vector<std::string> values;
    int                      P = neurons->GetTotalPopulations();

    for(std::vector<std::string>::iterator it = (*input).begin(); it != (*input).end(); ++it) {

        SplitString(&(*it),&name,&values);

        if((name.find("virtualExternalNeurons") != std::string::npos) ||
           (name.find("noExternalNeurons") != std::string::npos)){
            this->noExternalNeurons = std::stoi(values.at(0));
        }
        else if(name.find("seed") != std::string::npos){
            SetSeed(static_cast<int>(std::stod(values.at(0))));
        }
        else if(name.find("stimulus_step") != std::string::npos){
            AddStimulusStep(static_cast<int>(std::stod(values.at(0))), std::stod(values.at(1)));
        }
        else if((name.find("Poisson_table_entries") != std::string::npos) ||
                (name.find("PoissonTableEntries") != std::string::npos)){
            this->table_entries = static_cast<int>(std::stod(values.at(0)));
        }
        else if(name.find("J_X") != std::string::npos){
            for(int i = 0;i < min_(P,(int)values.size());i++)
                this->J_X[i] = std::stod(values.at(i));
        }

    }

    if(info->globalSeed != -1){
        std::uniform_int_distribution<int> distribution(0,INT32_MAX);
        seed = distribution(info->globalGenerator);
    }


}
