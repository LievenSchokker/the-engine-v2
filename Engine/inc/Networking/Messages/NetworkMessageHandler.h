//
// Created by samle on 24/11/2025.
//

#pragma once

#include "IMessage.h"

namespace spelmotor_networking
{
    class NetworkMessageHandler
    {
        public:
            virtual ~NetworkMessageHandler() = default;
            void handleMessage(const IMessage& message);
    };
}