
#include "AdvancedRecorder.hpp"


AdvancedRecorder::AdvancedRecorder(NeuronPopSample *ns, SynapseSample *syn, Stimulus *sti, std::string baseDir, std::vector<std::string> *input, std::string str_t, GlobalSimInfo * info) :Recorder(ns, syn, sti, baseDir, input, str_t, info)
{
	trackSynapses = false;
//	writeHistogram = false;

	Heatmap = 0;

	unsigned int P = neurons->GetTotalPopulations();

	noRasterPlotNeurons.resize(P);
	notrackNeuronPotentials.resize(P);
	noCorrNeurons.resize(P);
	CurrentContributions.resize(P);
    noTrackHeteroSynapsePerTrackedNeuron.resize(P);

	currentBin.potential.resize(P);
	currentBin.spiker_ratio.resize(P);
	currentBin.externalCurrent.resize(P);
	currentBin.synapticState.resize(P);
	currentBin.synapticCurrents.resize(P);
	currentBin.totalCurrentSquared_mean.resize(P);
	currentBin.no_recordedSynapses.resize(P);

	currentBin.totalCurrent_mean_N.resize(P);
	for (unsigned int i = 0; i < P; i++)
	{
		noRasterPlotNeurons[i] = 0;
		notrackNeuronPotentials[i] = 0;
		noCorrNeurons[i] = 0;
		currentBin.no_recordedSynapses[i].resize(P);
		currentBin.synapticState[i].resize(P);
		currentBin.synapticCurrents[i].resize(P);
		currentBin.totalCurrent_mean_N.at(i).resize(neurons->GetNeuronsPop(i), 0);
		noTrackHeteroSynapsePerTrackedNeuron[i] = 0;
	}

	LoadParameters(input);
	CurrentContrBin.resize((P + 1)*CurrentContributions.sum());
	savecurrents.resize(noCorrNeurons[0]);
	for (int i = 0; i < noCorrNeurons[0]; i++) {
		savecurrents[i].resize(averaging_steps);
		for (int j = 0; j < averaging_steps; j++) {
			savecurrents[i][j].resize(P);
		}
	}
	if (Heatmap != 0) {
		Densimap.resize(P);
		currentBin.Heatmap.resize(P);
		for (unsigned int pop = 0;pop < P;pop++) {
            size_t heatmapDimensions{static_cast<size_t>(pow(Heatmap, info->Dimensions))};
            currentBin.Heatmap[pop].resize(heatmapDimensions);
			Densimap[pop].resize(heatmapDimensions);
			if (info->Dimensions == 2) {
				for (unsigned long cell = 0;cell < neurons->GetNeuronsPop(pop);cell++)
					Densimap[pop][static_cast<size_t>(Heatmap*(floor(neurons->GetY_Pos(pop, cell) * Heatmap / info->Ly)) + floor(neurons->GetX_Pos(pop, cell) * Heatmap / info->Lx))] += 1;
			}
			else if (info->Dimensions == 1) {
				for (unsigned long cell = 0;cell < neurons->GetNeuronsPop(pop);cell++)
					Densimap[pop][static_cast<size_t>(floor(neurons->GetX_Pos(pop, cell) * Heatmap / info->Lx))] += 1;
			}
		}
	}
	if (trackSynapses) {
		for (unsigned i = 0; i < P; i++) {
			for (unsigned j = 0; j < P; j++)
				currentBin.synapticState[i][j].resize(this->synapses->GetNumberOfDataColumns(i, j));
		}
	}
}

void AdvancedRecorder::SaveParameters(std::ofstream * stream){
    Recorder::SaveParameters(stream);

    double  dt = info->dt;
    *stream << "recorder_binSize                   " << std::to_string(GetAveragingSteps()*dt)  << " seconds \t#Bin size over which data saved in main recording data file is average over\n";
    *stream << "recorder_noRasterPlotNeurons       ";
    for(unsigned i = 0; i < noRasterPlotNeurons.size();i++) {
		*stream << std::to_string(noRasterPlotNeurons[i]) << " \t";
	}
	*stream << std::to_string(((double)raster_t_0)*info->dt) << "\t";
    *stream << "#Record spike times of x neurons for (i-th column is x for the i-th population). The i+1-th column sets the initial recording time\n";

    *stream << "recorder_notrackNeuronProfiles     ";
    for(unsigned i = 0; i < notrackNeuronPotentials.size();i++)
        *stream << std::to_string(notrackNeuronPotentials[i]) << " \t";
    *stream << "\t\t\t#Record currents and potentials at all time steps of the first x_p neurons, p = population index. [column 1: track #neurons in pop1, column 2: track #neurons in pop2, .. ]\n";

    *stream << "recorder_noCorrNeurons             ";
    for(unsigned i = 0; i < noCorrNeurons.size();i++)
        *stream << std::to_string(noCorrNeurons[i]) << " \t";
    *stream << "\t\t\t#Record correlations between first x_p neurons for each bin size. p = population index. [column 1: #neurons in pop1, column 2: track #neurons in pop2, .. ]\n";

	*stream << "recorder_CurrentContributions      ";
	for (unsigned i = 0; i < CurrentContributions.size();i++)
		*stream << std::to_string(CurrentContributions[i])<<"\t";
	*stream << std::to_string(((double)current_t_0)*info->dt) << "\t";
	*stream << "#Record the sources of input current to x neurons. (i-th column is x for the i-th population). The i+1-th column sets the initial recording time\n";

    *stream <<  "recorder_trackSynapses             " << std::to_string(trackSynapses)  << "\t\t\t\t\t#Set = 1 to track averaged data from synapes, Set = 0 to ignore.\n";
    // *stream <<  "recorder_Histogram                 " << std::to_string(writeHistogram) << "\t\t\t\t\t#Set = 0 to ignore. Option under construction.\n";
	*stream <<  "recorder_Heatmap                   " << std::to_string(Heatmap) << "\t\t\t\t\t#Number of bins used to represent each dimension of the spatial domain in the firing rates Heatmap\n";
}

void AdvancedRecorder::LoadParameters(std::vector<std::string> *input){
    std::string              name,token;
    std::vector<std::string> values;

    for(std::vector<std::string>::iterator it = (*input).begin(); it != (*input).end(); ++it) {

        SplitString(&(*it),&name,&values);

		if (name.find("recorder_binSize") != std::string::npos) {
            SetAveraging_steps(std::stod(values.at(0)));
        } else if (name.find("recorder_noRasterPlotNeurons") != std::string::npos) {
            SetNoRasterplotNeurons(&values);
		} else if (name.find("recorder_trackSynapses") != std::string::npos) {
            trackSynapses = std::stoi(values.at(0));
		} else if ((name.find("recorder_notrackNeuronPotentials") != std::string::npos) ||
			(name.find("recorder_notrackNeuronProfiles") != std::string::npos)) {
			SetNoTrackNeuronPotentials(&values);
		} else if (name.find("recorder_noCorrNeurons") != std::string::npos) {
            SetNoCorrNeurons(&values);
		} //else if (name.find("recorder_Histogram") != std::string::npos) {
            //	writeHistogram = std::stoi(values.at(0));
		// }
		else if (name.find("recorder_Heatmap") != std::string::npos) {
            Heatmap = std::stoi(values.at(0));
		} else if (name.find("recorder_CurrentContributions") != std::string::npos) {
            SetNoCurrentContribution(&values);
        } else if (name.find("recorder_notrackHeteroSynapseProfiles") != std::string::npos) {
            SetNoTrackHeteroSynapseProfilesPerTrackedNeuronPerPop(&values);
		}
    }

    Recorder::LoadParameters(input);
}

void AdvancedRecorder::SetNoTrackNeuronPotentials(std::vector<std::string> *values){
    unsigned int P = neurons->GetTotalPopulations();
    for(unsigned int i = 0; i < min_(P, values->size());i++){
        notrackNeuronPotentials[i] = std::stoul(values->at(i));
        if((notrackNeuronPotentials[i] >= neurons->GetNeuronsPop(i)) ||
            (notrackNeuronPotentials[i] < 0)){
            std::cout << "Potentials: Tracking all neurons of population" << i << "\n";
            notrackNeuronPotentials[i] = neurons->GetNeuronsPop(i);
        }
    }
}

void AdvancedRecorder::SetNoCurrentContribution(std::vector<std::string> *values) {
	unsigned int P = neurons->GetTotalPopulations();
	for (unsigned int i = 0;i < min_(P, values->size());i++) {
		CurrentContributions[i] = min_(neurons->GetNeuronsPop(i), std::stoul(values->at(i)));
		if (CurrentContributions[i] < 0) {
			std::cout << "Current contribution of all neurons in population" << i << "\n";
			CurrentContributions[i] = neurons->GetNeuronsPop(i);
		}
	}
	if (values->size() > P) {
		if (values->at(P)[0] != '#')// The comments in the code are not meant to be read upon execution
			current_t_0 = static_cast<int>(std::round(std::stod(values->at(P)) / info->dt));
	}
}

void AdvancedRecorder::SetNoRasterplotNeurons(std::vector<std::string> *values){
    unsigned int P = neurons->GetTotalPopulations();
    for(unsigned int i = 0; i < min_(P, values->size());i++){
        noRasterPlotNeurons[i] = std::stoul(values->at(i));
        if((noRasterPlotNeurons[i]  > neurons->GetNeuronsPop(i)) ||
           (noRasterPlotNeurons[i] < 0)){
            std::cout << "Rasterplot: Tracking all neurons of population "<< i << "\n";
            noRasterPlotNeurons[i]  = neurons->GetNeuronsPop(i);
        }
    }
	if (values->size() > P) {
		if (values->at(P)[0] != '#')
			raster_t_0 = static_cast<int>(std::round(std::stod(values->at(P)) / info->dt));
	}
}

void AdvancedRecorder::SetNoCorrNeurons(std::vector<std::string> *values){
    int P = (int)neurons->GetTotalPopulations();
    for(int i = 0; i < min_(P,(int)values->size());i++){
        noCorrNeurons[i] = std::stoi(values->at(i));
        if ((static_cast<unsigned long>(noCorrNeurons[i]) > neurons->GetNeuronsPop(i)) ||
           (noCorrNeurons[i] < 0)){
            std::cout << "Correlations: Tracking all neurons of population "<< i << "\n";
            noCorrNeurons[i]  = neurons->GetNeuronsPop(i);
        }
    }
}

void AdvancedRecorder::SetNoTrackHeteroSynapseProfilesPerTrackedNeuronPerPop(std::vector<std::string> *values) {
    int P = (int)neurons->GetTotalPopulations();
    for(int i = 0; i < min_(P,(int)values->size());i++){
        noTrackHeteroSynapsePerTrackedNeuron[i] = std::stoi(values->at(i));
    }
}


void AdvancedRecorder::WriteDataHeader_Heatmap() {
	if (Heatmap == 0)
		return;
	std::ofstream file;
	int P = this->neurons->GetTotalPopulations();
	int dim = info->Dimensions;
	if (dim != 1 && dim != 2) {
		std::cout << "Heatmap can only be used in 1D or 2D\n";
		throw "Heatmap can only be used in 1D or 2D\n";
	}


	for (int pop = 0;pop < P;pop++) {
		file.open(this->GetHeatmapFilename() + std::to_string(pop)+".dat", std::ofstream::out | std::ofstream::trunc);
		WriteHeader(&file);
		file << "# Population:" + std::to_string(pop) + "\n";
		file << "# Dimension:" + std::to_string(dim) + "\n";
		file << "# L=" + std::to_string(info->Lx) + "\n";
		file << "#************************************\n";
		if (dim == 1) {
			file << "# column 1 : t (secs.) \n";
			file << "# columnn n+1: r_" + std::to_string(pop) + "(Hz) for neurons in position:\n";
			file << "#\t\t (n-1) * " + std::to_string(info->Lx / Heatmap) + "  <  x  <  n * " + std::to_string(info->Lx / Heatmap) + "\n";
		}
		else {
			file << "# column 1 : t (secs.) \n";
			file << "# columnn n+1: r_" + std::to_string(pop) + "(Hz) for neurons in position:\n";
			file << "#\t\t (n-1)%" + std::to_string(Heatmap) + " * " + std::to_string(info->Lx / Heatmap) + "  <  x  <  n%" + std::to_string(Heatmap) + " * " + std::to_string(info->Lx / Heatmap) + "\n";
			file << "#\t\t (n-1)//" + std::to_string(Heatmap) + " * " + std::to_string(info->Lx / Heatmap) + "  <  y  <  n//" + std::to_string(Heatmap) + " * " + std::to_string(info->Lx / Heatmap) + "\n";
		}
		//legend header
		if (dim == 1) {
			file << "t\t";
			for (int x = 0;x < Heatmap;x++) {
				file <<"["<< double(x) / double(Heatmap)*info->Lx<<"]\t";
			}
		}
		else if (dim == 2) {
			file << "t\t";
			for (int x = 0;x < Heatmap;x++) {
				for (int y = 0;y < Heatmap;y++) {
					file << "[" << double(x) / double(Heatmap)*info->Lx <<","<< double(y) / double(Heatmap)*info->Lx << "]\t";
				}
			}
		}
		file << "\n#************************************\n";
		for (int k = 1;k < 2 + pow(Heatmap, dim);k++)
			file << "#" + std::to_string(k) << "  \t";
		file << "\n";
		file.close();
	}
}

void AdvancedRecorder::WriteDataHeader_Averages(){

    std::ofstream file;
    int P = this->neurons->GetTotalPopulations();
    int col = 1;

    file.open (this->GetDataFilename(), std::ofstream::out | std::ofstream::trunc);
    WriteHeader(&file);
    file << "#************************************\n";
    file << "#Columns: \n";
    file << "#" +std::to_string(col)+ " t (secs.) \n"; col++;
    for(int i = 0; i < P; i++){
        file << "#" +std::to_string(col)+ " V_" << i << " (mV)\n";col++;}

    for(int i = 0; i < P; i++){
        file << "#" +std::to_string(col)+ " r_" << i << " (Hz)\n"; col++;}

    for(int i = 0; i < P; i++){
        file << "#" +std::to_string(col)+ " mu_Ext_" << i << "/tau_m (dmV/sec) \n"; col++;}

    for(int i = 0; i < P; i++)
    {
        for(int j = 0; j < P; j++){
            file << "#" +std::to_string(col)+ " mu_" << i << "_" << j << "/tau_m (dmV/sec) \n"; col++;}
    }
    for(int i = 0; i < P; i++){
        file << "#" +std::to_string(col)+ " mu_" << i << "/tau_m (dmV/sec) \n"; col++;}

    for(int i = 0; i < P; i++){
        file << "#" +std::to_string(col)+ " Quenched fluctuations of mu_" << i << "/tau_m (dmV/sec) = sqrt(PopAver(TempAver(mu_i)^2) - PopAver(TempAver(mu_i))^2) \n"; col++;}

    for(int i = 0; i < P; i++){
        file << "#" +std::to_string(col)+ " Temporal fluctuations of mu_" << i << "/tau_m*sqrt(dt) (dmV/sqrt(sec)) = sqrt(dt)*sqrt(PopAver(TempAver(mu_i^2) - TempAver(mu_i)^2))\n"; col++;}

	file << "t\t";
	for (int i = 0; i < P; i++)
		file << "V_" << i<<"\t";
	for (int i = 0; i < P; i++)
		file << "r_" << i << "\t";
	for (int i = 0; i < P; i++)
		file << "mu_ext_" << i << "\t";
	for (int i = 0; i < P; i++) {
		for (int j=0;j<P;j++)
			file << "mu_" << i << "_"<< j << "\t";
	}
	for (int i = 0; i < P; i++)
		file << "mu_tot_" << i << "\t";
	for (int i = 0; i < P; i++)
		file << "sigma_quenched_" << i << "\t";
	for (int i = 0; i < P; i++)
		file << "sigma_t_" << i << "\t";
	file << "\n#************************************\n";

    for(int k = 1;k<col;k++)
        file << "#"+std::to_string(k) << "\t\t";
    file << "\n";
    file.close();

}

void AdvancedRecorder::WriteDataHeader_Rasterplot(){

    if(noRasterPlotNeurons.sum() == 0)
        return;

    std::ofstream write_raster_plot;
    write_raster_plot.open (GetRasterplotFilename(), std::ofstream::out | std::ofstream::trunc);
    WriteHeader(&write_raster_plot);
    write_raster_plot << "#1 t (secs.) \t #2 neuron_id \t #3 neuron_pop_id \t\n";
	write_raster_plot << "# Note that the recorded neurons are equidistant within the population. Therefore, neuron_id are not necessarily successive numbers\n";
	write_raster_plot << "Spike_t\tNeuron_id\tPop_id\n";
	write_raster_plot << "#************************************\n";
    write_raster_plot.close();
}

void AdvancedRecorder::WriteDataHeader_Potential(){

    if(notrackNeuronPotentials.sum() == 0)
        return;

    unsigned long             P = neurons->GetTotalPopulations();
    std::ofstream    stream;
    stream.open(GetPotentialFilename(), std::ofstream::out | std::ofstream::app);

    WriteHeader(&stream);
    stream << "#1 t (secs.)\t 2-"<<1+notrackNeuronPotentials.sum()<<" V_pop_id (mV) \n";

	stream << "t\t";
    for(unsigned long p = 0;p<P;p++){
        for(unsigned long i = 0;i<notrackNeuronPotentials[p];i++)
            stream << "V_" << p << "_" << i <<  "\t";
    }
	stream << "\n#************************************\n";

    stream.close();

}

void AdvancedRecorder::WriteDataHeader_Currents(){

    if(notrackNeuronPotentials.sum() == 0)
        return;

    unsigned long             P = neurons->GetTotalPopulations();
    std::ofstream    stream;
    stream.open(GetCurrentsFilename(), std::ofstream::out | std::ofstream::app);

    WriteHeader(&stream);
    stream << "#1 t (sec)\t 2-"<<1+ notrackNeuronPotentials.sum()<<" mu_pop_id / tau_m(dmV / sec)\n";

	stream << "t\t";
	for(unsigned long p = 0;p<P;p++){
        for(unsigned long i = 0;i<notrackNeuronPotentials[p];i++)
            stream << "mu_" << p << "_" << i << "\t";
    }
	stream << "\n#************************************\n";

    stream.close();

}

void AdvancedRecorder::WriteDataHeader_CurrentsContribution() {
	if (CurrentContributions.sum() == 0)
		return;
	unsigned long P = neurons->GetTotalPopulations();
	long index;
	long HeaderIndex;
	long N;
	std::ofstream stream;
	stream.open(GetCurrentCrontributionFilename(), std::ofstream::out | std::ofstream::app);

	WriteHeader(&stream);
	stream << "# 1 t (sec)\n";
	HeaderIndex = 2;
	for (unsigned long p_target = 0;p_target < P;p_target++) {
		N = CurrentContributions[p_target];
		if (N > 0) {
			for (unsigned long p_source = 0;p_source < P;p_source++) {
				stream <<"# "<< HeaderIndex <<"-"<< HeaderIndex +N -1 <<" Irecurrent_pop"<< p_target<<"<-pop"<< p_source<<"_neuronid / tau_m (mV/s)\n";
				HeaderIndex = HeaderIndex + N;
			}
			stream << "# "<<HeaderIndex << "-" << HeaderIndex + N -1 << " Ifeedforward_pop"<< p_target <<"_id / tau_m (mV/s)\n";
			HeaderIndex = HeaderIndex + N;
		}
	}
	stream << "# Note that the recorded neurons are equidistant within the population. Therefore, neuron_id are not necessarily successive numbers\n";

	stream << "t\t";
	for (unsigned long p_target = 0;p_target < P;p_target++) {
		for (unsigned long p_source = 0;p_source < P;p_source++) {
			for (long i = 0;i < CurrentContributions[p_target];i++) {
                //This is an integer type division, it does not need floor()
				index = i*neurons->GetNeuronsPop(p_target) / CurrentContributions[p_target];
				stream << "IRec" << p_target << "_" << p_source << "_" << index << "\t";
			}
		}
		for (long i = 0;i < CurrentContributions[p_target];i++) {
            //This is an integer type division, it does not need floor()
			index = i*neurons->GetNeuronsPop(p_target) / CurrentContributions[p_target];
			stream << "Iffd" << p_target << "_" << index << "\t";
		}
	}
	stream << "\n#************************************\n";
	stream.close();
}


/*void AdvancedRecorder::WriteDataHeader_Histogram(){

    if(!writeHistogram)
        return;

    std::ofstream     stream;
    stream.open(GetHistogramFilename(), std::ofstream::out | std::ofstream::app);

    WriteHeader(&stream);
    stream << "#1 neuron_id \t #2 current (dmV/sec) \t #3 firingrate (Hz)\t\n";

    stream.close();
}*/

void AdvancedRecorder::WriteDataHeader_Correlations(){

    if(noCorrNeurons.sum() == 0)
        return;

    std::ofstream     stream;
    stream.open(GetMeanCorrelationsFilename(), std::ofstream::out | std::ofstream::app);

    WriteHeader(&stream);
    stream << "#1 t (sec) \t #2 CC_EE \t #3 CC_EI \t #4 CC_IE \t #5 CC_II\t\n";

    stream.close();

}

void AdvancedRecorder::WriteDataHeader_HeteroSynapses(){

    if(noTrackHeteroSynapsePerTrackedNeuron.sum() == 0)
        return;

    unsigned long P = neurons->GetTotalPopulations();
    std::ofstream stream;
    stream.open(GetHeteroSynapseStateFilename(), std::ofstream::out | std::ofstream::app);

    WriteHeader(&stream);
    stream << "Profile -> {<dist to soma>, <hetero cooperativity>, <weight>, <last spike>} \n";
    stream << "\n#************************************\n";

    stream << "#1 t (secs.)\t 2-"<<1+noTrackHeteroSynapsePerTrackedNeuron.sum()<<" Profile_pop_id_neuron_id_synapse_id \n";

    stream << "t\t";

    HeteroNeuronPop* heteroNeuronPop;
    unsigned long synCount, synTrackCount, skipStride;
    for(unsigned long p = 0;p<P;p++){
        heteroNeuronPop = dynamic_cast<HeteroNeuronPop*>(this->neurons->GetPop(p));
        synTrackCount =  noTrackHeteroSynapsePerTrackedNeuron[p];
        if (heteroNeuronPop == nullptr || synTrackCount == 0) {
            continue;
        }
        for(unsigned long i = 0;i<notrackNeuronPotentials[p];i++) {
            synCount = heteroNeuronPop->getSynapseCount(i);
            skipStride = synCount / synTrackCount;
            skipStride = skipStride == 0 ? 1 : skipStride;
            for (unsigned long k = 0; k < noTrackHeteroSynapsePerTrackedNeuron[i]; ++k) {
                stream << "Profile_" << p << "_" << i << "_" << (k * skipStride) <<  "\t";
            }
        }
    }
    stream << "\n#************************************\n";

    stream.close();
}

void AdvancedRecorder::WriteDataHeader_HeteroSynapsesOverall(){

    if(noTrackHeteroSynapsePerTrackedNeuron.sum() == 0)
        return;

    unsigned long P = neurons->GetTotalPopulations();
    std::ofstream stream;
    stream.open(GetOverallHeteroSynapseStateFilename(), std::ofstream::out | std::ofstream::app);

    WriteHeader(&stream);
    stream << "Overall Profile -> {<average weight>, <total post spikes>, <totoal pre spikes>} \n";
    stream << "\n#************************************\n";

    stream << "#1 t (secs.)\t 2-"<<1+noTrackHeteroSynapsePerTrackedNeuron.sum()<<" Profile_pop_id_neuron_id \n";

    stream << "t\t";

    unsigned long synTrackCount;
    for(unsigned long p = 0;p<P;p++){
        synTrackCount =  noTrackHeteroSynapsePerTrackedNeuron[p];
        if (synTrackCount == 0) {
            continue;
        }
        for(unsigned long i = 0;i<notrackNeuronPotentials[p];i++) {
            for (unsigned long k = 0; k < noTrackHeteroSynapsePerTrackedNeuron[i]; ++k) {
                stream << "OverallProfile_" << p << "_" << i <<  "\t";
            }
        }

    }
    stream << "\n#************************************\n";

    stream.close();

}

//void AdvancedRecorder::InitializeRecorder(std::string filename){
void AdvancedRecorder::WriteDataHeader(){
    Record_SynapseStates(1);
    WriteDataHeader_Rasterplot();
    WriteDataHeader_Averages();
    WriteDataHeader_Potential();
    WriteDataHeader_Currents();
    //WriteDataHeader_Histogram();
    WriteDataHeader_Correlations();
	WriteDataHeader_Heatmap();
	WriteDataHeader_CurrentsContribution();
	WriteDataHeader_HeteroSynapses();
	WriteDataHeader_HeteroSynapsesOverall();
    reset_statistics();
}

void AdvancedRecorder::reset_statistics()
{
    int P = this->neurons->GetTotalPopulations();

    currentBin.potential         = 0.0;
    currentBin.spiker_ratio      = 0.0;
    currentBin.externalCurrent   = 0.0;
    currentBin.totalCurrentSquared_mean  = 0.0;

    for(int i = 0; i < P; i++)
    {
        std::fill(currentBin.totalCurrent_mean_N.at(i).begin(), currentBin.totalCurrent_mean_N.at(i).end(), 0);
        for(int j = 0; j < P; j++)
        {
            currentBin.synapticCurrents[i][j]    = 0.0;
            currentBin.no_recordedSynapses[i][j] = 0;
            currentBin.synapticState[i][j]       = 0.0;
        }
		if (Heatmap != 0) {
			for (int pixel = 0;pixel < pow(Heatmap, info->Dimensions);pixel++)
				currentBin.Heatmap[i][pixel] = 0.0;
		}
    }
}

void AdvancedRecorder::Record_Rasterplot(){

	if (noRasterPlotNeurons.sum() == 0 || info->time_step < raster_t_0)
        return;
    double           dt = info->dt;
    std::ofstream    write_raster_plot;
    std::vector<long> *spiker;
    // int				first_id;
	long			Nrec;//number of neurons to record
	long			Ntot;//number of neurons in population
    write_raster_plot.open (GetRasterplotFilename(), std::ofstream::out | std::ofstream::app);

    for(unsigned int pop = 0;pop<neurons->GetTotalPopulations();pop++){
		Nrec = noRasterPlotNeurons[pop];
		if (Nrec == 0)
			continue;
        spiker   = neurons->GetSpikers(pop);
		Ntot = neurons->GetNeuronsPop(pop);

        for(auto const &spiker_id : (*spiker) ){
            if(spiker_id == std::round(double(Ntot / Nrec * std::round(double(Nrec*spiker_id / Ntot)))))
                write_raster_plot << double(info->time_step)*dt <<"\t" << spiker_id << "\t" << pop << "\n";
        }
    }
    write_raster_plot.close();
}

void AdvancedRecorder::Record_Currents(std::vector<std::vector<double>> * synaptic_dV){

    if(notrackNeuronPotentials.sum() == 0)
        return;
    double           dt = info->dt;
    double           t = double(info->time_step)*dt;
    int              P = neurons->GetTotalPopulations();
    std::ofstream    file;

    file.open (GetCurrentsFilename(), std::ofstream::out | std::ofstream::app);

    SaveDoubleFile(&file,t,5);
    for(int p = 0;p<P;p++){
        for(unsigned long i = 0;i<notrackNeuronPotentials[p];i++){
            SaveDoubleFile(&file,synaptic_dV->at(p).at(i)/info->dt,5);
        }
    }
    file << "\n";
    file.close();
}

void AdvancedRecorder::Record_Potential(){

    if(notrackNeuronPotentials.sum() == 0)
        return;

    double           dt = info->dt;
    double           t = double(info->time_step)*dt;
    long             P = neurons->GetTotalPopulations();
    std::ofstream    file;
    file.open(GetPotentialFilename(), std::ofstream::out | std::ofstream::app);

    SaveDoubleFile(&file,t,5);
    for(int p = 0;p<P;p++){
        for(unsigned long i = 0;i<notrackNeuronPotentials[p];i++)
            SaveDoubleFile(&file,neurons->GetPotential(p,i),5);
    }
    file << "\n";

    file.close();

}

/*void AdvancedRecorder::Record_Histogram(std::vector<std::vector<double>> * synaptic_dV){

    if(!writeHistogram)
        return;

    double n_aver     = double(averaging_steps);
    int pops          = neurons->GetTotalPopulations();
    double value;
    std::vector<long> *spiker;

    std::ofstream file;
    file.open(GetHistogramFilename(), std::ofstream::out | std::ofstream::app);

    for(int pop = 0; pop < pops; pop++){
            spiker = neurons->GetSpikers(pop);
            for(auto const &spiker_id : (*spiker) ){
                SaveDoubleFile(&file,(double)(info->time_step)*(info->dt),5);
                SaveDoubleFile(&file,spiker_id,0);
                //SaveDoubleFile(&file,pop,0);
                file << "\n";
            }

    }

    if((info->time_step)%this->averaging_steps == 0)
    {
        for(int pop = 0; pop < pops; pop++){
            for(int i = 0;i<neurons->GetNeuronsPop(pop);i++){
                SaveDoubleFile(&file,i+1,0);
                value = currentBin.totalCurrent_mean_N.at(pop).at(i)/n_aver;
                SaveDoubleFile(&file,value,5);
                file << "\n";
            }
        }
        file << "\n";
        file.close();

    }

}*/

void AdvancedRecorder::Record_Correlations(std::vector<std::vector<double>> * synaptic_dV){

    if(noCorrNeurons.sum() == 0)
        return;

    double           dt = double(info->dt);
    double           t = double(info->time_step)*dt;
    double           fac;
    long             n_aver = averaging_steps;
    long             ts = info->time_step;
    // long             start, end;
    long             itj;
    long             P = neurons->GetTotalPopulations();

    std::vector<std::vector<double>> mean(noCorrNeurons[0], std::vector<double>(P));
    std::vector<std::vector<double>> sigma(noCorrNeurons[0], std::vector<double>(P));
    std::vector<std::vector<std::vector<std::vector<double>>>> crosscorr(noCorrNeurons[0], std::vector<std::vector<std::vector<double>>> (noCorrNeurons[0], std::vector<std::vector<double>> (P, std::vector<double>(P))));
    std::vector<std::vector<double>> crosscorr_final(P, std::vector<double> (P));
    std::ofstream    file_meanCorr,file_pairCorr;

    //Fill current memory
    for(int m = 0;m<P;m++){
        for(long i = 0;i<noCorrNeurons[m];i++){
            savecurrents[i][ts%n_aver][m] = synaptic_dV->at(m).at(i)/double(info->dt);
        }
    }

    if((info->time_step)%this->averaging_steps != 0)
        return;

    //Compute current mean and standard deviation
    for(int m = 0;m<P;m++){
        for(long i=0;i<noCorrNeurons[m];i++){
            for(int k=0;k<n_aver;k++)
                mean[i][m] += savecurrents[i][k][m]/n_aver;

            for(int k=0;k<n_aver;k++)
                sigma[i][m] += pow(savecurrents[i][k][m]-mean[i][m],2)/n_aver;
        }
    }

    //Compute pairwise and mean crosscorrelations
    for(int m = 0;m<P;m++){
        for(int n=0;n<P;n++){
            for(long i = 0;i<noCorrNeurons[m];i++){
                for(long j = 0;j<noCorrNeurons[n];j++){
                    for(int k=0;k<n_aver;k++){
                        crosscorr[i][j][m][n] += (savecurrents[i][k][m]*savecurrents[j][k][n]-mean[i][m]*mean[j][n])/(pow(sigma[i][m]*sigma[j][n],0.5)*n_aver);
                    }
                }

                if(n == m){ //same populations m,n
                    itj = i+1;
                    fac = 2/((double)noCorrNeurons[n]*((double)noCorrNeurons[n]-1));
                }
                else{       //different populations m,n
                    itj = 0;
                    fac = 1/((double)noCorrNeurons[n]*(double)noCorrNeurons[m]);
                }
                for(long j=itj;j<noCorrNeurons[n];j++){
                    crosscorr_final[m][n] += fac*crosscorr[i][j][m][n];
                }
            }
        }
    }


    //save mean crosscorrelations
    file_meanCorr.open(GetMeanCorrelationsFilename(), std::ofstream::out | std::ofstream::app);
    SaveDoubleFile(&file_meanCorr,t,5);
    file_meanCorr << "\t";
    for(int m = 0;m<P;m++){
        for(int n=0;n<P;n++){
            SaveDoubleFile(&file_meanCorr,crosscorr_final[m][n],5);
            file_meanCorr << "\t";
        }
    }
    file_meanCorr << "\n";
    file_meanCorr.close();


    //save pair crosscorrelations
    file_pairCorr.open(GetPairCorrelationsFilename(), std::ofstream::out | std::ofstream::app);

    WriteHeader(&file_pairCorr);
    file_pairCorr << "#Pairwise correlations averages over t = ["<< t- averaging_steps*dt <<", " << t << "] seconds \n";
    file_pairCorr << "#Tracking correlations between first \n";
    for(int i = 0;i<P;i++)
        file_pairCorr << "# " << noCorrNeurons[i] << " neurons of population " << i << " ,\n";
    file_pairCorr << "# respectively. \n";
    //file_pairCorr << " neurons of each population\n";

    for (int m = 0; m<P; m++){
        for (int i = 0; i < crosscorr.size(); i++){
            for (int n = 0; n<P; n++){
                for (int j = 0; j < crosscorr[i].size(); j++){
                    SaveDoubleFile(&file_pairCorr,crosscorr[i][j][m][n],5);
                    file_pairCorr << "\t";
                }
            }
            file_pairCorr << "\n";
        }
    }
    file_pairCorr.close();

}

void AdvancedRecorder::Record_CurrentContributions(std::vector<std::vector<double>> * synaptic_dV) {

	if (CurrentContributions.sum() == 0 || info->time_step < current_t_0)
		return;

	double dt = info->dt;
	double t = double(info->time_step)*dt;
	unsigned P = neurons->GetTotalPopulations();
	long index;
	long IndexMemory = 0;
	std::ofstream file;


	for (unsigned int p_target = 0;p_target < P;p_target++) {
		for (unsigned int p_source = 0;p_source < P;p_source++) {
			for (long i = 0;i < CurrentContributions[p_target];i++) {
				index = i*neurons->GetNeuronsPop(p_target) / CurrentContributions[p_target];
				CurrentContrBin[i + IndexMemory] += synapses->GetRecurrentInput(p_target, p_source, index);
			}
			IndexMemory += CurrentContributions[p_target];
		}
		for (long i = 0;i < CurrentContributions[p_target];i++) {
			index = i*neurons->GetNeuronsPop(p_target) / CurrentContributions[p_target];
			CurrentContrBin[i + IndexMemory] += stimulus->GetSignalArray(p_target, index);
		}
		IndexMemory += CurrentContributions[p_target];
	}

	file.open(GetCurrentCrontributionFilename(), std::ofstream::out | std::ofstream::app);

	if ((info->time_step) % this->averaging_steps == 0) {
		SaveDoubleFile(&file, t, 5);
		for (unsigned int i = 0; i < CurrentContrBin.size(); i++) {
			SaveDoubleFile(&file, CurrentContrBin[i], 5);
			CurrentContrBin[i] = 0;
		}
		file << "\n";
		file.close();
	}
}

void AdvancedRecorder::Record_SynapseStates(int header){

    std::ofstream file;
    double dt = info->dt;
    double value;
    unsigned    P = neurons->GetTotalPopulations();

    if(!trackSynapses)
        return;

    if(header == 1){
        file.open (this->GetSynapseStateFilename(), std::ofstream::out | std::ofstream::trunc);
        WriteHeader(&file);
        file << "#************************************\n";
        file << "#Columns: \n";
        file << "#1 t (secs.) \n";
		file << synapses->GetDataHeader(2);
		file << "t\t"<<synapses->GetUnhashedDataHeader() << "\n";
        file << "# Attention, Synaptic state data is spike-induced : at each time step, only synapses from which the presynaptic neuron has spiked are measured. Data only tested for CurrentSynapse and MongilloSynapse\n";
        file << "#************************************\n";
        file.close();
        return;
    }

    if((info->time_step+1)%this->averaging_steps == 0)
    {

        file.open(GetSynapseStateFilename(),std::ofstream::out | std::ofstream::app);
        SaveDoubleFile(&file,double(info->time_step)*dt,4);

        //Record synaptic strengths
        for(unsigned post_population = 0; post_population < P; post_population++)
        {
            for(unsigned pre_population = 0; pre_population < P; pre_population++)
            {
                if(currentBin.no_recordedSynapses[post_population][pre_population] > 0)
                {
                    for(unsigned data_length = 0; data_length < currentBin.synapticState[post_population][pre_population].size(); data_length++)
                    {
                        //std::cout << "synapse " << std::to_string(pre_population) << " to " << std::to_string(post_population) << ": ";
                        //std::cout << std::to_string(currentBin.synapticState[post_population][pre_population][data_length]) << " ";
                        //std::cout << std::to_string(double(currentBin.no_recordedSynapses[post_population][pre_population])) << " ";
                        value = currentBin.synapticState[post_population][pre_population][data_length] /
                        double(currentBin.no_recordedSynapses[post_population][pre_population]);
                        //std::cout << std::to_string(value) << "\n";
                        SaveDoubleFile(&file,value,6);
                    }
                }
                else
                {
                    for(unsigned data_length = 0; data_length < currentBin.synapticState[post_population][pre_population].size(); data_length++)
                    {
                        value = NAN;
                        SaveDoubleFile(&file,value,6);
                        //file << "\t";
                    }
                }
            }
        }
        file << "\n";
        file.close();
    }
}

void AdvancedRecorder::Record_Averages(){

    std::ofstream file;
    double value;
    unsigned int    pops       = neurons->GetTotalPopulations();
    double dt         = info->dt;
    double deltaT_rec = double(this->averaging_steps)*dt;
    double n_aver     = double(averaging_steps);
    double n;

    std::valarray<double> means(pops);
    std::valarray<double> popAver_SquaredMeanTime(pops);

    means        = 0.0; popAver_SquaredMeanTime = 0.0;

    for(unsigned int pop = 0; pop < pops; pop++){
        n = neurons->GetNeuronsPop(pop);
        for(unsigned long j = 0;j<neurons->GetNeuronsPop(pop);j++){
            means[pop]                   += currentBin.totalCurrent_mean_N.at(pop).at(j)/n/n_aver;
            popAver_SquaredMeanTime[pop] += pow(currentBin.totalCurrent_mean_N.at(pop).at(j)/n_aver,2)/n; // population average over square of temporal average [(I_alpha^i)^2]
        }
    }


    if((info->time_step)%this->averaging_steps == 0)
    {
        file.open(GetDataFilename(), std::ofstream::out | std::ofstream::app);
        SaveDoubleFile(&file,double(info->time_step)*dt,4);

        //Record average potential
        for(unsigned int i = 0; i < pops; i++)
        {
            value      = currentBin.potential[i]/n_aver;
            SaveDoubleFile(&file,value,3);
        }
        //Record firing rate
        for(unsigned int i = 0; i < pops; i++)
        {
            value = currentBin.spiker_ratio[i]/deltaT_rec;
            SaveDoubleFile(&file,value,3);
        }
        //Record external input 'current' (in mV/sec)
        for(unsigned int i = 0; i < pops; i++)
        {
            value   = currentBin.externalCurrent[i]/n_aver;
            SaveDoubleFile(&file,value,3);
        }
        //Record population averaged synaptic input currents mu_ij (in mV/sec)
        for(unsigned int i = 0; i < pops; i++)
        {
            for(unsigned int j = 0; j < pops; j++)
            {
                value   = currentBin.synapticCurrents[i][j]/n_aver;
                SaveDoubleFile(&file, value,3);
            }
        }
        //Record population averaged total synaptic input currents mu_i (in mV/sec)
        //for(int i = 0; i < pops; i++)
        //{
        //    value   = currentBin.totalCurrent_mean[i]/n_aver;
        //    SaveDoubleFile(&file,value,3);
        //}

        //Record check of mean total input currents
        for(unsigned int i = 0; i < pops; i++)
        {
            value = means[i];
            SaveDoubleFile(&file,value,3);
        }

        //Quenched fluctuations (due to varying input over population)
        for(unsigned int i = 0; i < pops; i++)
        {
            value = sqrt(popAver_SquaredMeanTime[i]-pow(means[i],2));
            SaveDoubleFile(&file,value,3);
        }

        //Temporal fluctuations
        for(unsigned int i = 0; i < pops; i++)
        {
            value = sqrt(currentBin.totalCurrentSquared_mean[i]/n_aver - popAver_SquaredMeanTime[i])*sqrt(info->dt);
            SaveDoubleFile(&file,value,3);
        }

        file << "\n";
        file.close();

        reset_statistics();
    }
}
//
void AdvancedRecorder::Record_Heatmap() {

	if (Heatmap == 0)
		return;

	std::ofstream file;
	double value;
	unsigned int    pops = neurons->GetTotalPopulations();
	double dt = info->dt;
	double deltaT_rec = double(this->averaging_steps)*dt;
	// double n_aver = double(averaging_steps);
	// double n;

	if ((info->time_step) % this->averaging_steps == 0){
		for (unsigned int pop = 0; pop < pops;pop++) {
			file.open(this->GetHeatmapFilename() + std::to_string(pop)+".dat", std::ofstream::out | std::ofstream::app);
			SaveDoubleFile(&file, double(info->time_step)*dt, 4);
			for (int pixel = 0;pixel < pow(Heatmap, info->Dimensions); pixel++) {
				value = currentBin.Heatmap[pop][pixel] / deltaT_rec;
				SaveDoubleFile(&file, value, 3);
			}
			file << "\n";
			file.close();
		}
	}
}

void AdvancedRecorder::Record_HeteroSynapses() {

    if(noTrackHeteroSynapsePerTrackedNeuron.sum() == 0) {
        return;
    }

    double           dt = info->dt;
    double           t = double(info->time_step)*dt;
    long             P = neurons->GetTotalPopulations();
    std::ofstream    file;
    file.open(GetHeteroSynapseStateFilename(), std::ofstream::out | std::ofstream::app);

    SaveDoubleFile(&file,t,5);

    HeteroNeuronPop* heteroNeuronPop;
    unsigned long synCount, synTrackCount, skipStride;
    for(long p = 0;p < P; p++){
        heteroNeuronPop = dynamic_cast<HeteroNeuronPop*>(this->neurons->GetPop(p));
        synTrackCount =  noTrackHeteroSynapsePerTrackedNeuron[p];
        if (heteroNeuronPop == nullptr || synTrackCount == 0) {
            continue;
        }
        for(unsigned long i = 0;i<notrackNeuronPotentials[p];i++) {
            synCount = heteroNeuronPop->getSynapseCount(i);
            skipStride = synCount / synTrackCount;
            skipStride = skipStride == 0 ? 1 : skipStride;
            for (unsigned long k = 0; k < noTrackHeteroSynapsePerTrackedNeuron[i]; ++k) {
                SaveDoubleFile(&file, heteroNeuronPop->getIndividualSynapticProfile(i, k*skipStride)[2], 5);
//                SaveTupleOfDoublesFile(&file, heteroNeuronPop->getIndividualSynapticProfile(i, k*skipStride), 5);
            }
        }
    }
    file << "\n";
    file.close();
}

void AdvancedRecorder::Record_HeteroSynapsesOverall() {
    if(noTrackHeteroSynapsePerTrackedNeuron.sum() == 0) {
        return;
    }

    double           dt = info->dt;
    double           t = double(info->time_step)*dt;
    long             P = neurons->GetTotalPopulations();
    std::ofstream    file;
    file.open(GetOverallHeteroSynapseStateFilename(), std::ofstream::out | std::ofstream::app);

    SaveDoubleFile(&file,t,5);

    HeteroNeuronPop* heteroNeuronPop;
    for(long p = 0;p < P; p++){
        heteroNeuronPop = dynamic_cast<HeteroNeuronPop*>(this->neurons->GetPop(p));
        if (heteroNeuronPop == nullptr) {
            continue;
        }
        for(unsigned long i = 0;i<notrackNeuronPotentials[p];i++) {
            SaveDoubleFile(&file, heteroNeuronPop->getOverallSynapticProfile(i)[0], 5);
//            SaveTupleOfDoublesFile(&file, heteroNeuronPop->getOverallSynapticProfile(i), 5);
        }
    }
    file << "\n";
    file.close();

}


void AdvancedRecorder::Record(std::vector<std::vector<double>> * synaptic_dV)
{
	std::vector<long> *spikers;
	unsigned int P = this->neurons->GetTotalPopulations();
	double n;
	int Xbin, Ybin;

	for (unsigned int p {0}; p < P; p++) {
		for (unsigned long i {0}; i < neurons->GetNeuronsPop(p); i++) {
			n = static_cast<double>(this->neurons->GetNeuronsPop(p));
			currentBin.potential[p] += this->neurons->GetPotential(p, i) / n;
			currentBin.externalCurrent[p] += this->stimulus->GetSignalArray(p, i) / n / info->dt;
			currentBin.totalCurrentSquared_mean[p] += pow(synaptic_dV->at(p).at(i) / info->dt, 2.0) / n;
			//record per neuron
			currentBin.totalCurrent_mean_N.at(p).at(i) += synaptic_dV->at(p).at(i) / info->dt;
		}
	}

	for (unsigned int prePop = 0;prePop < P; prePop++) {
		spikers = this->neurons->GetSpikers(prePop);
		currentBin.spiker_ratio[prePop] += ((double)spikers->size()) / ((double)this->neurons->GetNeuronsPop(prePop));
		if (Heatmap != 0) {
			if (info->Dimensions == 2) {
				for (unsigned int spike = 0;spike < spikers->size();spike++) {
					Xbin = static_cast<int>(neurons->GetX_Pos(prePop, spikers->at(spike)) * Heatmap / info->Lx);
					Ybin = static_cast<int>(neurons->GetY_Pos(prePop, spikers->at(spike)) * Heatmap / info->Ly);
					currentBin.Heatmap[prePop][Heatmap*Ybin + Xbin] += 1/ (double)Densimap[prePop][Heatmap*Ybin + Xbin];
				}
			}
			else {
				for (unsigned int spike = 0;spike < spikers->size();spike++) {
					Xbin = static_cast<int>(neurons->GetX_Pos(prePop, spikers->at(spike)) * Heatmap / info->Lx);
					currentBin.Heatmap[prePop][Xbin] += 1 / (double) Densimap[prePop][Xbin];
				}
			}
		}
		for (unsigned int postPop = 0; postPop < P; postPop++)
			currentBin.synapticCurrents[postPop][prePop] += this->synapses->GetCumulatedDV(postPop, prePop) / ((double)this->neurons->GetNeuronsPop(postPop)) / info->dt;

		if (!trackSynapses)
			continue;
		for (unsigned int i = 0; i < spikers->size(); i++)
		{
			for (unsigned int post_population = 0; post_population < P; post_population++) {
                //std::cout << "Synapse " << std::to_string(prePop) << " to " << std::to_string(post_population) << " at time " << std::to_string(this->info->time_step) << "\n";
				currentBin.synapticState[post_population][prePop] += this->synapses->GetSynapticState(post_population, prePop, (*spikers)[i]);  //arrays are added
				currentBin.no_recordedSynapses[post_population][prePop] += this->synapses->GetNumberOfPostsynapticTargets(post_population, prePop, (*spikers)[i]);
                for(unsigned int i_data = 0; i_data<currentBin.synapticState[post_population][prePop].size(); i_data++){
                //std::cout << std::to_string(currentBin.synapticState[post_population][prePop][i_data]) << " ";
                //std::cout << std::to_string(currentBin.synapticState[post_population][prePop][i_data]/currentBin.no_recordedSynapses[post_population][prePop]) << "\n";
                }

            }
		}
	}
	Record_SynapseStates();
	Record_Heatmap();
	Record_Rasterplot();
	Record_Currents(synaptic_dV);
	//Record_Histogram(synaptic_dV);
	Record_Correlations(synaptic_dV);
	Record_Averages();
	Record_Potential();
	Record_CurrentContributions(synaptic_dV);
	Record_HeteroSynapses();
	Record_HeteroSynapsesOverall();
}

void AdvancedRecorder::writeFinalDataFile(double comp_time)
{
    std::chrono::time_point<std::chrono::system_clock> t;
    t                    = std::chrono::system_clock::now();
    std::time_t end_time {std::chrono::system_clock::to_time_t(t)};

    std::ofstream stream(GetParametersFilename(),std::ofstream::out | std::ofstream::app);
    stream <<  "#*****************************************************************\n";
    stream <<  "#Comp. finalized: " << std::ctime(&end_time);
    stream <<  "#Comp. time: " << comp_time << " secs.\n";
    stream.close();

    std::cout << "Results saved in " << directoryPath << "\n";

}
