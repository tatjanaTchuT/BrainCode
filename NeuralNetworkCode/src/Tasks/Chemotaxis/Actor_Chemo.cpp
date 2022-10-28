//Both actor and gradients have max positions and max move vectors determined by params?PENDING
//For now they are global constexpr PENDING
#include "Actor_Chemo.h"
#include <valarray>

void Chemo_Actor::UpdateVelocityVector() {
	//Put limits on speed here, alter operations? PENDING
	SetAccelerationVector();//Has to transform [0,1,0,0] to [0,0,0,-1] if velocity can reach maximumPENDING
	velocity_vector += acceleration_vector;
	ActorPositionUpdate();
	//velocity decay
};

void Chemo_Actor::ActorPositionUpdate() {
	position += velocity_vector;

}