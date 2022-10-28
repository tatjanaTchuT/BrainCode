#ifndef _TASK_BASE_CLASS_HEADER_
#define _TASK_BASE_CLASS_HEADER_
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <valarray>
#include <fstream>
#include <string>
#include "../GlobalFunctions.hpp"
#include "Chemotaxis/Actor_Chemo.h"
#include "Chemotaxis/ChemoGFunctions.h"



class  Task {
protected:

    GlobalSimInfo * info; //I proably need access to the params

    int    identifier;
    std::string task_type;

public:
    // constructor
    Task(GlobalSimInfo* info, int id);
    virtual ~Task();

    //*******************
    //Get-Functions
    //*******************
    virtual std::string GetType() = 0;
    int     GetTaskId() { return this->identifier; }


    //*******************
    //Set-Functions
    //*******************

    virtual void advect(std::vector<double>* spiker) = 0;
    virtual void LoadParameters(std::vector<std::string>* input);
    virtual void SaveParameters(std::ofstream* stream);
};
#endif