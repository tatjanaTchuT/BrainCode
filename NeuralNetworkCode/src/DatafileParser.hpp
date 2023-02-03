#ifndef _DATA_FILE_PARSER_HPP_
#define _DATA_FILE_PARSER_HPP_

#include "AdvancedRecorder.hpp"
#include "GlobalFunctions.hpp"

#include <vector>
#include <iostream>

struct DataOnFile{
    int noNeurons;
    double dt;
    int total_timesteps;
    char separator {','};
};

class DatafileParser {

protected:

    //Bools will be necessary to check which parsing is needed, or whether it is needed or not
    bool parsingNOutputBool{false};
    //Add here more if there are more files to be parsed at the end of the simulation

    std::string folderPath;
    std::string extension {".dat"};

    std::vector<std::string> fileNamesToParse; //Here we store the file names to open. A similar path will be used for writing, adding _parsed to the filename
    std::vector<std::ifstream> filesToParse; //Here we store the streams to read. Streams to write can be  opened and closed locally.
    std::vector<DataOnFile> metaDataForFile; //Here we store any extra information to write in the file that may be necessary for reading the datafile in the future.

public:
    //Constructor/destructor
    explicit DatafileParser(AdvancedRecorder& recorder);
    ~DatafileParser()=default;

    std::string indexToFilePath(int vecIndex);//To iterate through the files
    std::string indexToParsedFilePath(int vecIndex);
    std::vector<int> entryToNeuronIndexes(FileEntry fileEntry);

    std::vector<std::vector<int>> parseSpikesToSpikeTimes(std::string rfilePath); //Create a vector of vectors (one per neuron) of spike times from the read file

    void writeSpikeTimesFile(std::vector<std::vector<int>> parsedData, std::string wfilePath); //

    void parse();

};



#endif