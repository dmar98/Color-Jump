//Written by Paul Bichler (D00242563)
#include "ColorJumpClientPCH.hpp"


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
	if (m_is_pulse || tile->GetPlatform()->GetPlatformType() == kVerticalNormal)
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
	case kHorizontalImpact:
		if (color_type == kBlue)
			SetType(kHorizontalBlue);
		else
			SetType(kHorizontalRed);

		AudioManager::sInstance->Play("PlatformColorChange");
		break;
	//Impact Platforms change color to the color of the colliding player
	case kVerticalImpact:
		if (color_type == kBlue)
			SetType(kVerticalBlue);
		else
			SetType(kVerticalRed);

		AudioManager::sInstance->Play("PlatformColorChange");
		break;
	//Only the Blue Player can collide with the blue platforms
	case kHorizontalBlue:
	case kVerticalBlue:
		if (color_type != kBlue)
			return false;
		break;
	//Only the Red Player can collide with the red platforms
	case kHorizontalRed:
	case kVerticalRed:
		if (color_type != kRed)
			return false;
		break;
	case kNormal:
	case kHorizontalPulse:
	case kVerticalNormal:
	case kGoal:
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
	case kHorizontalBlue:
	case kVerticalBlue:
		if (color_type != kBlue)
			return false;
		break;
	//Only the Red Player can collide with the red platforms
	case kHorizontalRed:
	case kVerticalRed:
		if (color_type != kRed)
			return false;
		break;
	case kHorizontalImpact:
	case kVerticalImpact:
	case kNormal:
	case kHorizontalPulse:
	case kVerticalNormal:
	case kGoal:
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
	case kHorizontalBlue:
	{
		SetTextureOnParts(TextureManager::sInstance->GetTexture("HImpactBluePlatform"));
	}
	break;
	case kHorizontalRed:
	{
		SetTextureOnParts(TextureManager::sInstance->GetTexture("HImpactRedPlatform"));
	}
	break;
	case kVerticalBlue:
	{
		SetTextureOnParts(TextureManager::sInstance->GetTexture("VImpactBluePlatform"));
	}
	break;
	case kVerticalRed:
	{
		SetTextureOnParts(TextureManager::sInstance->GetTexture("VImpactRedPlatform"));
	}
	break;
	case kHorizontalPulse:
	{
		m_is_pulse = true;
		SetType(kHorizontalRed);
	}
	break;
	case kCheckpointActivated:
	{
		SetTextureOnParts(TextureManager::sInstance->GetTexture("ActivatedCheckpointPlatform"));
	}
	break;
	case kHorizontalImpact:
	{
		SetTextureOnParts(TextureManager::sInstance->GetTexture("HImpactBlankPlatform"));
	}
	break;
	case kVerticalImpact:
	{
		SetTextureOnParts(TextureManager::sInstance->GetTexture("VImpactBlankPlatform"));
	}
	break;
	case kVerticalNormal:
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
		case kHorizontalBlue:
			SetType(kHorizontalRed);
			break;
		case kHorizontalRed:
			SetType(kHorizontalBlue);
			break;
		case kVerticalBlue:
			SetType(kVerticalRed);
			break;
		case kVerticalRed:
			SetType(kVerticalBlue);
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
