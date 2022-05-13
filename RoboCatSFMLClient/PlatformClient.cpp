//Written by Paul Bichler (D00242563)
#include "RoboCatClientPCH.hpp"


PlatformClient::PlatformClient(const sf::Int8 id, const EPlatformType platform_type)
	: Platform(id, platform_type)
{
	SetType(platform_type);
}

void PlatformClient::AddPlatformPart(PlatformPart* tile)
{
	Platform::AddPlatformPart(tile);

	const auto client_platform_part = dynamic_cast<PlatformPartClient*>(tile);

	//In the tileset, the pulse and normal vertical platforms uses a gray platform sprite,
	//which has to be replaced here.
	if (m_is_pulse || tile->GetPlatform()->GetPlatformType() == EPlatformType::kVerticalNormal)
	{
		client_platform_part->SetTexture(m_current_texture,
							  sf::IntRect(0, 0, m_current_texture->getSize().x,
										  m_current_texture->getSize().y));
	}
}

//Written by Paul Bichler (D00242563)
//This method is used to decide whether a player will collide with the platform.
//It also decides how the platform reacts to the collision.
bool PlatformClient::HandlePlayerCollisionAndChangeColor(const EColorType color_type)
{
	switch (m_type)
	{
	//Impact Platforms change color to the color of the colliding player
	case EPlatformType::kHorizontalImpact:
		if (color_type == EColorType::kBlue)
			SetType(EPlatformType::kHorizontalBlue);
		else
			SetType(EPlatformType::kHorizontalRed);

		AudioManager::sInstance->Play("PlatformColorChange");
		break;
	//Impact Platforms change color to the color of the colliding player
	case EPlatformType::kVerticalImpact:
		if (color_type == EColorType::kBlue)
			SetType(EPlatformType::kVerticalBlue);
		else
			SetType(EPlatformType::kVerticalRed);

		AudioManager::sInstance->Play("PlatformColorChange");
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

bool PlatformClient::HandlePlayerCollision(const EColorType color_type)
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
void PlatformClient::SetType(const EPlatformType type)
{
	Platform::SetType(type);

	switch (type)
	{
	case EPlatformType::kHorizontalBlue:
	{
		SetTextureOnParts(TextureManager::sInstance->GetTexture("HImpactBluePlatform"));
	}
	break;
	case EPlatformType::kHorizontalRed:
	{
		SetTextureOnParts(TextureManager::sInstance->GetTexture("HImpactRedPlatform"));
	}
	break;
	case EPlatformType::kVerticalBlue:
	{
		SetTextureOnParts(TextureManager::sInstance->GetTexture("VImpactBluePlatform"));
	}
	break;
	case EPlatformType::kVerticalRed:
	{
		SetTextureOnParts(TextureManager::sInstance->GetTexture("VImpactRedPlatform"));
	}
	break;
	case EPlatformType::kHorizontalPulse:
	{
		m_is_pulse = true;
		SetType(EPlatformType::kHorizontalRed);
	}
	break;
	case EPlatformType::kCheckpointActivated:
	{
		SetTextureOnParts(TextureManager::sInstance->GetTexture("ActivatedCheckpointPlatform"));
	}
	break;
	case EPlatformType::kHorizontalImpact:
	{
		SetTextureOnParts(TextureManager::sInstance->GetTexture("HImpactBlankPlatform"));
	}
	break;
	case EPlatformType::kVerticalImpact:
	{
		SetTextureOnParts(TextureManager::sInstance->GetTexture("VImpactBlankPlatform"));
	}
	break;
	case EPlatformType::kVerticalNormal:
	{
		SetTextureOnParts(TextureManager::sInstance->GetTexture("VNormalPlatform"));
	}
	break;
	default:
		break;
	}
}

//Written by Paul Bichler (D00242563)
//Update method is used by Pulse platforms to change the color every 2 seconds.
void PlatformClient::Update()
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

		m_current_pulse_cooldown = 1.5f;
	}
}

//Written by Paul Bichler (D00242563)
//This method is used to change the texture on all the platform parts with a specified texture
void PlatformClient::SetTextureOnParts(const TexturePtr& texture)
{
	m_current_texture = texture;

	for (const auto part : m_platform_parts)
	{
		const auto client_part = dynamic_cast<PlatformPartClient*>(part);
		client_part->SetTexture(texture, sf::IntRect(0, 0, texture->getSize().x, texture->getSize().y));
	}
}
