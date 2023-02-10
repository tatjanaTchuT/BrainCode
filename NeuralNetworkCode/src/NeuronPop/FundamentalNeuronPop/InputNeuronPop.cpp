#include "./InputNeuronPop.hpp"
#include "InputNeuronPop.hpp"

InputNeuronPop::InputNeuronPop(GlobalSimInfo *info, int id) : NeuronPop (info, id)
{
    inputFileAddress = info->pathTo_inputFile + "Input_File_"+std::to_string(id)+".txt";
    inputStreamFile.open(inputFileAddress, std::ifstream::in);
    //Then just open and read, with bools for raw spikers or instruction-baseds
}

void InputNeuronPop::LoadParameters(std::vector<std::string> *input)
{
    NeuronPop::LoadParameters(input);//For now this is commented out to avoid confusion, and prevent from recording output in InputNeuronPop

    std::string              name,token;
    std::vector<std::string> values;

    for(std::vector<std::string>::iterator it = (*input).begin(); it != (*input).end(); ++it) {

        SplitString(&(*it),&name,&values);

        if(name.find("inputFile") != std::string::npos){
            if(values.at(0).find("spiker") != std::string::npos){
                spikerListFiringBasedBool=true;
            } else if (values.at(0).find("instruction") != std::string::npos){
                instructionBasedBool=true;
                inputInstructions.resize(noNeurons);
                activeInstructions.resize(noNeurons);
                ReadInstructionsFromFile();
            } else {
                throw;
            }
        }   
    }
}

void InputNeuronPop::SaveParameters(std::ofstream *stream)
{

    std::string id = "neurons_" + std::to_string(GetId());

    NeuronPop::SaveParameters(stream);

    *stream <<  id + "_inputFile                   ";
    if (spikerListFiringBasedBool){
        *stream<<"spiker";
    } else if (instructionBasedBool){
        *stream<<"instruction";
    } else {
        *stream<<"None (sth went wrong)";
    }
    *stream <<  "#Write 'instruction' for instruction-based, and 'spiker' for spiker-based \n";
}

void InputNeuronPop::advect(std::vector<double> *synaptic_dV)
{
    ClearSpiker();
    if (spikerListFiringBasedBool){
        ReadSpikersFromFile();
    } else if(instructionBasedBool){
        GenerateSpikersFromInstructions();
    } else {
        throw;
    }
}

void InputNeuronPop::ReadInstructionsFromFile()
{
    /*Instruction files for a population should be written in the following format:
    > neuronid starttime1 endtime1 fire_every_n_steps
    And ALWAYS time ordered with correct intervals
    */
   char*    entry = new char[2048];
    while(inputStreamFile.getline(entry, 256)){
        if(entry[0] == '#'){
            delete[] entry;
            entry = new char[2048];
            continue;
        } else if (entry[0] == '>'){
            FileEntry s_entry {std::move(stringToFileEntry(std::move(static_cast<std::string>(entry))))};
            inputInstructions.at(std::stoi(s_entry.values.at(0))).emplace_back(Instruction(std::stoi(s_entry.values.at(0)),std::lround(std::stod(s_entry.values.at(1))/info->dt), std::lround(std::stod(s_entry.values.at(2))/info->dt), std::stoi(s_entry.values.at(3))));
            delete[] entry;
            entry = new char[2048];
        } else {
            std::cout<<"Reading error. The unexpected read input was: "<< entry <<"\n";
            throw;
        }
    }
    for (std::vector<Instruction>& neuronInstructions: inputInstructions){
        neuronInstructions.back().last=true;
    }
}

void InputNeuronPop::GenerateSpikersFromInstructions()
{
    // //loop option 1 (inefficient but less exception prone)
    // for (int neuronId = 0; neuronId<static_cast<int>(inputInstructions.size()); neuronId++){
    //     for (Instruction& instruction : inputInstructions.at(neuronId)){
    //         if ((!instruction.completed) && (instruction.startTimeStep>info->time_step)){ //Here not >= to avoid a spike of frequency when changing instructions
    //             if (((info->time_step-instruction.startTimeStep)%instruction.fireEveryNSteps)==0){
    //                 spiker.push_back(neuronId);
    //                 if (instruction.endTimeStep>=info->time_step){
    //                 instruction.completed=true;
    //             }
    //             break;
    //             } 
    //         }
    //     }
    // }
    //loop option 2 (efficient but probably worse overall)
    for (int neuronId = 0; neuronId<static_cast<int>(activeInstructions.size()); neuronId++){
        Instruction& instruction = inputInstructions.at(neuronId).at(activeInstructions.at(neuronId));
        if (((info->time_step-instruction.startTimeStep)%instruction.fireEveryNSteps)==0){
            if (instruction.completed){continue;}
            spiker.push_back(neuronId);
            if (instruction.endTimeStep<=info->time_step){
                if (!(instruction.last)){
                    activeInstructions.at(neuronId)++;
                }
                instruction.completed=true;
            }
        }
    }
}

void InputNeuronPop::ReadSpikersFromFile()
{
    /*Spikers files for a population should be written in the following format:
    > time neuron1, neuron2, neuron 4.... (neurons that spike in time_step)
    NEVER PUT COMMENTS OR EXTRA_LINES IN THIS TYPE OF FILE!
    And be careful with your dt, as this will alter behaviour
    */
   char* entry = new char[2048];
    inputStreamFile.getline(entry, 256);
        if(entry[0] == '>'){
            FileEntry s_entry {std::move(stringToFileEntry(std::move(static_cast<std::string>(entry))))};
            long timeStep{std::lround(std::stod(s_entry.values.at(0))/info->dt)};
            if (timeStep != info->time_step){
                std::cout<<"There was a reading alignment error";
                throw;
            } else {
                for (int i = 1; i<static_cast<int>(s_entry.values.size()); i++){
                    spiker.push_back(std::stoi(s_entry.values.at(i)));
                }
            }
        } else {
            std::cout<<"Reading error. The unexpected read input was: "<< entry <<"\n";
            throw;
            }
}

Instruction::Instruction(int neuronId, long startTimeStep, long endTimeStep, int fireEveryNSteps): neuronId{neuronId}, startTimeStep{startTimeStep}, endTimeStep{endTimeStep}, fireEveryNSteps{fireEveryNSteps}
{
}
