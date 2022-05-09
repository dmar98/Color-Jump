#pragma once

class CollisionHandler
{
	static bool CheckPlatform(const Platform* platform, EColorType character);
	static bool IsVerticalPlatform(EPlatformType platform_type);
	static bool MatchesCategories(GameObject::GOPair& collision, Category::Type type1, Category::Type type2);
	static void IsAtTheFinishLine(const Character* player_1, const Character* player_2,
		const std::function<void()>& checkpoint_callback,
		const std::function<void()>& win_callback, const Platform* platform);
	static void ChangeVerticalPlatformColor(const Character& player, Platform* platform);
	static void StopPlayerMovement(CharacterClient& player, const PlatformPart& platform_part,
		Platform* platform);
	static bool CollideAndChangeColors(CharacterClient& player, const PlatformPart& platform_part,
		Platform* platform);
	static bool IsPlayerAbovePlatform(const Character& player, const PlatformPart& platform_part);
	static bool IsPlatformStatic(EPlatformType platform_type);
	static bool IsPlatformStillChangeable(EPlatformType platform_type);
	static bool IsPlayerAtHisPlatform(const Character& player, const Platform* platform);
	static bool CheckPlatformUnderneath(EColorType color, EPlatformType platform);
	static void GroundPlayerAndChangePlatformColor(Character& player, Platform* platform);

public:
	static bool PlatformCollision(GameObject::GOPair pair, const std::function<void()>& checkpoint_callback,
		const std::function<void()>& win_callback, const WorldClient* world = nullptr);
	static void PlayerGroundRayCast(GameObject::GOPair& pair);
	static void TrapCollision(GameObject::GOPair pair, const std::function<void()>& callback);

};

