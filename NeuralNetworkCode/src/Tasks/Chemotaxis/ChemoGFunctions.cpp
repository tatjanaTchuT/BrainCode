/*    The paired header file
Other headers from your project
3rd party library headers
Standard library headers*/
#include "ChemoGFunctions.h"
#include "ChemoConstants.h"
#include <cmath>
#include <random>
#include <stdexcept>


double AbsoluteDistance2(const std::vector<double>& vector1, const std::vector<double>& vector2) {
	//This is intended for vectors of size 2
	if (vector1.size() != static_cast<std::vector<double>::size_type>(2))
	{
		throw std::invalid_argument("Used AbsoluteDistance on a vector that is not size 2");
	}

	else {
		std::vector<double> vector3{ vector1[0] - vector2[0],vector1[1] - vector2[1] };
		return sqrt(std::pow(vector3[0], 2) + std::pow(vector3[1], 2));
	}
}


double CalculateGradient(double distance) {
	return 1 - (1 / (exp(-std::pow(distance, 2)) / 100000 + 1));
}

double CalculateAccuracy(double angle) {
	return (180 - angle) / 180;
}
//Angular accuracy calculation, remember: 180-alpha/180

std::vector<double> CircleCoordinateCalc(int receptor, const std::vector<double>& position) {
	double theta { receptor*(360.0 / GlobalReceptorNumber) };
	double x_position{ GlobalActorRadius * cos(theta) + position[1] };
	double y_position{ GlobalActorRadius * sin(theta) + position[1] };
	std::vector<double> receptor_position { x_position,y_position };
	return receptor_position;
}
//receptor relative coordinate vector calculation

namespace Random
{
	std::vector<double> Coordinates(double max)
	{
		std::uniform_int_distribution sandbox{ 0.0, max }; // we can create a distribution in any function that needs it
		return std::vector<double> {sandbox(mt), sandbox(mt)}; // and then generate a random number from our global generator
	}
	std::vector<double> MoveVector(double max) {
		std::uniform_int_distribution moverange{ 0.0, max }; // we can create a distribution in any function that needs it
		return std::vector<double> {moverange(mt), moverange(mt), moverange(mt), moverange(mt)};
	}
}