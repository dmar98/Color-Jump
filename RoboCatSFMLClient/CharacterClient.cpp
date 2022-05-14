#include "ColorJumpClientPCH.hpp"

CharacterClient::CharacterClient(EColorType type, const sf::IntRect& texture_rect)
	: Character(type, texture_rect)
{
	mSpriteComponent.reset(new SpriteComponent(this));
	mSpriteComponent->SetTexture(TextureManager::sInstance->GetTexture("LevelTileSet"),
	                             texture_rect, true);

	m_name_text.reset(new TextComponent(this));
	m_name_text->SetFont(*FontManager::sInstance->GetFont("carlito"));
	m_name_text->SetFontSize(15.f);
	m_team_id_text.reset(new TextComponent(this));
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

unsigned CharacterClient::GetCategory() const
{
	return Category::kPlayer;
}

void CharacterClient::SetTeamIdentifier(const int identifier)
{
	Character::SetTeamIdentifier(identifier);

	m_team_id_text->SetText(std::to_string(identifier));
	const auto& sprite = mSpriteComponent->GetSprite();

	m_team_id_text->SetOffset(sf::Vector2f(0, -sprite.getLocalBounds().height / 4));
}

void CharacterClient::SetName(const std::string& name)
{
	Character::SetName(name);

	m_name_text->SetText(name);
	const auto& sprite = mSpriteComponent->GetSprite();
	m_name_text->SetOffset(sf::Vector2f(0, -sprite.getLocalBounds().height / 2 - 10));
}

sf::IntRect CharacterClient::GetSize() const
{
	return mSpriteComponent->GetSprite().getTextureRect();
}

void CharacterClient::Jump()
{
	if (m_can_jump == false)
	{
		return;
	}

	Character::Jump();
	AudioManager::sInstance->Play("Jump");
}

void CharacterClient::SetGrounded(Platform* platform)
{
	Character::SetGrounded(platform);
}

void CharacterClient::MoveOutOfCollision(const sf::FloatRect& rect)
{
	Vector3 velocity = GetVelocity();

	if (velocity.Length2D() == 0.f)
	{
		velocity = Vector3(0, -9.81f, 0);
	}

	Vector3 normal_velocity = velocity;
	normal_velocity.Normalize2D();
	normal_velocity.mY = m_grounded ? normal_velocity.mY : 0;
	const Vector3 down_force(0, 9.81f, 0);

	while (rect.intersects(GetBoundingRect()))
	{
		this->SetLocation(GetLocation() - normal_velocity + down_force * Timing::sInstance.GetDeltaTime());
		mSpriteComponent->UpdatePosition();
	}
}

RayGround* CharacterClient::GetRay() const
{
	return m_ray;
}

bool CharacterClient::IsGrounded() const
{
	return m_grounded && m_current_platform != nullptr;
}

void CharacterClient::UpdateRay() const
{
	m_ray->SetLocation(GetLocation() + Vector3(0.f, 25.f, 0.f));
}

void CharacterClient::CreateRay()
{
	const std::shared_ptr<RayGround> ray(new RayGround(this));
	m_ray = ray.get();
	World::sInstance->AddGameObject(ray);

	RenderManager::sInstance->SetRay(ray.get());
}

sf::FloatRect CharacterClient::GetBoundingRect() const
{
	return mSpriteComponent->getTransform().transformRect(
		mSpriteComponent->GetSprite().getLocalBounds());
}
