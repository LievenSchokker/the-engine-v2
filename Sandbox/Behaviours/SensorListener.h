
#include <iostream>

class SensorListener: public Behaviour
{
   public:
	void onSensorEnter(Collider* other) override
	{
		std::cout << "Sensor hit by: " << other->getGameObject()->getName()
				  << std::endl;
	}

	void onSensorExit(Collider* other) override
	{
		std::cout << "Sensor STOPPED touching: "
				  << other->getGameObject()->getName() << std::endl;
	}
};