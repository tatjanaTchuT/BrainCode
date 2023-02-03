#include "DatafileParser.hpp"

DatafileParser::DatafileParser(AdvancedRecorder& recorder)
{
    parsingNOutputBool=recorder.streamingNOutputBool;//We keep it to run parse()
    if(parsingNOutputBool){

    }
    //Here we have to access the recorder to obtain the filepaths/filenames and paths in general (and the metadata)
    //Then we have to convert the filepaths into the proper ifstreams
    //After this constructor it is no longer necessary to keep the NN object in scope
}

std::string DatafileParser::indexToFilePath(int vecIndex)
{
    //This is to access the filename and add the necessary path more cleanly
    return std::string();
}
std::string DatafileParser::indexToParsedFilePath(int vecIndex)
{
    //This is to access the filename and add the necessary parsed path more cleanly
    return std::string();
}


std::vector<int> DatafileParser::entryToNeuronIndexes(FileEntry fileEntry)
{
    //Here we check the 1s and output the neuron indexes/numbers
    return std::vector<int>();
}

std::vector<std::vector<int>> DatafileParser::parseSpikesToSpikeTimes(std::string rfilePath)
{
    //Here we can use the FileEntry struct functions to parse a line, then iterate over values(two different loops). Name will be the timestep, luckily enough. 
    //If 1 is found, timestep added to the neuron's vector.
    //It will be necessary to properly use the indexes to achieve this (obtained in the  function above)
    //Use this return to call the write function
    return std::vector<std::vector<int>>();
}

void DatafileParser::writeSpikeTimesFile(std::vector<std::vector<int>> parsedData, std::string wfilePath)
{
    //Here I will have to first, create the filepaths, then the ofstreams in a vector with same index. Then basically iterate over the 
}

void DatafileParser::parse()
{
    if(parsingNOutputBool){
        
    }
}
