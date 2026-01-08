#pragma once

#include "Behaviour/Behaviour.h"
#include "Networking/Serialization/RegistrationBase.h"

class RigidBody: public Behaviour, RegistrationBase<RigidBody>
{
   public:
	static constexpr const char* name()
	{
		return "RigidBody";
	}
	const char* getName() const override { return name(); }

	enum class BodyType
	{
		Static,
		Dynamic,
		Kinematic
	};

	void makeStatic()
	{
		bodyType = BodyType::Static;
	}

	void makeDynamic()
	{
		bodyType = BodyType::Dynamic;
	}

	void makeKinematic()
	{
		bodyType = BodyType::Kinematic;
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

	void serialize(WriteArchive& archive) const override;
	void deserialize(ReadArchive& archive) override;

   protected:
	void onStart() override;
	void onDestroy() override;

   private:
	BodyType bodyType = BodyType::Dynamic;
	bool fixedRotation = false;
	float linearDamping = 0.0f;
	float gravityScale = 1.0f;
	bool isBullet = false;
};
