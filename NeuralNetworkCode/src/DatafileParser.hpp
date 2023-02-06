#ifndef _DATA_FILE_PARSER_HPP_
#define _DATA_FILE_PARSER_HPP_
#define _MAX_CHARACTERS_PER_LINE 30002//INT_MAX/2 // This may require adjustment

#include "AdvancedRecorder.hpp"
#include "GlobalFunctions.hpp"

#include <vector>
#include <iostream>

struct DataOnFile{
    unsigned long noNeurons;
    double dt;
    int totalTimesteps;
    int neuronPopId;
    char separator {','};
    DataOnFile(unsigned long noNeurons, double dt, int totalTimesteps,int neuronPopId);
};

class DatafileParser {

protected:

    //Bools will be necessary to check which parsing is needed, or whether it is needed or not
    bool parsingNOutputBool{false};
    //Add here more if there are more files to be parsed at the end of the simulation

    std::string directoryPath;
    std::string title;
    std::string extension {".dat"};

    std::vector<int> neuronPopIds;

    std::vector<std::string> fileNamesToParse; //Here we store the file names to open. A similar path will be used for writing, adding _parsed to the filename
    std::vector<std::ifstream> filesToParse; //Here we store the streams to read. Streams to write can be  opened and closed locally.
    std::vector<DataOnFile> metaDataForFiles; //Here we s tore any extra information to write in the file that may be necessary for reading the datafile in the future.

public:
    //Constructor/destructor
    explicit DatafileParser(AdvancedRecorder& recorder);
    ~DatafileParser()=default;

    std::string indexToParsedOutputStreamFilePath(int vecIndex);
    std::vector<int> entryToNeuronIndexes(FileEntry fileEntry);

    std::vector<std::vector<double>> parseSpikesToSpikeTimes(std::ifstream& fileStream, DataOnFile metadata); //Create a vector of vectors (one per neuron) of spike times from the read file

    std::vector<FileEntry> parseFileToEntries(std::ifstream& fileStream);

    void writeSpikeTimesFile(std::vector<std::vector<double>> parsedData, std::string wfilePath); //

    void parse();

};



#endif