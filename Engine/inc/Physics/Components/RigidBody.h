#pragma once

#include "Behaviour/Behaviour.h"
#include "Core/GameWorld.h"
#include "Networking/Serialization/RegistrationBase.h"
#include "Physics/IPhysicsWorld.h"

class	RigidBody: public Behaviour, public RegistrationBase<RigidBody>
{
public:
	static constexpr const char* name()
	{
		return "RigidBody";
	}
	const char* getName() const override { return name(); }

	bool isDynamic = true;

	void makeStatic()
	{
		isDynamic = false;
	}

	void makeDynamic()
	{
		isDynamic = true;
	}

	Vector2 linearVelocity{0.0f, 0.0f};
	float angularVelocity{0.0f};

	void serialize(WriteArchive& archive) const override
	{
		Component::serialize(archive);
		archive.process(isDynamic);
		archive.process(linearVelocity.x);
		archive.process(linearVelocity.y);
		archive.process(angularVelocity);
	}

	void deserialize(ReadArchive& archive) override
	{
		Component::deserialize(archive);
		archive.process(isDynamic);
		archive.process(linearVelocity.x);
		archive.process(linearVelocity.y);
		archive.process(angularVelocity);
	}

   protected:
	void onStart() override;
	void onDestroy() override;
};