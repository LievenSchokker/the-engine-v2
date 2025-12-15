#include "AI/Agent.h"
#include "Component/Transform.h"
#include "Scene/Scene.h"


void Agent::onAwake()
{
    if (transform == nullptr)
    {
        setEnabled(false);
    }
}


void Agent::update(float deltaTime, GameWorld* gameWorld)
{
    Vector2 velocity = computeDesiredVelocity();

    if (velocity.magnitude() > 0.0f)
    {
        transform->rotateTowards(velocity, rotationTurnRate, deltaTime);
        transform->moveTowards(transform->getPosition() + velocity, velocity.magnitude() * deltaTime);
    }
}


Vector2 Agent::computeDesiredVelocity()
{
    Vector2 desiredForce = computeModulesForce();

    if (desiredForce.magnitude() > maxSpeed)
    {
        desiredForce = desiredForce.normalised() * maxSpeed;
    }

    return desiredForce;
}


Vector2 Agent::computeModulesForce()
{
    Vector2 totalForce = Vector2::zero();

    for (std::unique_ptr<ModuleData>& moduleData : moduleDatas)
    {
        /// Skip inactive modules
        if (!moduleData->isModuleActive())
            continue;

        float weight = moduleData->getWeight();

        /// Skip computation for 0 weights.
        if (weight <= 0)
            continue;

        Vector2 direction = moduleData->getModule()->compute();

        /// Skip modules that return a zero length vector, no contributions to the resulting force.
        if (direction == Vector2::zero())
            continue;

        direction.normalize();
        totalForce += direction * weight;
    }

    /// Return the sum of all computated module's directions, multiplied by their weights
    return totalForce;
}


float Agent::getMaxSpeed() const
{
    return maxSpeed;
}


void Agent::setMaxSpeed(float max)
{
    maxSpeed = max;
}


size_t Agent::getModuleCount() const
{
    return moduleDatas.size();
}


float Agent::getArrivingDistance() const
{
    return arrivingDistance;
}


void Agent::setArrivingDistance(float distance)
{
    arrivingDistance = distance;
}


float Agent::getRotationTurnRate() const
{
    return rotationTurnRate;
}


void Agent::setRotationTurnRate(float value)
{
    rotationTurnRate = value;
}


bool Agent::tryGetPath(const Vector2& target, PathResult* out) const
{
    NavigationSystem* navSystem = gameObject->getScene().getNavigationSystem();

    if (navSystem == nullptr)
        return false;

    PathResult pathResult = navSystem->computePath(*pathFinder, transform->getPosition(), target);
    std::cout << "Target: " << target.x << target.y << std::endl;
    std::cout << "Found path, length: " << pathResult.getPathLength() << std::endl;


    if (out != nullptr)
        *out = pathResult;

    return pathResult.isValid();
}









