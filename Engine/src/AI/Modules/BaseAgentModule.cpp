//
// Created by samle on 04/12/2025.
//

#include "AI/Modules/BaseAgentModule.h"
#include "AI/Agent.h"

BaseAgentModule::BaseAgentModule(const Agent& _agent)
    : agent(_agent), agentTransform(*_agent.getTransform())
{
}