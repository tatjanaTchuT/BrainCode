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

std::vector<FileEntry> DatafileParser::parseFileToEntries(std::ifstream& fileStream)
{    
    //Here I have to parse each line (ignoring the first one with the column titles) and create FileEntries for each line
    char*    entry = new char[_MAX_CHARACTERS_PER_LINE];
    std::vector<FileEntry> parsedFileEntries;
    while (fileStream.getline(entry,_MAX_CHARACTERS_PER_LINE)){
        if(entry[0] == '#'){
            delete[] entry;
            entry = new char[_MAX_CHARACTERS_PER_LINE];
            continue;
        } else if (entry[0] == 't') {
            delete[] entry;
            entry = new char[_MAX_CHARACTERS_PER_LINE];
            continue;
        } else {
            parsedFileEntries.emplace_back(stringToFileEntry(std::move(static_cast<std::string>(entry))));
        }
        delete[] entry;
        entry = new char[_MAX_CHARACTERS_PER_LINE];
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
    if (parsedEntries[0].values.size() > static_cast<size_t>(metadata.noNeurons - 1)) {
        std::cout << "Error: the file contains more neurons than it should";
        throw;
    }
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



void DatafileParser::writeSpikeTimesFile(std::vector<std::vector<double>> parsedData, std::string wfilePath, DataOnFile metadata)
{
    //Wrapper 2
    std::ofstream stream(wfilePath, std::ofstream::out | std::ofstream::trunc);
    int neuronIndex{};
    stream<<"M="<<std::to_string(metadata.noNeurons)<<','<<std::to_string(metadata.dt)<<','<<std::to_string(metadata.totalTimesteps)<<','<<std::to_string(metadata.neuronPopId)<<'\n';
    for (std::vector<double>& neuronRec : parsedData){
        stream<<"N_"<<std::to_string(neuronIndex)<<"=";
        for (double spiketime : neuronRec){
            stream<<std::to_string(spiketime)<<",";
        }
        neuronIndex++;
    }
    //Here I will have to first create the ofstream. Then basically iterate over the vector and print each vector with a comma separator and write in desired format:
    //M(etadata)=noNeurons,dt,totaltimesteps,neuronPopId
    //N(euronpop)_1=spiketime1,spiketime2,
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
                std::cout <<fileNamesToParse.at(index)<<" file does not exist\n";
                std::cout << "*************************\n";
                throw;
            }
        //Now we assume the file exist
        writeSpikeTimesFile(parseSpikesToSpikeTimes(fileStream, metaDataForFiles.at(index)), this->indexToParsedOutputStreamFilePath(index), metaDataForFiles.at(index));
        remove(fileNamesToParse.at(index).c_str()); //This should delete the already parsed and converted file
        index++;
    }
    std::cout<<"\nParsing operations are finished.\n";
    } else {
        std::cout<<"\nNo parsing is needed\n";
    }
}

DataOnFile::DataOnFile(unsigned long noNeurons, double dt, int totalTimesteps,int neuronPopId): noNeurons{noNeurons}, dt{dt}, totalTimesteps{totalTimesteps}, neuronPopId{neuronPopId}
{
}
