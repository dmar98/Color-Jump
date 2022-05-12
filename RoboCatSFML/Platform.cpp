//Written by Paul Bichler (D00242563)
#include "RoboCatPCH.hpp"

Platform::Platform(const sf::Int8 id, const EPlatformType platform_type)
	: m_type(platform_type),
	  m_initial_type(platform_type),
	  m_current_pulse_cooldown(2.f),
	  m_id(id)
{
	SetType(platform_type);
}

EPlatformType Platform::GetPlatformType() const
{
	return m_type;
}

void Platform::AddPlatformPart(PlatformPart* tile)
{
	m_platform_parts.emplace_back(tile);
}

//Written by Paul Bichler (D00242563)
//This method is used to decide whether a player will collide with the platform.
//It also decides how the platform reacts to the collision.
bool Platform::HandlePlayerCollisionAndChangeColor(const EColorType color_type)
{
	switch (m_type)
	{
	//Impact Platforms change color to the color of the colliding player
	case EPlatformType::kHorizontalImpact:
		if (color_type == EColorType::kBlue)
			SetType(EPlatformType::kHorizontalBlue);
		else
			SetType(EPlatformType::kHorizontalRed);
		break;
	//Impact Platforms change color to the color of the colliding player
	case EPlatformType::kVerticalImpact:
		if (color_type == EColorType::kBlue)
			SetType(EPlatformType::kVerticalBlue);
		else
			SetType(EPlatformType::kVerticalRed);
		break;
	//Only the Blue Player can collide with the blue platforms
	case EPlatformType::kHorizontalBlue:
	case EPlatformType::kVerticalBlue:
		if (color_type != EColorType::kBlue)
			return false;
		break;
	//Only the Red Player can collide with the red platforms
	case EPlatformType::kHorizontalRed:
	case EPlatformType::kVerticalRed:
		if (color_type != EColorType::kRed)
			return false;
		break;
	case EPlatformType::kNormal:
	case EPlatformType::kHorizontalPulse:
	case EPlatformType::kVerticalNormal:
	case EPlatformType::kGoal:
	default:
		break;
	}

	return true;
}

bool Platform::HandlePlayerCollision(const EColorType color_type)
{
	switch (m_type)
	{
	//Only the Blue Player can collide with the blue platforms
	case EPlatformType::kHorizontalBlue:
	case EPlatformType::kVerticalBlue:
		if (color_type != EColorType::kBlue)
			return false;
		break;
	//Only the Red Player can collide with the red platforms
	case EPlatformType::kHorizontalRed:
	case EPlatformType::kVerticalRed:
		if (color_type != EColorType::kRed)
			return false;
		break;
	case EPlatformType::kHorizontalImpact:
	case EPlatformType::kVerticalImpact:
	case EPlatformType::kNormal:
	case EPlatformType::kHorizontalPulse:
	case EPlatformType::kVerticalNormal:
	case EPlatformType::kGoal:
	default:
		break;
	}

	return true;
}

//Written by Paul Bichler (D00242563)
//This method is used to set the platform type.
//The platform changes sprites depending on the type.
void Platform::SetType(const EPlatformType type)
{
	m_type = type;
}

void Platform::ResetToInitialType()
{
	SetType(m_initial_type);
}

//Written by Paul Bichler (D00242563)
//Update method is used by Pulse platforms to change the color every 2 seconds.
void Platform::Update()
{
	if (!m_is_pulse)
		return;

	m_current_pulse_cooldown -= Timing::sInstance.GetDeltaTime();

	if (m_current_pulse_cooldown <= 0)
	{
		switch (m_type)
		{
		case EPlatformType::kHorizontalBlue:
			SetType(EPlatformType::kHorizontalRed);
			break;
		case EPlatformType::kHorizontalRed:
			SetType(EPlatformType::kHorizontalBlue);
			break;
		case EPlatformType::kVerticalBlue:
			SetType(EPlatformType::kVerticalRed);
			break;
		case EPlatformType::kVerticalRed:
			SetType(EPlatformType::kVerticalBlue);
			break;
		}

		m_current_pulse_cooldown = 2.f;
	}
}

int Platform::GetID() const
{
	return m_id;
}

std::vector<PlatformPart*>& Platform::GetParts()
{
	return m_platform_parts;
}

////Written by Paul Bichler (D00242563)
////This method is used to change the texture on all the platform parts with a specified texture
//void Platform::SetTextureOnParts(sf::Texture& texture)
//{
//	m_current_texture = &texture;
//
//	for (const auto part : m_platform_parts)
//		part->SetSpriteTexture(
//			texture, sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));
//}
