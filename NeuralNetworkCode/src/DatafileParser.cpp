#include "DatafileParser.hpp"

DatafileParser::DatafileParser(AdvancedRecorder& recorder)
{
    parsingNOutputBool=recorder.streamingNOutputBool;//We keep it to run parse()
    directoryPath=recorder.directoryPath;
    title=recorder.title;
    neuronPopIds=recorder.streamingNeuronPops;
    if(parsingNOutputBool){
        for (int neuronPop : recorder.streamingNeuronPops){
            metaDataForFiles.emplace_back(DataOnFile(recorder.neurons->GetNeuronsPop(neuronPop),recorder.info->dt,static_cast<int>(recorder.info->simulationTime/recorder.info->dt), neuronPop));
            fileNamesToParse.emplace_back(recorder.GetNeuronOutputFilename(neuronPop));
        } for (std::string fileName : fileNamesToParse){
            filesToParse.emplace_back(std::ifstream(fileName));
        }
    }
    //Here we have to access the recorder to obtain the filepaths/filenames and paths in general (and the metadata)
    //Then we have to convert the filepaths into the proper ifstreams
    //After this constructor it is no longer necessary to keep the NN object in scope
}

std::string DatafileParser::indexToParsedOutputStreamFilePath(int vecIndex)
{
    return directoryPath + title + "_NeuronPop_" + std::to_string(neuronPopIds[vecIndex]) + "_ParsedOutput.dat";
}
std::vector<FileEntry> DatafileParser::parseFileToEntries(std::ifstream& fileStream)
{    
    //Here I have to parse each line (ignoring the first one with the column titles) and create FileEntries for each line
    char*    entry = new char[_MAX_CHARACTERS_PER_LINE];
    std::vector<FileEntry> parsedFileEntries;
    while (fileStream.getline(entry,_MAX_CHARACTERS_PER_LINE)){
        if(entry[0] == '#'){
            delete[] entry;
            continue;
        } else if (entry[0] == 't') {
            delete[] entry;
            continue;
        } else {
            parsedFileEntries.emplace_back(stringToFileEntry(std::move(static_cast<std::string>(entry))));
        }

        delete[] entry;
    }

    return parsedFileEntries;
}

std::vector<int> DatafileParser::entryToNeuronIndexes(FileEntry fileEntry)
{
    int neuronIndex{};
    std::vector<int> spikedNeurons;
    for (std::string str_point : fileEntry.values){
        if (str_point.find("1") != std::string::npos){
            spikedNeurons.push_back(neuronIndex);
        } else if (str_point.find("0") != std::string::npos){

        } else {
            std::cout<<"Parsing error oversight. The unexpected parsed input was: "<< str_point<<"\n";
            throw;
        }
        neuronIndex++;
    }
    //Here we check the 1s and output the neuron indexes/numbers that spiked on a given timestep
    return spikedNeurons;
}

std::vector<std::vector<double>> DatafileParser::parseSpikesToSpikeTimes(std::ifstream& fileStream, DataOnFile metadata)
{
    //Wrapper 1, assume the file does exist
    std::vector<std::vector<double>> spikeTimesVector (metadata.noNeurons); //vector of vectors of size equal to the number of neurons
    std::vector<FileEntry> parsedEntries{parseFileToEntries(fileStream)};
    double currentTimestep;
    std::vector<int> spikedNeurons;
    for (FileEntry& parsedEntry : parsedEntries){
        currentTimestep = std::stod(parsedEntry.name);
        for (int neuron : entryToNeuronIndexes(parsedEntry)){
            spikeTimesVector.at(neuron).push_back(currentTimestep);
        }
    }

    //Here we can use the FileEntry struct functions to parse a line, then iterate over values(two different loops). Name will be the timestep, luckily enough. 
    //If 1 is found, timestep added to the neuron's vector.
    //It will be necessary to properly use the indexes to achieve this (obtained in the  function above)
    //Use this return to call the write function
    return spikeTimesVector;
}



void DatafileParser::writeSpikeTimesFile(std::vector<std::vector<double>> parsedData, std::string wfilePath)
{
    //Wrapper 2
    //Here I will have to first, create the filepaths, then the ofstreams in a vector with same index. Then basically iterate over the 
}

void DatafileParser::parse()
{
    //Parent wrapper of wrappers 1 and 2
    if(parsingNOutputBool){

        int index{};
        for (std::ifstream& fileStream : filesToParse){
            struct stat buffer;
            if(stat(fileNamesToParse.at(index).c_str(),&buffer) != 0){
                std::cout << "*************************\n";
                std::cout <<fileNamesToParse.at(index)<<" Input file does not exist\n";
                std::cout << "*************************\n";
                throw;
            }

    }
    }
}

DataOnFile::DataOnFile(unsigned long noNeurons, double dt, int totalTimesteps,int neuronPopId): noNeurons{noNeurons}, dt{dt}, totalTimesteps{totalTimesteps}, neuronPopId{neuronPopId}
{
}
