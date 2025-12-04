//
// Created by samle on 18/11/2025.
//

#include "Behaviour/Behaviour.h"


Behaviour::~Behaviour() = default;


void Behaviour::awake()
{
    if (hasAwakened)
        return;

    hasAwakened = true;
    onAwake();
}


void Behaviour::start()
{
    if (hasStarted)
        return;

    hasStarted = true;
    onStart();
}


void Behaviour::setEnabled(const bool value)
{
    if (isEnabled == value)
        return;

    /// This might prevent crashes when attempting to enable a behaviour right after it has been deleted
    if (value == true)
        if (gameObject == nullptr || gameObject->getIsDestroyed())
            return;

    isEnabled = value;

    if (isEnabled)
        onEnable();
    else
        onDisable();
}


void Behaviour::onDestroy()
{
    Component::onDestroy();
}


bool Behaviour::getIsEnabled() const
{
    return isEnabled;
}


bool Behaviour::getIsActiveAndEnabled() const
{
    return (isEnabled && gameObject != nullptr && gameObject->getIsActive());
}


bool Behaviour::getHasAwakened() const
{
    return hasAwakened;
}


bool Behaviour::getHasStarted() const
{
    return hasStarted;
}