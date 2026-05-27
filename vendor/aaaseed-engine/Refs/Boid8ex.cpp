/*
 * Copyright (C) 1996, Christopher John Kline
 * Electronic mail: ckline@acm.org
 *
 * This software may be freely copied, modified, and redistributed
 * for academic purposes and by not-for-profit organizations, provided that
 * this copyright notice is preserved on all copies, and that the source
 * code is included or notice is given informing the end-user that the source
 * code is publicly available under the terms described here.
 *
 * Persons or organizations wishing to use this code or any modified version
 * of this code in a commercial and/or for-profit manner must contact the
 * author via electronic mail (preferred) or other method to arrange the terms
 * of usage. These terms may be as simple as giving the author visible credit
 * in the final product.
 *
 * There is no warranty or other guarantee of fitness for this software,
 * it is provided solely "as is". Bug reports or fixes may be sent
 * to the author, who may or may not act on them as he desires.
 *
 * If you use this software the author politely requests that you inform him via
 * electronic mail. 
 *
 */

// Boid.c++
//
// CODE: Class for objects that move with A-Life boid-type behavior.
//
// (c) 1996 Christopher Kline <ckline@acm.org>
//


#ifndef __BOID_C
#define __BOID_C

#include "ourmath.h"
#include "Boid.h"
#include <stdio.h>

// Returns the value in the visibility matrix which determines if this boid
// can see the "otherBoid". A value of -1 means that otherBoid cannot be
// seen; any other value indicates that otherBoid _can_ be seen, and the
// value is equal to the otherBoid's boidType.
#define visMatrix(otherBoid) \
     (Boid::visibilityMatrix[boidNumber-1][(otherBoid)->boidNumber-1])

float Boid::maxVelocity = 1.;
float Boid::maxAcceleration = 1.;  
float Boid::cruiseDistance = .5 ;
float Boid::separateFactor = 0.9;
float Boid::approachFactor = 0.5;

  
// -- Static member variables
ObjectList   Boid::boidList;
ObstacleList Boid::obstacles;
int          Boid::boidCount = 0;
int **       Boid::visibilityMatrix;
// --


void	Boid::calculateVisibilityMatrix(void) {

	NamedObject *i;
	Boid *n;   
	int val;
  
	boidList.resetIter(&i);   // reset iterator
  
	// Foreach boid, if it's not visible to us, put a a -1 in the
	// matrix. Otherwise put the boidType in the matrix.

	while ((n = (Boid *)boidList.iter(&i)) != NULL)
		{
		// Can't see self!
		if (n == this)
			val = -1;
		else
			val = (visibleToSelf(n) ? n->boidType : -1);

		visMatrix(n) = val;
		}
  
}

double
Boid::accumulate(Vector &accumulator, Vector valueToAdd) {

  double magnitudeRemaining = 1.0 - Magnitude(accumulator);
  double vMag = Magnitude(valueToAdd);
  double newMag = vMag < magnitudeRemaining ? vMag : magnitudeRemaining;

  accumulator += (Direction(valueToAdd) * newMag); 
  
  return newMag;
}


Vector	Boid::navigator()
{

	// Calculate the visibility matrix so that visibility computations are
	// much more efficient.
	calculateVisibilityMatrix();

	Vector	vacc;
	vacc = target-position;
	double	mag = Magnitude(vacc);
	double	speed = Magnitude(oldVelocity);
	if( speed > mag )
		vacc *= speed*.5/mag;
	
	vacc += maintainingCruisingDistance();

//	accumulate(vacc, collisionAvoidance());
//	accumulate(vacc, target-position);
//	accumulate(vacc, flockCentering());
//	accumulate(vacc, maintainingCruisingDistance());
//	accumulate(vacc, velocityMatching());
//	accumulate(vacc, wander());
//	if ( accumulate(vacc, levelFlight(vacc)) >= 1.0)              goto MAXACCEL_ATTAINED;


/*
	Vector vacc(0, 0, 0);  // vector accumulator

//	if ( accumulate(vacc, collisionAvoidance()) >= 1.0)           goto MAXACCEL_ATTAINED;
	if ( accumulate(vacc, (target-position)*.5) >= 1.0)			  goto MAXACCEL_ATTAINED;
//	accumulate(vacc, target-position);
//	if ( accumulate(vacc, flockCentering()) >= 1.0)               goto MAXACCEL_ATTAINED;
	if ( accumulate(vacc, maintainingCruisingDistance()) >= 1.0)  goto MAXACCEL_ATTAINED;
//	if ( accumulate(vacc, velocityMatching()) >= 1.0)             goto MAXACCEL_ATTAINED;
//	if ( accumulate(vacc, wander()) >= 1.0)                       goto MAXACCEL_ATTAINED;
//	if ( accumulate(vacc, levelFlight(vacc)) >= 1.0)              goto MAXACCEL_ATTAINED;
*/
  
MAXACCEL_ATTAINED: 
	// IMPORTANT:
	// Since the FlockCentering, CollisionAvoidance, and VelocityMatching modules
	// return a vector whose magnitude is a percentage of the maximum acceleration,
	// we need to convert this to a real magnitude before we hand it off to the Flight
	// module. 
	// Remember, maxAcceleration is in terms of a fraction (0.0 to 1.0) of maxVelocity

	vacc *= maxAcceleration * maxVelocity; 
	return(vacc);
}

void
Boid::calculateRollPitchYaw(Vector appliedAcceleration,
			    Vector currentVelocity,
			    Vector /*currentPosition*/) {
  // NOTES:
  // 
  // 1) currentPosition (the third argument) is unused, though in a more
  // advanced flight model it could make a difference (some flight dynamics
  // change depending on altitude, for example)
  //
  // 2) Dr. Craig Wanke of the Mitre Corporation in McLean, Virginia helped me
  // tremendously when working out these equations. Thanks, Craig!
  
  // In our simple flight model, roll only a function of lateral
  // (centripedal) acceleration and gravitational acceleration. We assume
  // that gravitational acceleration will NOT be zero. 

  // Determine the direction of the lateral acceleration.
  Vector lateralDir = Direction((currentVelocity % appliedAcceleration) % currentVelocity);
  
  // Set the lateral acceleration's magnitude. The magnitude is the vector
  // projection of the appliedAcceleration vector onto the direction of the
  // lateral acceleration).
  double lateralMag = appliedAcceleration * lateralDir;
  
  if (lateralMag == 0)
    roll = 0;  
  else
    roll = -ATAN2(getGravAcceleration(), lateralMag) + PI_BY_2;
  pitch = -ATAN(currentVelocity.y / 
		SQRT(currentVelocity.z*currentVelocity.z
		     + currentVelocity.x*currentVelocity.x));

  yaw = ATAN2(currentVelocity.x, currentVelocity.z);

}

Vector 
Boid::wander(void) {

  double distanceFromCruiseSpeed =
    (Magnitude(velocity) - desiredCruisingSpeed()) / maxVelocity;

  double urgency = fabs(distanceFromCruiseSpeed);
  if (urgency > 0.25)
    urgency = 0.25;
  
  return Direction(velocity) * urgency * (distanceFromCruiseSpeed > 0 ? -1 : 1);

}

Vector 
Boid::collisionAvoidance(void) {
    Obstacle *obs;
    ISectData d;
    Vector normalToObject(0, 0, 0);
    int objectSeen = 0;
    Vector pointOnObject;

    // Ignore obstacles that are out of the range of our probe.
    double distanceToObject = getProbeLength();

    // Find closest imminent collision with non-boid object 
    obstacles.ResetIter();
    while ((obs = obstacles.Iter()) != NULL) {
      
      d = obs->DoesRayIntersect(Direction(velocity), position);

      if (d.intersectionflag == 1) {      

	// Velocity vector intersects an obstacle
	
	if (Magnitude(d.point-position) <= distanceToObject) {
	  // found a closer object
	  objectSeen = 1; 
	  distanceToObject = Magnitude(d.point-position);
	  normalToObject = d.normal;
	  pointOnObject = d.point;
	}
	
      }

    }
    
    if (!objectSeen) {
      return Vector(0,0,0);
    }

    return resolveCollision(pointOnObject, normalToObject); 
}
  
Vector 
Boid::resolveCollision(Vector pointOnObject, Vector normalToObject) {

  double distanceToObject = Magnitude(pointOnObject - position);
  
  // Make sure the object's normal is pointing towards the boid.
  // The boid wants to head in the direction of the normal, which
  // should push it away from the obstacle if the normal is pointing
  // towards the boid.
  if (AngleBetween(velocity, normalToObject) < PI_BY_2) 
    normalToObject = - normalToObject;

  double mag = 1 - distanceToObject/getProbeLength();

  // Ignore objects that are farther away than our probe.
  if (mag < 0) mag = 0;

  return Direction(normalToObject) * mag;
}

Vector	Boid::maintainingCruisingDistance(void)
{
	double distanceToClosestNeighbor = REAL_BIG_VALUE; // DBL_MAX defined in <limits.h>
	int foundClosestNeighbor = 0;
  
	NamedObject *i;
	Boid *n, *closestNeighbor;   
	double tempDistance;
  
	boidList.resetIter(&i);   // reset iterator
  
	while ((n = (Boid *)boidList.iter(&i)) != NULL)
		{
		// Skip boids that we don't need to consider
		if (visMatrix(n) == -1) continue; 
		if (visMatrix(n) != boidType && flockSelectively) continue; 
    
		// Find distance from the current boid to self
		tempDistance = Magnitude(n->position - position);
    
		// remember distance to closest boid
		if (tempDistance < distanceToClosestNeighbor)
			{		
			distanceToClosestNeighbor = tempDistance;
			foundClosestNeighbor = 1;
			closestNeighbor = n;
			}
		}
  
	Vector speedAdjustmentVector(0, 0, 0);

	if (foundClosestNeighbor)
		{
		// Have the boid try to remain at least cruiseDistance away from its
		// nearest neighbor at all times in all directions (i.e., don't violate
		// your neighbor's "personal space" bounding sphere of radius
		// cruiseDistance, but stay as close to the neighbor as possible). 

		Vector separationVector = closestNeighbor->getPosition() - position;
		tempDistance = Magnitude(separationVector);

		if( tempDistance < cruiseDistance )
			speedAdjustmentVector = separationVector*separateFactor;
		else
			speedAdjustmentVector = separationVector*approachFactor;

		}
	// -- Otherwise, if we couldn't find a closest boid, speedAdjustmentVector 
	//    will have a magnitude of 0 and thus have no effect on navigation.

	return(speedAdjustmentVector);
}


Vector Boid::velocityMatching(void)
{
	Vector velocityOfClosestNeighbor(0,0,0);
	double tempDistance;
	double distanceToClosestNeighbor = REAL_BIG_VALUE;

	NamedObject *i;
	Boid *n;   

	boidList.resetIter(&i);   // reset iterator

	while ((n = (Boid *)boidList.iter(&i)) != NULL)
		{
		// Skip boids that we don't need to consider
		if (visMatrix(n) == -1)
			continue;
		if (visMatrix(n) != boidType && flockSelectively)
			continue;

		// Find distance from the current boid to self
		tempDistance = Magnitude(n->position - position);

		// remember velocity vector of closest boid
		if (tempDistance < distanceToClosestNeighbor)
			{
			distanceToClosestNeighbor = tempDistance;
			velocityOfClosestNeighbor = n->velocity;
			}
		}
  
	// If we found a close boid, set the percentage of our acceleration that
	// we want to use in order to begin flying parallel to its velocity vector.
	// -- Otherwise, if we couldn't find a closest boid, velocityOfClosestNeighbor 
	//    will have a magnitude of 0 and thus have no effect on navigation.
	if ( distanceToClosestNeighbor != REAL_BIG_VALUE )
		{
		// return velocity vector of closest boid so we can try to match it
		velocityOfClosestNeighbor.SetMagnitude(0.05);
		}

	return( velocityOfClosestNeighbor );
}

Vector Boid::flockCentering()
{
Vector t;
double boids_observed = 0;   // number of boids that were checked 
Vector flockcenter(0,0,0);   // approximate center of flock

NamedObject *i;
Boid *n;   
  
	boidList.resetIter(&i);   // reset iterator
  
	// Calculate approximate center of flock by averaging the positions of all
	// visible boids that we are flocking with.
	while ((n = (Boid *)boidList.iter(&i)) != NULL)
		{
		if (visMatrix(n) == -1)
			continue; 
		if (visMatrix(n) != boidType && flockSelectively)
			continue; 
   
		flockcenter += n->position;
		boids_observed++;
		}
  
	if (boids_observed != 0)
		{ 
		flockcenter /= boids_observed;
    
		// now calculate a vector to head towards center of flock
		t = flockcenter - position;
    
		// and the percentage of maximum acceleration (in decimal) to use when yaw toward center
		t.SetMagnitude(0.1); 
		}
	else
		{
		// Don't see any other boids. 
		t.SetMagnitude(0);
		}

	return(t);
}

bool	Boid::update(const double &elapsedSeconds)
{
BOOL	b_ok = TRUE;
  
	if (b_flight == FALSE)
		b_flight = TRUE;
	else
		{
		double dt = elapsedSeconds - lastUpdate;
    
		// Step 1: Calculate new position and velocity by integrating
		//         the acceleration vector.
    
		// Update position based on where we wanted to go, and how long it has 
		// been since we made the decision to go there
		position += (oldVelocity*dt + 0.5*acceleration*dt*dt);
    
		// Set new velocity, which will be in the direction boid is traveling.
		velocity += (acceleration*dt);
    
		// Cap off velocity at maximum allowed value
		if (Magnitude(velocity) > maxVelocity)
			velocity.SetMagnitude(maxVelocity);
    
		// Step 2: Calculate new roll, pitch, and yaw of the boid
		//         which correspond to the changes in position and velocity.
		//         Remember: the boid isn't necessarily oriented in the
		//         direction of the velocity!
		// (assuming +y is up, +z is through the nose, +x is through the left wing)
		calculateRollPitchYaw(acceleration, oldVelocity, position);
		}    
  
	// remember current velocity 
	oldVelocity = velocity; 

	// remember desired acceleration (the acceleration vector that the
	// Navigator() module specified
	acceleration = navigator(); 
  
	b_ok &= SimObject::update(elapsedSeconds); // Do generic object  updating

	return b_ok;
}

Boid::~Boid()
{
	boidCount--;
	boidList.remove(bName);

}

Boid::Boid(Vector bPosition, Vector bVelocity, Vector bDimensions)
{
  
	b_flight = FALSE;	      // haven't flown yet

	position = bPosition;
	velocity = bVelocity;
	dimensions = bDimensions; // width, height, length
	mass = 1.; // 9.07 kg is approx 20 lbs.

//	maxVelocity = 10.0;
//	maxAcceleration = 2.;
	roll = pitch = yaw = 0;
	boidType = NORMAL;
	cruiseDistance = 5.*dimensions.z; // Try to stay at least one bodywidth apart
	flockSelectively = FALSE; 

	bodyLength = bDimensions.z;

	boidCount++;
	boidNumber = boidCount;
	//cerr << "\nConstructing boid " << boidNumber << endl;
	sprintf(bName, "boid%d", boidCount);
	boidList.add(*this, bName);

	// Allocate a new visibility matrix dynamically each time a boid is
	// added. 
	int **newMatrix = new int*[boidCount];
	int i;
	for (i = 0; i < boidCount; i++)
		newMatrix[i] = new int[boidCount];

	// Copy the old visibility matrix before we delete it.
	int j;
	for (i = 0; i < boidCount-1; i++)
		{
		for (j = 0; j < boidCount-1; j++)
			{
			//cerr << "i=" << i << " j=" << j<<endl;
			newMatrix[i][j] = visibilityMatrix[i][j];
			}
		}

	int **temp = visibilityMatrix;
	visibilityMatrix = newMatrix;

	// Delete the old matrix.
	for (i = 0; i < boidCount-1; i++)
		delete[] temp[i];
	delete[] temp;
  
}		

				
#endif /* __BOID_C */
