#include "AI/Agent.h"
#include "Component/NetworkIdentity.h"
#include "Component/Transform.h"
#include "Scene/Scene.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Physics/Components/RigidBody.h"
#include "Physics/IPhysicsWorld.h"


void Agent::onAwake()
{
    if (transform == nullptr)
    {
        setEnabled(false);
    }

    rigidBody = getComponent<RigidBody>();

    for (auto& moduleData  :  moduleDatas)
    {
        moduleData->getModule()->initialise();
    }
}


void Agent::update(double deltaTime, const GameWorld& gameWorld)
{
    if (gameWorld.isClient() && !gameWorld.isServer())
    {
        auto* identity = getComponent<NetworkIdentity>();
        if (identity != nullptr)
        {
            const int ownerId = identity->getOwnerId();
            if (ownerId < 0 || ownerId != gameWorld.localClientId)
            {
                return;
            }
        }
    }

    Vector2 velocity = computeDesiredVelocity();

    if (rigidBody != nullptr && gameWorld.physics != nullptr)
    {
        if (velocity.magnitude() > 0.0f && !rigidBody->isFixedRotation())
        {
            transform->rotateTowards(velocity, rotationTurnRate, deltaTime);
        }

        const Vector2 currentVelocity =
            gameWorld.physics->getLinearVelocity(rigidBody);
        const Vector2 velocityDelta = velocity - currentVelocity;
        const float accelStrength = std::max(minAcceleration, maxSpeed * accelerationMultiplier);
        gameWorld.physics->applyForce(rigidBody, velocityDelta * accelStrength);
        return;
    }

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

float Agent::getAccelerationMultiplier() const
{
    return accelerationMultiplier;
}

void Agent::setAccelerationMultiplier(float value)
{
    accelerationMultiplier = value;
}

float Agent::getMinAcceleration() const
{
    return minAcceleration;
}

void Agent::setMinAcceleration(float value)
{
    minAcceleration = value;
}


bool Agent::requestPath(const Vector2 &target)
{
    NavigationSystem* navSystem = gameObject->getScene()->getNavigationSystem();

    if (navSystem == nullptr)
        return false;

    PathResult pathResult = navSystem->computePath(*pathFinder, transform->getPosition(), target);

    if (!pathResult.isValid())
        return false;

    currentPath = pathResult;

    return pathResult.isValid();
}


const PathResult &Agent::getCurrentPath() const
{
    return currentPath;
}

bool Agent::hasPath() const
{
    return currentPath.isValid();
}



