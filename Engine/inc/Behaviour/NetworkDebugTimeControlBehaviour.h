#pragma once

#include "Behaviour/NetworkBehaviour.h"
#include "Input/KeyCode.h"
#include "Networking/Serialization/RegistrationBase.h"

#include <optional>

class InputManager;
class ApplicationClock;
class GameWorld;
class NetworkBuilder;

class NetworkDebugTimeControlBehaviour final
	: public NetworkBehaviour,
	  public RegistrationBase<NetworkDebugTimeControlBehaviour>
{
   public:
	NetworkDebugTimeControlBehaviour(
		std::optional<KeyCode> pauseKey = KeyCode::P,
		std::optional<KeyCode> verySlowKey = KeyCode::NUMBER_1_AND_EXCLAMATION,
		std::optional<KeyCode> slowKey = KeyCode::NUMBER_2_AND_AT,
		std::optional<KeyCode> normalSpeedKey = KeyCode::NUMBER_3_AND_HASHMARK,
		std::optional<KeyCode> fastKey = KeyCode::NUMBER_4_AND_DOLLAR,
		std::optional<KeyCode> veryFastKey = KeyCode::NUMBER_5_AND_PERCENTAGE,
		bool printMenu = true);

	static constexpr const char* name()
	{
		return "NetworkDebugTimeControlBehaviour";
	}

	const char* getName() const override
	{
		return name();
	}

	void onStart() override;
	void update(double deltaTime, const GameWorld& gameWorld) override;
	bool shouldRunWhenPaused() const override;
	void registerNetworkMethods(NetworkBuilder& builder) override;

   private:
	void requestTogglePause();
	void requestTimeScale(double scale);

	void commandTogglePause();
	void commandTimeScale(double scale);
	void applyTimeState(double scale, bool paused);

	void broadcastTimeState();
	void maybePrintMenu();
	void updateClientCount();

	InputManager* inputManager = nullptr;
	ApplicationClock* clock = nullptr;
	bool menuPrinted = false;
	bool printMenu = true;

	std::optional<KeyCode> pauseKey;
	std::optional<KeyCode> normalSpeedKey;
	std::optional<KeyCode> slowKey;
	std::optional<KeyCode> verySlowKey;
	std::optional<KeyCode> fastKey;
	std::optional<KeyCode> veryFastKey;

	int lastClientCount = 0;
};
