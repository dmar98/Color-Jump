//Written by Paul Bichler (D00242563)
#include "ColorJumpServerPCH.hpp"

PlatformServer::PlatformServer(const sf::Int8 id, const EPlatformType platform_type)
	: Platform(id, platform_type)
{
}

//Written by Paul Bichler (D00242563)
//This method is used to decide whether a player will collide with the platform.
//It also decides how the platform reacts to the collision.
bool PlatformServer::HandlePlayerCollisionAndChangeColor(const EColorType color_type)
{
	//switch (m_type)
	//{
	////Impact Platforms change color to the color of the colliding player
	//case EPlatformType::kHorizontalImpact:
	//	if (color_type == EColorType::kBlue)
	//		SetType(EPlatformType::kHorizontalBlue);
	//	else
	//		SetType(EPlatformType::kHorizontalRed);
	//	break;
	////Impact Platforms change color to the color of the colliding player
	//case EPlatformType::kVerticalImpact:
	//	if (color_type == EColorType::kBlue)
	//		SetType(EPlatformType::kVerticalBlue);
	//	else
	//		SetType(EPlatformType::kVerticalRed);
	//	break;
	////Only the Blue Player can collide with the blue platforms
	//case EPlatformType::kHorizontalBlue:
	//case EPlatformType::kVerticalBlue:
	//	if (color_type != EColorType::kBlue)
	//		return false;
	//	break;
	////Only the Red Player can collide with the red platforms
	//case EPlatformType::kHorizontalRed:
	//case EPlatformType::kVerticalRed:
	//	if (color_type != EColorType::kRed)
	//		return false;
	//	break;
	//case EPlatformType::kNormal:
	//case EPlatformType::kHorizontalPulse:
	//case EPlatformType::kVerticalPulse:
	//case EPlatformType::kGoal:
	//default:
	//	break;
	//}

	//return true;

	return false;
}

bool PlatformServer::HandlePlayerCollision(const EColorType color_type)
{
	//switch (m_type)
	//{
	////Only the Blue Player can collide with the blue platforms
	//case EPlatformType::kHorizontalBlue:
	//case EPlatformType::kVerticalBlue:
	//	if (color_type != EColorType::kBlue)
	//		return false;
	//	break;
	////Only the Red Player can collide with the red platforms
	//case EPlatformType::kHorizontalRed:
	//case EPlatformType::kVerticalRed:
	//	if (color_type != EColorType::kRed)
	//		return false;
	//	break;
	//case EPlatformType::kHorizontalImpact:
	//case EPlatformType::kVerticalImpact:
	//case EPlatformType::kNormal:
	//case EPlatformType::kHorizontalPulse:
	//case EPlatformType::kVerticalPulse:
	//case EPlatformType::kGoal:
	//default:
	//	break;
	//}

	//return true;

	return false;
}

//Written by Paul Bichler (D00242563)
//This method is used to set the platform type.
//The platform changes sprites depending on the type.
void PlatformServer::SetType(const EPlatformType type)
{
	Platform::SetType(type);

	if(type == EPlatformType::kHorizontalPulse)
	{
		m_is_pulse = true;
		SetType(EPlatformType::kHorizontalRed);
		return;
	}

	//todo send new texture and type

	/*switch (type)
	{
	case EPlatformType::kHorizontalBlue:
		{
			SetTextureOnParts(m_textures.Get(Textures::kHImpactBluePlatform));
		}
		break;
	case EPlatformType::kHorizontalRed:
		{
			SetTextureOnParts(m_textures.Get(Textures::kHImpactRedPlatform));
		}
		break;
	case EPlatformType::kVerticalBlue:
		{
			SetTextureOnParts(m_textures.Get(Textures::kVImpactBluePlatform));
		}
		break;
	case EPlatformType::kVerticalRed:
		{
			SetTextureOnParts(m_textures.Get(Textures::kVImpactRedPlatform));
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
			SetTextureOnParts(m_textures.Get(Textures::kActivatedCheckpointPlatform));
		}
		break;
	case EPlatformType::kHorizontalImpact:
		{
			SetTextureOnParts(m_textures.Get(Textures::kHImpactBlankPlatform));
		}
		break;
	case EPlatformType::kVerticalImpact:
		{
			SetTextureOnParts(m_textures.Get(Textures::kVImpactBlankPlatform));
		}
		break;
	default: 
		break;
	}*/
}

//Written by Paul Bichler (D00242563)
//Update method is used by Pulse platforms to change the color every 2 seconds.
void PlatformServer::Update()
{
	if (!m_is_pulse)
		return;

	m_current_pulse_cooldown -= Timing::sInstance.GetDeltaTime();

	if (m_current_pulse_cooldown <= 0)
	{
		//todo send new type
		/*switch (m_type)
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
		}*/

		m_current_pulse_cooldown = 2.f;
	}
}
