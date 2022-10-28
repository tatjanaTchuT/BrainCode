#ifndef _CHEMOTAXIS_TASK_ACTOR_HEADER_
#define _CHEMOTAXIS_TASK_ACTOR_HEADER_
#include <valarray>
//Remember, the position of the receptors changes the same way the central position changes
//Probably 24 receptors is the good amount
class Chemo_Actor {
protected:
	std::valarray<double> position;
	std::valarray<double> velocity_vector;
	std::valarray<double> acceleration_vector;
	int noReceptors;
	bool negativeReceptors;

public:
	Chemo_Actor();
	~Chemo_Actor();

	//Movement update functions
	void UpdateVelocityVector();
	void SetAccelerationVector();
	//Position update functions
	void ActorPositionUpdate();


};
#endif