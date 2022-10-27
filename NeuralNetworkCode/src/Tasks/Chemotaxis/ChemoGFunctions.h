#ifndef _GLOBAL_FUNCTIONS_TASK_CHEMOTAXIS_
#define _GLOBAL_FUNCTIONS_TASK_CHEMOTAXIS_
#include <vector>
#include <random>

double AbsoluteDistance2(const std::vector<double>& vector1, const std::vector<double>& vector2);
//Calculates absolute distance from 2 coordinate (x,y) vectors

double CalculateGradient(double distance);
//gradient calc from distance, careful with indetermination at 0
//-1/((e**(-x*x/1000))+1)+1

double CalculateAccuracy(double angle);
//Angular accuracy calculation, remember: 180-alpha/180

std::vector<double> CircleCoordinateCalc(int receptor, const std::vector<double>& position);
//receptor relative coordinate vector calculation

namespace Random
{
	//Everything in this namespace is supposed to run only at the beggining of the simulation
	std::random_device rd;
	std::seed_seq ss{ rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() }; // get 8 integers of random numbers from std::random_device for our seed
	std::mt19937 mt{ ss }; // initialize our Mersenne Twister with the std::seed_seq

	std::vector<double> Coordinates(double max);
	//Generate random coordinates based on min-max for sandbox. While no params, global constant
	std::vector<double> MoveVector(double max);
	//Generate random movement vector based on min-max for sandbox. While no params, global constant
}


#endif