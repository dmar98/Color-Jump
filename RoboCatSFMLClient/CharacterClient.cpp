#include "RoboCatClientPCH.hpp"

CharacterClient::CharacterClient(EColorType type, const sf::IntRect& texture_rect)
	: Character(type, texture_rect)
{
	mSpriteComponent.reset(new SpriteComponent(this));
	mSpriteComponent->SetTexture(TextureManager::sInstance->GetTexture("LevelTileSet"));

	/*m_jump_smoke_animation.SetFrameSize(sf::Vector2i(256, 256));
	m_jump_smoke_animation.SetNumFrames(16);
	m_jump_smoke_animation.SetDuration(sf::seconds(.5f));
	m_jump_smoke_animation.setScale(.5f, .5f);

	const sf::FloatRect bounds = m_jump_smoke_animation.GetLocalBounds();
	m_jump_smoke_animation.setOrigin(std::floor(bounds.left + bounds.width / 2.f),
	                                 std::floor(bounds.top + 50.f));*/

	m_name_text->SetFont(*FontManager::sInstance->GetFont("carlito"));
	m_name_text->SetFontSize(15.f);
	m_team_id_text->SetFont(*FontManager::sInstance->GetFont("carlito"));
	m_team_id_text->SetFontSize(15.f);
}

void CharacterClient::InitRay()
{
	CreateRay();
	UpdateRay();
}

void CharacterClient::Update()
{
	Character::Update();
	UpdateRay();

	/*if (m_show_jump_animation)
	{
		m_jump_smoke_animation.Update(dt);
		m_show_jump_animation = !m_jump_smoke_animation.IsFinished();
	}*/
}

void CharacterClient::HandleDying()
{
	Character::HandleDying();
}

void CharacterClient::Read(InputMemoryBitStream& inInputStream)
{
	Character::Read(inInputStream);
}

void CharacterClient::SetTransparent() const
{
	const auto& color = mSpriteComponent->GetSprite().getColor();
	const auto& white = sf::Color(255, 255, 255, 50);
	mSpriteComponent->GetSprite().setColor(sf::Color(color.r, color.g, color.b, 50));
	m_name_text->SetColor(white);
	m_team_id_text->SetColor(white);
}

void CharacterClient::SetOpaque() const
{
	const auto& color = mSpriteComponent->GetSprite().getColor();
	const auto& white = sf::Color(255, 255, 255, 255);
	mSpriteComponent->GetSprite().setColor(sf::Color(color.r, color.g, color.b, 255));
	m_name_text->SetColor(white);
	m_team_id_text->SetColor(white);
}

void CharacterClient::SetTeamIdentifier(sf::Int8 identifier)
{
	Character::SetTeamIdentifier(identifier);

	m_team_id_text->SetText(std::to_string(identifier));
	const sf::FloatRect bounds = m_team_id_text->GetText().getLocalBounds();
	m_team_id_text->GetText().setOrigin(std::floor(bounds.left + bounds.width / 2.f), mSpriteComponent->GetSprite().getOrigin().y - 5.0f);
}

void CharacterClient::SetName(const std::string& name)
{
	Character::SetName(name);

	m_name_text->SetText(name);
	const auto sprite = mSpriteComponent->GetSprite();
	const sf::FloatRect bounds = m_name_text->GetText().getLocalBounds();
	m_name_text->GetText().setOrigin(std::floor(bounds.left + bounds.width / 2.f),
						  sprite.getOrigin().y + sprite.getTextureRect().height / 2.f - 5.f);
}

sf::IntRect CharacterClient::GetSize() const
{
	return mSpriteComponent->GetSprite().getTextureRect();
}

void CharacterClient::Jump()
{
	Character::Jump();

	if (m_can_jump == false)
	{
		return;
	}

	m_show_jump_animation = true;
	/*m_jump_smoke_animation.Restart();
	m_sounds.Play(SoundEffect::kJump);*/
}

void CharacterClient::SetGrounded(Platform* platform)
{
	Character::SetGrounded(platform);
	m_show_jump_animation = false;
}

void CharacterClient::SetGrounded()
{
	Character::SetGrounded();
	m_show_jump_animation = false;
}

void CharacterClient::MoveOutOfCollision(const sf::Time dt, const sf::FloatRect& rect)
{
	Vector3 velocity = GetVelocity();

	if (velocity.Length() == 0.f)
	{
		velocity = Vector3(0, -9.81f, 0);
	}

	const Vector3 normal_velocity = Utility::UnitVector(velocity);
	SetVelocity(Vector3(0, 9.81f, 0));

	while (rect.intersects(GetBoundingRect()))
	{
		SetLocation(GetLocation() - normal_velocity + Vector3(0, 9.81f * dt.asSeconds(), 0));
	}
}

void CharacterClient::UpdateRay() const
{
	m_ray->SetLocation(GetLocation() + Vector3(0.f, 50.f, 0.f));
}

void CharacterClient::CreateRay()
{
	const std::shared_ptr<RayGround> ray(new RayGround(this));
	m_ray = ray.get();
	World::sInstance->AddGameObject(ray);
}

sf::FloatRect CharacterClient::GetBoundingRect() const
{
	return mSpriteComponent->GetSprite().getGlobalBounds();
}
