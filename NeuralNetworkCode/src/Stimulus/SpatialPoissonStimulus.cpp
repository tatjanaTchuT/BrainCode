#include "SpatialPoissonStimulus.hpp"

/* When the stimulus has to be changed, the stimulus value at the end
 * is deleted such that at the end of next_stimulus_step there is the current
 * stimulus value.
 * The last value of next_stimulus_time_step is also deleted such that
 * at the end there is the timestep for the next stimulus change.
 */

SpatialPoissonStimulus::SpatialPoissonStimulus(NeuronPopSample *neur,
                                                                 std::vector<std::string> *input,
                                                                 GlobalSimInfo  * info):Stimulus(neur,info){

    int P             = neurons->GetTotalPopulations();
    noExternalNeurons = 1;
    table_entries     = 1000;
    seed              = -1;
	tau_syn = 0;
	exact = 0;
	J_X.resize(P);
	lengthscale.resize(P);
	PeakConnectProba.resize(P);

	for (int i = 0; i < P; i++) {
		J_X[i] = 0.0;
		lengthscale[i] = 0.0;
		PeakConnectProba[i] = 0.0;
	}

    LoadParameters(input);

	SetPositions();
	Generate_Connectivity();
	SetTableEntries();
	if (tau_syn == 0 || tau_syn<info->dt)
		attenuation = 0;
	else
		attenuation = 1-info->dt / tau_syn;
}

SpatialPoissonStimulus::~SpatialPoissonStimulus()
{
    delete [] poisson_value_table;
}

void SpatialPoissonStimulus::SetTableEntries(){

    double  dt = info->dt;
	TablePick = std::uniform_int_distribution<int>(0, table_entries - 1);
    poisson_value_table = new double[table_entries];

    double signal;

    if(next_stimulus_step.size() == 0)
        signal = 0;
    else
        signal = dt*(double)noExternalNeurons*next_stimulus_step.back();

    fill_poisson_value_table(signal);
}

inline void SpatialPoissonStimulus::UpdatePoissonTable()
{
    double  signal;
    double  dt = info->dt;
    if(!next_stimulus_time_step.empty())
    {
       while(next_stimulus_time_step.back() <= info->time_step)
        {
            next_stimulus_step.pop_back();
            next_stimulus_time_step.pop_back();
            signal = dt*(double)noExternalNeurons*next_stimulus_step.back();
            fill_poisson_value_table(signal);

            if(next_stimulus_time_step.empty())
                break;
        }
    }
}

void SpatialPoissonStimulus::Update(std::vector<std::vector<double>> * synaptic_dV)
{
    unsigned int P = this->neurons->GetTotalPopulations();
	int NoFire;//number of ExternalNeurons that fired during the timestep
	int FireSource;//selected neuron from the external population
	double c;
	int target;

	UpdatePoissonTable();

	for (unsigned int pop = 0;pop < P;pop++) {
		for (unsigned long cell = 0;cell < neurons->GetNeuronsPop(pop);cell++) {
			signal_array[pop][cell] = signal_array[pop][cell] * attenuation;
		}
	}

	NoFire = poisson_value_table[TablePick(generator)];
	FireSourcePick = std::uniform_int_distribution<int>(0, noExternalNeurons - 1);
	for (int Source_id = 0;Source_id < NoFire;Source_id++) {
		FireSource = FireSourcePick(generator);
		for (unsigned long pop = 0;pop < P;pop++) {
			c = GetExternalCouplingStrength(pop)*info->dt/tau_syn;
			for (std::size_t target_id = 0;target_id < Ext_Connectivity[FireSource][pop].size();target_id++) {
				target = Ext_Connectivity[FireSource][pop][target_id];
				signal_array[pop][target] += c;
			}
		}
	}
	Stimulus::Update(synaptic_dV);

}

void SpatialPoissonStimulus::Generate_Connectivity() {
	if (exact == 1) {
		Generate_Connectivity_Exact();
		return;
	}
	double Xe, Ye, Xi, Yi;
	double distance;
	double proba;
	double rand;
	unsigned int P = neurons->GetTotalPopulations();
	if (info->Dimensions == 0)
		return;
	RandomConnect = std::uniform_real_distribution<double>(0, 1);
	Ext_Connectivity.resize(noExternalNeurons);
	for (unsigned long ExtNeuron = 0;ExtNeuron < noExternalNeurons;ExtNeuron++) {
		Ext_Connectivity[ExtNeuron].resize(P);
		Xe = Ext_Xpos[ExtNeuron];
		Ye = Ext_Ypos[ExtNeuron];
		for (unsigned int pop = 0;pop < P;pop++) {
			for (unsigned long IntNeuron = 0;IntNeuron < neurons->GetNeuronsPop(pop);IntNeuron++) {
				Xi = neurons->GetX_Pos(pop, IntNeuron);
				Yi = neurons->GetY_Pos(pop, IntNeuron);
				proba = 0;
				for (int X_rep = -1;X_rep <= 1;X_rep++) {
					if (info->Dimensions == 1) {
						distance = pow(Xe - Xi + X_rep * info->Lx, 2);
						proba = proba + PeakConnectProba[pop] * exp(-distance / (2 * pow(lengthscale[pop], 2)));
					}
					else if (info->Dimensions == 2) {
						for (int Y_rep = -1;Y_rep <= 1;Y_rep++) {
							distance = pow(Xe - Xi + X_rep * info->Lx, 2) + pow(Ye - Yi + Y_rep * info->Ly, 2);
							proba = proba + PeakConnectProba[pop] * exp(-distance / (2 * pow(lengthscale[pop], 2)));
						}
					}
				}
				rand = RandomConnect(generator);
				if (rand<proba) {
					Ext_Connectivity[ExtNeuron][pop].push_back(IntNeuron);
				}
			}
		}
	}
}

void SpatialPoissonStimulus::Generate_Connectivity_Exact() {
	double Xe, Ye, Xi, Yi;
	long Xtarget, Ytarget, target;
	double rand_R;
	double rand_theta;
	double length;
	RandomConnect = std::uniform_real_distribution<double>(0, 1);
	int P = neurons->GetTotalPopulations();
	long Nconnect;
	long Nix;
	long Ni;
	if (info->Dimensions != 2)
		throw"Exact SpacePoisson is only available for 2D systems";
	Ext_Connectivity.resize(noExternalNeurons);
	for (unsigned long ExtNeuron = 0;ExtNeuron < noExternalNeurons;ExtNeuron++) {
		Ext_Connectivity[ExtNeuron].resize(P);
		Xe = Ext_Xpos[ExtNeuron];
		Ye = Ext_Ypos[ExtNeuron];
		for (int pop = 0;pop < P;pop++) {
			Ni = neurons->GetNeuronsPop(pop);
			Nconnect = PeakConnectProba[pop] * Ni;
			length = lengthscale[pop];
			Nix = round(sqrt(Ni));
			for (int connection = 0;connection < Nconnect;connection++) {
				rand_R = RandomConnect(generator);
				rand_theta = RandomConnect(generator);
				Xi = Xe + length * sqrt(-2 * log(rand_R))*cos(2 * (4 * atan(1))*rand_theta);
				Yi = Ye + length * sqrt(-2 * log(rand_R))*sin(2 * (4 * atan(1))*rand_theta);
				while (Xi < 0)
					Xi = Xi + info->Lx;
				while (Yi < 0)
					Yi = Yi + info->Lx;
				Xtarget = round(Xi / (info->Lx / Nix));
				Ytarget = round(Yi / (info->Lx / Nix));
				while (Xtarget > (Nix - 1))
					Xtarget = Xtarget - Nix;
				while (Ytarget > (Nix - 1))
					Ytarget = Ytarget - Nix;
				target = Xtarget + Nix * Ytarget;
				Ext_Connectivity[ExtNeuron][pop].push_back(target);
			}
			std::sort(Ext_Connectivity[ExtNeuron][pop].begin(), Ext_Connectivity[ExtNeuron][pop].end());
		}
	}
}


void SpatialPoissonStimulus::SetPositions() {
	int rows;
	int columns;
	int mod;
	Ext_Xpos.resize(noExternalNeurons);
	Ext_Ypos.resize(noExternalNeurons);
	if (info->Dimensions == 2) {
		rows = ceil(sqrt(noExternalNeurons));
		columns = floor(noExternalNeurons / rows);
		mod = noExternalNeurons - rows * columns;//see NeuronPop::SetPosition()
		for (int i = 0; i < mod*(columns + 1); i++) {
			Ext_Xpos[i] = (info->Lx / (columns + 1))*(i % (columns + 1));
			Ext_Ypos[i] = (info->Ly / rows) *floor(i / (columns + 1));
		}
		for (unsigned long i = (mod + mod * columns); i < noExternalNeurons; i++) {
			Ext_Xpos[i] = (info->Lx / (columns))*((i - mod) % columns);
			Ext_Ypos[i] = (info->Ly / rows) *floor((i - mod) / (columns));
		}
	}
	else if (info->Dimensions == 1) {
		for (unsigned long i = 0;i < noExternalNeurons; i++) {
			Ext_Xpos[i] = i * info->Lx / noExternalNeurons;
		}
	}
}

// fills the poisson_value_table that is a tabular version of the poisson distribution
// with the needed mean that is firing_rate*dt*number_of_neurons
inline void SpatialPoissonStimulus::fill_poisson_value_table(double mu){

	int value = 0;
	double probability = exp(-mu);
	double cumulated_probability = exp(-mu);

	for (int i = 0; i < table_entries; i++)
	{
		if (cumulated_probability < double(i) / double(table_entries))
		{
			value++;
			probability = probability * mu / double(value);
			cumulated_probability += probability;
		}
		poisson_value_table[i] = double(value);
	}
}

void SpatialPoissonStimulus::AddStimulusStep(double ts,double sS){

    if(next_stimulus_step.empty())
    {
      next_stimulus_time_step.push_back(ts/info->dt);
      next_stimulus_step.push_back(sS);
    }
    else
    {
      if(ts/info->dt > next_stimulus_time_step.back())
      {
        std::vector<double> temp_time_step(next_stimulus_time_step);
        std::vector<double> temp_stimulus(next_stimulus_step);

        next_stimulus_time_step.clear();
        next_stimulus_step.clear();

        bool done_flag = false;
        for(unsigned i = 0; i < temp_time_step.size(); i++)
        {
          if(!done_flag && (ts/info->dt > temp_time_step[i]))
          {
            next_stimulus_time_step.push_back(ts/info->dt);
            next_stimulus_step.push_back(sS);
            done_flag = true;
          }
          next_stimulus_time_step.push_back(temp_time_step[i]);
          next_stimulus_step.push_back(temp_stimulus[i]);
        }
        if(next_stimulus_time_step.size() == temp_time_step.size())
        {
          next_stimulus_time_step.push_back(ts/info->dt);
          next_stimulus_step.push_back(sS);
        }
      }
      else
      {
        next_stimulus_time_step.push_back(ts/info->dt);
        next_stimulus_step.push_back(sS);
      }
    }
}

void SpatialPoissonStimulus::SaveParameters(std::ofstream * stream){

    int P        = neurons->GetTotalPopulations();
    Stimulus::SaveParameters(stream);

    *stream <<  "stimulus_noExternalNeurons           " << std::to_string(noExternalNeurons)  << "\n";
    if(info->globalSeed == -1){
        *stream <<  "stimulus_seed                        " << std::to_string(seed)  << "\n";
    }
	*stream <<  "stimulus_ExtConnect_lengthscale      ";
	for (int i = 0;i<P;i++)
		*stream << std::to_string(lengthscale[i]) << "\t";
	*stream << "mm\n";
	*stream << "stimulus_ExtConnect_PeakProbability  ";
	for (int i = 0;i<P;i++)
		*stream << std::to_string(PeakConnectProba[i]) << "\t";
	*stream << "\n";

    *stream <<  "stimulus_J_X                         ";
    for(int i = 0;i<P;i++)
        *stream << std::to_string(J_X[i]) << "\t";
    *stream << "dmV/Spike\n";

    for(int i = 0; i < GetStimulusNoSteps(); i++) {
        *stream <<  "stimulus_step                        " << std::to_string(GetStimulusStep_Time(i)*info->dt)  << "\t"
        << std::to_string(GetStimulusStep(i))  << "\t[t (secs.) -- Hz]\n";
    }

	*stream << "stimulus_tau_syn                     " << std::to_string(tau_syn) << "\t\t\t\ts\n";
	*stream << "stimulus_ExactConnections            " << std::to_string(exact) << "\t\t\t\t\t\t(0/1)\n";

    *stream <<  "#\t\t" << str_uncorrelatedStimulus << ": noExternalNeurons external neurons with (poisson) firing rate defined in stimulus_step are connected with population X with strength J_X using a distance-dependant connectivity.\n";
}

void SpatialPoissonStimulus::LoadParameters(std::vector<std::string> *input){

    Stimulus::LoadParameters(input);
    std::string              name,token;
    std::vector<std::string> values;
    int                      P = neurons->GetTotalPopulations();

	for (std::vector<std::string>::iterator it = (*input).begin(); it != (*input).end(); ++it) {

		SplitString(&(*it), &name, &values);

		if ((name.find("virtualExternalNeurons") != std::string::npos) ||
			(name.find("noExternalNeurons") != std::string::npos)) {
			this->noExternalNeurons = std::stoi(values.at(0));
		}
		else if (name.find("seed") != std::string::npos) {
			SetSeed(std::stod(values.at(0)));
		}
		else if (name.find("stimulus_step") != std::string::npos) {
			AddStimulusStep(std::stod(values.at(0)), std::stod(values.at(1)));
		}
		//        else if((name.find("Poisson_table_entries") != std::string::npos) ||
		//                (name.find("PoissonTableEntries") != std::string::npos)){
		//            this->table_entries = std::stod(values.at(0));
		//        }
		else if (name.find("J_X") != std::string::npos) {
			for (int i = 0;i < min_(P, (int)values.size());i++)
				this->J_X[i] = std::stod(values.at(i));
		}
		else if (name.find("PeakProba") != std::string::npos) {
			for (int i = 0;i < min_(P, (int)values.size());i++)
				this->PeakConnectProba[i] = std::stod(values.at(i));
		}
		else if (name.find("lengthscale") != std::string::npos) {
			for (int i = 0;i < min_(P, (int)values.size());i++)
				this->lengthscale[i] = std::stod(values.at(i));
		}
		else if (name.find("tau_syn") != std::string::npos) {
			this->tau_syn = std::stod(values.at(0));
		}
		else if (name.find("ExactConnections") != std::string::npos) {
			this->exact = std::stoi(values.at(0));
		}
		if (info->globalSeed != -1) {
			std::uniform_int_distribution<int> distribution(0, INT32_MAX);
			seed = distribution(info->globalGenerator);
		}
		else {
			std::cout << "please use a positive globalSeed\n\n     Thank you <3";
			throw "SpacePoisson requires GlobalSeed";
		}
	}
}
