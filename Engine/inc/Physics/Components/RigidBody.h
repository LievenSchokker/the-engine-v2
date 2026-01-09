#pragma once

#include "Behaviour/Behaviour.h"
#include "Core/GameWorld.h"
#include "Networking/Serialization/RegistrationBase.h"
#include "Physics/IPhysicsWorld.h"

class RigidBody: public Behaviour, public RegistrationBase<RigidBody>
{
   public:
	static constexpr const char* name()
	{
		return "RigidBody";
	}

	const char* getName() const override
	{
		return name();
	}

	enum class BodyType
	{
		Static,
		Dynamic,
		Kinematic
	};

	bool isDynamic = true;

	void makeStatic()
	{
		bodyType = BodyType::Static;
		isDynamic = false;
	}

	void makeDynamic()
	{
		bodyType = BodyType::Dynamic;
		isDynamic = true;
	}

	void makeKinematic()
	{
		bodyType = BodyType::Kinematic;
		isDynamic = false;
	}

	BodyType getBodyType() const
	{
		return bodyType;
	}

	void setFixedRotation(bool fixed)
	{
		fixedRotation = fixed;
	}

	bool isFixedRotation() const
	{
		return fixedRotation;
	}

	void setLinearDamping(float damping)
	{
		linearDamping = damping;
	}

	float getLinearDamping() const
	{
		return linearDamping;
	}

	void setGravityScale(float scale)
	{
		gravityScale = scale;
	}

	float getGravityScale() const
	{
		return gravityScale;
	}

	void setBullet(bool bullet)
	{
		isBullet = bullet;
	}

	bool getBullet() const
	{
		return isBullet;
	}

	Vector2 linearVelocity{0.0f, 0.0f};
	float angularVelocity{0.0f};

	void serialize(WriteArchive& archive) const override;
	void deserialize(ReadArchive& archive) override;

   protected:
	void onStart() override;
	void onDestroy() override;

   private:
	BodyType bodyType = BodyType::Dynamic;
	bool fixedRotation = false;
	float linearDamping = 0.0f;
	float gravityScale = 0.0f;
	bool isBullet = false;
};
