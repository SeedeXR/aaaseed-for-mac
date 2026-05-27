#include "Utils.h"

#if AAA_USE_FLEX()
#	include "math/v.h"

void flex_set_default_simulation_params( NvFlexParams& params )
{
	params.numIterations			= 5;

	set_v3( params.gravity,	.0,-9.81,.0 );

	params.radius					= 0.15f;
	params.solidRestDistance		= params.radius;
	params.fluidRestDistance		= 0.15f; //check for fluids to make somthing like radius * 0.5 to 0.7f as specified by sdk

	//general shape collision settings
	params.dynamicFriction			= 0.0f;
	params.staticFriction			= 0.0f;
	params.particleFriction			= 0.0f;
	params.restitution				= 0.2f;
	params.adhesion					= 0.0f; //we will want to change that one for paint
	params.sleepThreshold			= 0.0f;

	params.maxSpeed					= FLT_MAX;
	params.maxAcceleration			= 100.0f;

	params.shockPropagation			= 0.0f;
	params.dissipation				= 0.0f;
	params.damping					= 0.0f;
	
	params.cohesion					= 0.025f;
	params.surfaceTension			= 0.0f;
	params.viscosity				= 0.0f;
	params.vorticityConfinement		= 0.0f;
	params.anisotropyScale			= 1.0f;
	params.anisotropyMin			= 0.1f;
	params.anisotropyMax			= 2.0f;
	params.smoothing				= 1.0f;
	params.solidPressure			= 1.0f;
	params.freeSurfaceDrag			= 0.0f;
	params.buoyancy					= 1.0f;

	//we will not use cloth, if using those settings should be dynamic here, they fun to play with
	clear_v3( params.wind );
	params.drag						= 0.0f;
	params.lift						= 0.0f;

	//we will not use diffuse for now, but we can keep the standard default for now
	params.diffuseThreshold			= 100.0f;
	params.diffuseBuoyancy			= 1.0f;
	params.diffuseDrag				= 0.8f;
	params.diffuseBallistic			= 16;
	params.diffuseLifetime			= 2.0f;

	params.particleCollisionMargin	= 0.0f;
	params.shapeCollisionMargin		= 0.0f;
	params.collisionDistance		= 0.0f;
	params.numPlanes				= 0;

	params.relaxationMode			= eNvFlexRelaxationGlobal;
	params.relaxationFactor			= 1.0f;
}

#endif	//#if AAA_USE_FLEX()
