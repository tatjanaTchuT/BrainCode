#ifndef _CHEMOTAXIS_TASK_CLASS_HEADER_
#define _CHEMOTAXIS_TASK_CLASS_HEADER_
//Here all the runs functions, data generation
//Load parameter function in every class, probably needed. Talk with Pierre for adaptation of parameter files?
#include <valarray>
#include <fstream>
#include "../Task.h"
class Chemotaxis : public Task {
protected:
	
	std::valarray<double> task_input;
	std::valarray<double> task_output;

public:
	void InitPosition(long noNeur);
	void ReceiveNNOutput();
	void SetNNInput();//Here I am supposed to modify the Poisson population with the receptor array
};


#endif