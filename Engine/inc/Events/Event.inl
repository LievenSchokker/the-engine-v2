#pragma once


#include "Event.h"

/**
* @brief Event base class for creation of events
 *
 * Has a template with the idea that the generic class still has
 * access to the specefic static type of the template class.
 *
 * Usage:
 *   struct MyEvent : public Event<MyEvent> { ... };
 */
template <typename T>
class Event: public EventBase
{
public:
    static uint32_t staticTypeId()
    {
        static uint32_t id = generateTypeId();
        return id;
    }

    [[nodiscard]] uint32_t getTypeId() const override
    {
        return staticTypeId();
    }
};