#include "RoboCatClientPCH.hpp"

bool CollisionHandler::CheckPlatform(const Platform* platform, EColorType character)
{
	const auto platform_type = platform->GetPlatformType();

	if (character == EColorType::kBlue)
	{
		if (platform_type == EPlatformType::kHorizontalBlue ||
			platform_type == EPlatformType::kVerticalBlue)
		{
			return true;
		}
	}
	else if (character == EColorType::kRed)
	{
		if (platform_type == EPlatformType::kHorizontalRed ||
			platform_type == EPlatformType::kVerticalRed)
		{
			return true;
		}
	}

	return false;
}

bool CollisionHandler::IsVerticalPlatform(EPlatformType platform_type)
{
	return platform_type == EPlatformType::kVerticalBlue ||
		platform_type == EPlatformType::kVerticalImpact ||
		platform_type == EPlatformType::kVerticalRed;
}

bool CollisionHandler::MatchesCategories(GameObject::GOPair& collision, Category::Type type1, Category::Type type2)
{
	const unsigned int category1 = collision.first->GetCategory();
	const unsigned int category2 = collision.second->GetCategory();

	if (type1 & category1 && type2 & category2)
	{
		return true;
	}

	if (type1 & category2 && type2 & category1)
	{
		std::swap(collision.first, collision.second);
		return true;
	}

	return false;
}

void CollisionHandler::IsAtTheFinishLine(const Character* player_1, const Character* player_2,
                                         const std::function<void()>& checkpoint_callback, const std::function<void()>& win_callback,
                                         const Platform* platform)
{
	const EPlatformType platform_type = platform->GetPlatformType();

	//Check Win Condition
	if (platform_type == EPlatformType::kCheckpoint)
	{
		if (player_1->IsOnPlatformOfType(platform_type))
		{
			checkpoint_callback();
		}
	}

	if (platform_type == EPlatformType::kGoal)
	{
		if (player_1 == nullptr ||
			player_2 == nullptr)
			return;

		const bool player_1_check = player_1->IsOnPlatformOfType(platform_type);
		const bool player_2_check = player_2->IsOnPlatformOfType(platform_type);

		if (player_1_check && player_2_check)
		{
			win_callback();
		}
	}
}

void CollisionHandler::ChangeVerticalPlatformColor(const Character& player, Platform* platform)
{
	// Set color of vertical platform if there is a collision from the side/underneath 
	if (platform->GetPlatformType() == EPlatformType::kVerticalImpact)
	{
		platform->HandlePlayerCollision(player.GetCharacterType());
	}
}

void CollisionHandler::StopPlayerMovement(CharacterClient& player, const PlatformPart& platform_part,
	Platform* platform)
{
	//Checks if platform is collidable with player
	if (IsPlayerAtHisPlatform(player, platform))
	{
		// move player out of collision and stop his movement
		player.MoveOutOfCollision(platform_part.GetBoundingRect());
		player.StopMovement();

		ChangeVerticalPlatformColor(player, platform);
	}
}

bool CollisionHandler::CollideAndChangeColors(CharacterClient& player, const PlatformPart& platform_part,
	Platform* platform)
{
	//Checks if player collided from underneath the center of the platform
	if (IsPlayerAbovePlatform(player, platform_part))
	{
		StopPlayerMovement(player, platform_part, platform);
		// continue to next pair
		return true;
	}
	return false;
}

bool CollisionHandler::IsPlayerAbovePlatform(const Character& player, const PlatformPart& platform_part)
{
	if (player.GetLocation().mY > platform_part.GetLocation().mY)
	{
		return true;
	}

	return false;
}

bool CollisionHandler::IsPlatformStatic(EPlatformType platform_type)
{
	if (platform_type == EPlatformType::kNormal ||
		platform_type == EPlatformType::kGoal ||
		platform_type == EPlatformType::kCheckpoint ||
		platform_type == EPlatformType::kCheckpointActivated)
	{
		return true;
	}
	return false;
}

bool CollisionHandler::IsPlatformStillChangeable(EPlatformType platform_type)
{
	return platform_type == EPlatformType::kHorizontalImpact ||
		platform_type == EPlatformType::kVerticalImpact;
}

bool CollisionHandler::IsPlayerAtHisPlatform(const Character& player, const Platform* platform)
{
	const auto platform_type = platform->GetPlatformType();

	if (IsPlatformStatic(platform_type))
		return true;

	if (IsPlatformStillChangeable(platform_type))
		return true;

	return CheckPlatform(platform, player.GetCharacterType());
}

bool CollisionHandler::CheckPlatformUnderneath(EColorType color, EPlatformType platform)
{
	if (IsPlatformStatic(platform))
		return true;

	if (color == EColorType::kRed)
	{
		if (platform == EPlatformType::kVerticalRed ||
			platform == EPlatformType::kHorizontalRed)
		{
			return true;
		}
	}

	if (color == EColorType::kBlue)
	{
		if (platform == EPlatformType::kVerticalBlue ||
			platform == EPlatformType::kHorizontalBlue)
		{
			return true;
		}
	}

	return false;
}

void CollisionHandler::GroundPlayerAndChangePlatformColor(Character& player, Platform* platform)
{
	//Ground players
	if (platform->HandlePlayerCollisionAndChangeColor(player.GetCharacterType()))
	{
		const int player_id = player.GetPlayerID();
		const int platform_id = platform->GetID();
		const auto platform_type = platform->GetPlatformType();

		NetworkManagerClient::sInstance->SendPlatformInfo(player_id, platform_id, platform_type);

		if (IsVerticalPlatform(platform_type))
			return;

		player.SetGrounded(platform);
	}
}

bool CollisionHandler::PlatformCollision(GameObject::GOPair pair,
	const std::function<void()>& checkpoint_callback, const std::function<void()>& win_callback,
	const WorldClient* world)
{
	if (MatchesCategories(pair, Category::Type::kPlayer, Category::Type::kPlatform))
	{
		auto& player = dynamic_cast<CharacterClient&>(*pair.first);
		const auto& platform_part = dynamic_cast<PlatformPart&>(*pair.second);
		Platform* platform = platform_part.GetPlatform();

		if (CollideAndChangeColors(player, platform_part, platform)) return true;

		GroundPlayerAndChangePlatformColor(player, platform);
		IsAtTheFinishLine(&player, world->GetTeammate(), checkpoint_callback,
			win_callback, platform);
	}
	return false;
}

void CollisionHandler::PlayerGroundRayCast(GameObject::GOPair& pair)
{
	if (MatchesCategories(pair, Category::Type::kRay, Category::Type::kPlatform))
	{
		const auto& ray_ground = dynamic_cast<RayGround&>(*pair.first);
		const auto& platform_part = dynamic_cast<PlatformPart&>(*pair.second);
		const auto platform = platform_part.GetPlatform();
		const Character* player = ray_ground.m_character;

		// Check if platform underneath is valid
		if (CheckPlatformUnderneath(player->GetCharacterType(), platform->GetPlatformType()))
		{
			//collision found
			return;
		}
	}

	// check to see which object in pair is the ray 
	if (pair.first != nullptr && (pair.first->GetCategory() & Category::Type::kRay) != 0)
	{
		//call set falling
		const auto& ray_ground = dynamic_cast<RayGround&>(*pair.first);
		ray_ground.SetFalling();
	}
	else if (pair.second != nullptr && (pair.second->GetCategory() & Category::Type::kRay) != 0)
	{
		const auto& ray_ground = dynamic_cast<RayGround&>(*pair.second);
		ray_ground.SetFalling();
	}
}

void CollisionHandler::TrapCollision(GameObject::GOPair pair, const std::function<void()>& callback)
{
	//Check Lose Condition
	if (MatchesCategories(pair, Category::Type::kPlayer, Category::Type::kTrap))
	{
		//Lose
		callback();
	}
}
