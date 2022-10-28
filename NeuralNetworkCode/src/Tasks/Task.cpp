#include "Task.h"
void Task::LoadParameters(std::vector<std::string>* input) {

    std::string              name, token;
    std::vector<std::string> values;

    for (std::vector<std::string>::iterator it = (*input).begin(); it != (*input).end(); ++it) {

        SplitString(&(*it), &name, &values);

        /*if(name.find("Ni") != std::string::npos){
            totalPopulations = (int)values.size();
            //spiker              = new std::vector<int>[totalPopulations];
            //neuronsInPopulation = new int[totalPopulations];
            //for(int i = 0;i<totalPopulations;i++)
            //    neuronsInPopulation[i] = std::stoi(values.at(i));
        }*/
        if (name.find("tauM") != std::string::npos) {
            this->tau_m = (std::stod(values.at(0)));
        }
        else if (name.find("vReset") != std::string::npos) {
            this->v_reset = (std::stod(values.at(0)));
        }
        else if (name.find("vThresh") != std::string::npos) {
            this->v_thresh = (std::stod(values.at(0)));
        }
        else if (name.find("refractoryTime") != std::string::npos) {
            this->refractorySteps = std::round((std::stod(values.at(0))) / info->dt);
        }
        //else if(name.find("r_target") != std::string::npos){
         //   this->r_target = (std::stoi(values.at(0)));
        //}
        else if (name.find("seedInitialPotentials") != std::string::npos) {
            this->seed_InitialPotentials = (std::stoi(values.at(0)));
        }
        else if (name.find("seedInitialPrevSpike") != std::string::npos) {
            this->seed_InitialPrevSpike = (std::stoi(values.at(0)));
        }
    }

    for (std::vector<std::string>::iterator it = (*input).begin(); it != (*input).end(); ++it) {

        SplitString(&(*it), &name, &values);

        if (name.find("noNeurons") != std::string::npos) {
            SetNeurons((long)(std::stod(values.at(0))));
            //spiker              = new std::vector<int>[totalPopulations];
            //neuronsInPopulation = new int[totalPopulations];
            //for(int i = 0;i<totalPopulations;i++)
            //    neuronsInPopulation[i] = std::stoi(values.at(i));
        }
    }
