#include "ApplicationSpecifications.h"

class EngineLoop
{
public:
    EngineLoop(ApplicationSpecifications applicationSpecifications);
    virtual ~EngineLoop() = default;


    virtual void run() = 0;
};
