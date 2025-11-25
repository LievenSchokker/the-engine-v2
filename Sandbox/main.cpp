//
// Created by Lieven Schokker on 07/11/2025.
//

#include "EntryPoint.h"
#include "Networking/Messages/ActionMessage.h"

#include <iostream>

int main(int argc, char** argv)
{
    ActionMessage original(10, 20, "Wow It works great", 12345);
    std::cout << "Original Message:" << std::endl;
    std::cout << "  Component ID: " << original.getComponentIdentity() << std::endl;
    std::cout << "  GameObject ID: " << original.getGameObjectIdentity() << std::endl;
    std::cout << "  Action: " << original.getAction() << std::endl;
    std::cout << "  Tick: " << original.getTick() << std::endl;

    // Serialize
    auto serialized = original.serialize();
    std::cout << "\nSerialized to " << serialized.size() << " bytes" << std::endl;

    // Deserialize into new message
    ActionMessage restored;
    bool success = restored.deserialize(serialized.data(), serialized.size());

    std::cout << "Restored Message:" << std::endl;
    std::cout << "  Component ID: " << restored.getComponentIdentity() << std::endl;
    std::cout << "  GameObject ID: " << restored.getGameObjectIdentity() << std::endl;
    std::cout << "  Action: " << restored.getAction() << std::endl;
    std::cout << "  Tick: " << restored.getTick() << std::endl;


    bool matches = (original.getComponentIdentity() == restored.getComponentIdentity() &&
                   original.getGameObjectIdentity() == restored.getGameObjectIdentity() &&
                   original.getAction() == restored.getAction() &&
                   original.getTick() == restored.getTick());

    std::cout << "\nData Match: " << (matches ? "PASS" : "FAIL") << std::endl;



    return SpelMotorEntry::main(argc, argv);
}