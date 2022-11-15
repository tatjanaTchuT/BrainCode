#include "DistanceConnectivity.hpp"
#include "../Synapse/Synapse.hpp"
#include "time.h"

DistanceConnectivity::DistanceConnectivity(Synapse * syn,GlobalSimInfo  * info):Connectivity(syn,info){
	PeakProbability = 0;
	StdProbability = 1;
	exact = 0;
    SetSeed(0);
	a = 1;
}

void DistanceConnectivity::SaveParameters(std::ofstream * stream,std::string id_str){
    Connectivity::SaveParameters(stream,id_str);
    *stream << id_str << "connectivity_ConnectionProba\t" << std::to_string(this->PeakProbability) << "\n";
	*stream << id_str << "connectivity_StdProbability\t" << std::to_string(this->StdProbability) << "\tmm\n";
	*stream << id_str << "connectivity_ExactConnections\t" << std::to_string(this->exact) << "\t\t\t(0/1)\tIf 1, each neuron will receive exactly C connections. \n";
	if (info->Dimensions == 2) {
		*stream << id_str << "connectivity_AsymmetryFactor\t" << std::to_string(this->a) << "\t# sigma_y=sigma/AF. If AF is 1, the projection length is the same in all directions\n";}
	if (info->globalSeed == -1) {
		std::cout << "Please set globalseed >= 0 to use DistanceConnectivity";
		throw "Globalseed is -1";
	}
	//*stream << "# If ExactConnections==1, neurons have a predefined number of connections randomly generatd from its neighbours . The number of connections is ConnectProba*N.\n";
	//*stream << "# If ExactConnections==0, the connection probability between each pair of neurons is calculated independantly. There is no repeat, and the number of presynaptic connections is random. ConnectProba defines the peak probability of connection (at zero distance))\n";
    //*stream << "# "<< str_distanceConnectivity << ": Each neuronal pair is connected with probability depending on their distance. (as used by [Chariker et al, 2016]). \n";
    }

void DistanceConnectivity::LoadParameters(std::vector<std::string> *input){
    Connectivity::LoadParameters(input);

    std::string name;
    std::vector<std::string> values;

    for(std::vector<std::string>::iterator it = (*input).begin(); it != (*input).end(); ++it) {
        SplitString(&(*it),&name,&values);
		if(name.find("ConnectionProba") != std::string::npos || name.find("ConnectProba") != std::string::npos){
            this->PeakProbability = std::stod(values.at(0));
        }
		else if(name.find("StdProbability") != std::string::npos) {
			this->StdProbability = std::stod(values.at(0));
		}
		else if (name.find("ExactConnections") != std::string::npos) {
			this->exact = std::stoi(values.at(0));
		}
		else if (name.find("AsymmetryFactor") != std::string::npos) {
			this->a = std::stoi(values.at(0));
		}
    }
}

void DistanceConnectivity::ConnectNeurons()
{
	if ((info->Dimensions != 2) && (info->Dimensions != 1)) {
		std::cout << "Dimensions is not valid for DistanceConnectivity !\n";
		throw "Dimensions is not valid for DistanceConnectivity !\n";
	}
	if (exact==1) {
		ConnectNeuronsExact();
		return;
	}
	double  P;//probability of connecting neurons Source and Target
	double  p;//randomly generated number to be compared with P
	double dist;//Euclidian distance squared
    long    noPost = synapse->GetNoNeuronsPost();
	long    noPre = synapse->GetNoNeuronsPre();
	double x_pr, y_pr, x_ps, y_ps;
    long    output_Interval = noPost/10;
	int c=0;

    if(output_Interval == 0) {
		output_Interval = 1;
	}

	std::uniform_real_distribution<double> distribution(0,1);

	if (StdProbability > 0.29*info->Lx*std::min(1.0, a))//with std=0.29, the periodic boundaries cause an increase in connection probability of 1% at the center of the domain
		std::cout << "\n \nWarning sigma_ConnectionProbability is high compared to the size of the system \n";
    //Iterate through all target neurons

    for(long target = 0; target < noPost; target++){
        //Iterate through all source neurons
		x_ps = synapse->X_post(target);
		y_ps = synapse->Y_post(target);

        for(long source = 0; source < noPre; source++){
            //Connect with given probability
			x_pr = synapse->X_pre(source);
			y_pr = synapse->Y_pre(source);

            p = distribution(generator);
			P = 0;
			for (int x_repeat = -1 ; x_repeat<=1 ; x_repeat++) { //the motif in which neurons are located is repeated 8 other times to prevent boundary effects
				if (info->Dimensions == 1) {
					dist = pow((x_repeat*info->Lx) + x_pr - x_ps, 2);//d^2
					P = P + PeakProbability * exp(-dist / (2 * pow(StdProbability,2)));
				}
				else if (info->Dimensions == 2) {
					for (int y_repeat = -1; y_repeat <= 1; y_repeat++) {
						dist = pow((x_repeat * info->Lx) + x_pr - x_ps, 2) + pow(a,2)*pow((y_repeat*info->Ly) + y_pr - y_ps, 2);
						P = P + PeakProbability * exp(-dist / (2 * pow(StdProbability, 2)));
					}
				}
			}
			if ((x_ps == x_pr) && (y_ps == y_pr) && (source==target)){
				P = 0;
			}
            if(p <= P){
                target_id[source].push_back(target);
				c++;
            }
        }
        if(target%output_Interval == 0)
            std::cout << 100*(target)/noPost << "%-";
    }
    std::cout << "100%\n";
	std::cout << " Average Number of Presynaptic neurons : " << std::to_string(c / noPost) << "\n\n";

	if (StdProbability > 0.29*info->Lx*std::min(1.0, a)) {
		std::cout << "Expected number of presynaptic neurons :" << std::to_string(GetNumberAverageSourceNeurons());
		std::cout << " | Actual result : " << std::to_string(c / noPost) << "\n\n";
	}
}

void DistanceConnectivity::ConnectNeuronsExact()
{
	//variation of Connect Neuron, in which the number of neurons connected is set
	//Similar to RandomConnectivity vs BinaryConnectivity
	//can only be used if the number of neurons is a square for each population (sqrt(N_pre) is integer and, sqrt(N_post) is integer)

	unsigned long    noPost = synapse->GetNoNeuronsPost();
	unsigned long    noPre = synapse->GetNoNeuronsPre();
	long	connection;
	double	x_pr, y_pr, x_ps, y_ps;
	long	output_Interval = noPost / 10;
	long	Xsource, Ysource;
	unsigned long	source;
	double	randdistance;
	double	randtheta;
	long	Nconnect = lround((noPre - 1) * PeakProbability);
	long	NxPre = noPre;
	double	DXsource=info->Lx/NxPre;

	if (info->Dimensions == 2) {
		//lround() returns a long integer
		NxPre = lround(sqrt(noPre));//in 2D the distance between rows is different
		DXsource = info->Lx / NxPre;
		if (0.86*Nconnect > (4 * atan(1))*pow(2*StdProbability, 2)*noPre / (a*pow(info->Lx,2)))//86% of the Gaussian distribution should fall within the ellipse of radii 2sigma, 2sigma/a.
			std::cout << "WARNING !! \n Connectivity pattern cannot follow a Gaussian pdf: sigma too low, density too low or Connection_probability too high\n";
	}else if (0.86*Nconnect > 3*StdProbability*noPre / info->Lx)//86% of the Gaussian distribution should fall within the boundary [-1.5sigma 1.5 sigma].
			std::cout << "WARNING !! \n Connectivity pattern cannot follow a Gaussian pdf: sigma too low, density too low or Connection_probability too high\n";

	if (pow(NxPre, 2) != noPre && info->Dimensions == 2) {

		std::cout << "N_pre and N_Post must be exact squares in order to put the neurons on a regular grid in 2D";
		throw "N_pre and N_Post must be exact squares in order to put the neurons on a regular grid in 2D";
	}

	if (output_Interval == 0)
		output_Interval = 1;

	std::uniform_real_distribution<double> distribution(0, 1);

	for (unsigned long target = 0; target < noPost; target++) {
		//Iterate through all source neurons
		x_ps = synapse->X_post(target);
		y_ps = 0;
		if (info->Dimensions==2)
			y_ps = synapse->Y_post(target);
		connection = 0;
		while (connection < Nconnect) {

			randdistance = distribution(generator);
			randtheta = distribution(generator);
			x_pr = StdProbability * sqrt(-2 * log(randdistance))*cos(2 * (4 * atan(1))*randtheta) + x_ps;//Box Muller transform
			while (x_pr < 0)
				x_pr = x_pr + info->Lx;
			Xsource = lround(x_pr / DXsource);
			while (Xsource >(NxPre - 1))
				Xsource = Xsource - NxPre;
			if (info->Dimensions == 2) {
				y_pr = StdProbability/a * sqrt(-2 * log(randdistance))*sin(2 * (4 * atan(1))*randtheta) + y_ps;
				while (y_pr < 0)
					y_pr = y_pr + info->Lx;
				Ysource = lround(y_pr / DXsource);
				while (Ysource >(NxPre - 1))
					Ysource = Ysource - NxPre;
				source = Xsource + NxPre * Ysource;
			}
			else //1D
				source = Xsource;

			if ((synapse->IsRecurrent()) && (source == target))
				continue;

			if (!target_id[source].empty() && target_id[source].back() == target)
				continue;

			target_id[source].push_back(target);
			connection++;
		}

		if (target%output_Interval == 0)
			std::cout << 100 * (target) / noPost << "%-";
	}
	std::cout << "100%\n";
}

unsigned long  DistanceConnectivity::GetNumberAverageSourceNeurons() {
	if (exact == 1)
		return static_cast<unsigned long>(synapse->GetNoNeuronsPre() * PeakProbability);
	else {
		if (info->Dimensions == 2)
			return static_cast<unsigned long>((synapse->GetNoNeuronsPre() / (double)info->N) * 2 * (4 * atan(1)) * info->density * PeakProbability * pow(StdProbability, 2) / a);//integral of density*probability(x,y)dxdy over the infinite 2D space
		else if (info->Dimensions == 1)
			return static_cast<double>((synapse->GetNoNeuronsPre() / (double)info->N) * sqrt(2 * (4 * atan(1))) * info->density * PeakProbability * StdProbability);//integral of density*probability(x,y)dxdy over the infinite 1D space
        else{
            throw "ERROR DistanceConnectivity::GetNumberAverageSourceNeurons";
        }
	}
}

// Based on the method of Chariker et al, 2016; and the repeating pattern as in Rosenbaum and Doiron, 2014
