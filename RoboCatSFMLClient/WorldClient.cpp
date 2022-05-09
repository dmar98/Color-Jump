#include "RoboCatClientPCH.hpp"

#include "LevelLoaderClient.hpp"

WorldClient::WorldClient()
	: World(),
	  m_checkpoint(nullptr),
	  m_client_player(nullptr),
	  m_team_mate(nullptr)
{
}

void WorldClient::LoadLevel()
{
	LevelManager::sInstance->SetIsMultiplayer(true);
	LevelManager::LevelData level_data = LevelManager::sInstance->GetCurrentLevelData();

	LevelLoaderClient loader {};
	m_level_info = loader.LoadLevel(level_data);
}

void WorldClient::Update()
{
	World::Update();

	CheckClientCollisions();
}

void WorldClient::CheckClientCollisions() const
{
	if(m_client_player == nullptr)
		return;

	const sf::FloatRect client_bounds = m_client_player->GetBoundingRect();
	RayGround* client_ray = m_client_player->GetRay();
	const sf::FloatRect client_ray_bounds = client_ray->GetBoundingRect();
	std::set<GameObject::GOPair> ray_collision_pairs;

	for(auto go : mGameObjects)
	{
		//Ignore yourself and your ray ground object
		if(go.get() == m_client_player || go.get() == client_ray)
			continue;

		sf::FloatRect go_bounds = go->GetBoundingRect();

		//Character Collisions
		if(go_bounds.intersects(client_bounds))
		{
			//Client Character Collision!
			const GameObject::GOPair collision_pair(go.get(), m_client_player);

			if (CollisionHandler::PlatformCollision(collision_pair, [this] { OnReachedCheckpoint(); }, [this] { OnReachedGoal(); }, this))
				continue;

			CollisionHandler::TrapCollision(collision_pair, [this] { OnClientPlayerDeath(); });
		}

		//Ray Ground Collisions
		if(go_bounds.intersects(client_ray_bounds))
		{
			//Client Ray Collision!
			GameObject::GOPair collision_pair(go.get(), client_ray);
			CollisionHandler::PlayerGroundRayCast(collision_pair);
			
		}
	}
}

void WorldClient::SetCamera(sf::View camera)
{
	SetWorldBounds(sf::FloatRect(0.f, 0.f, camera.getSize().x, camera.getSize().y));
	camera.setCenter(camera.getSize().x / 2.f, m_world_bounds.height - camera.getSize().y / 2.f);
	m_camera = camera;
}

sf::View& WorldClient::GetCamera()
{
	return m_camera;
}

Character* WorldClient::AddCharacterWithColor(sf::Int8 identifier, EColorType color, sf::IntRect rect, Vector3 spawn_pos)
{
	const std::shared_ptr<CharacterClient> player(new CharacterClient(color, rect));
	player->InitRay();
	player->SetLocation(spawn_pos);
	player->SetIdentifier(identifier);

	m_players.emplace_back(player.get());
	AddGameObject(player);

	return player.get();
}

Character* WorldClient::AddCharacter(sf::Int8 identifier, sf::Int8 color, bool is_client_player)
{
	Character* character;

	if (color == 1)
	{
		character = AddCharacterWithColor(identifier, EColorType::kRed, m_level_info.m_red_player_rect,
			m_level_info.m_red_player_spawn_pos);
	}
	else
	{
		character = AddCharacterWithColor(identifier, EColorType::kBlue, m_level_info.m_blue_player_rect,
			m_level_info.m_blue_player_spawn_pos);
	}

	if (is_client_player)
	{
		m_client_player = dynamic_cast<CharacterClient*>(character);
		UpdateCharacterTransparencies(character->GetTeamIdentifier());
	}

	return character;
}

Character* WorldClient::AddGhostCharacterWithColor(sf::Int8 identifier, EColorType color, const sf::IntRect& int_rect,
	const Vector3 spawn_pos)
{
	const std::shared_ptr<GhostCharacterClient> ghost_char(new GhostCharacterClient(color, int_rect));
	ghost_char->SetLocation(spawn_pos);
	ghost_char->SetIdentifier(identifier);

	m_players.emplace_back(ghost_char.get());
	AddGameObject(ghost_char);
	return ghost_char.get();
}

Character* WorldClient::AddGhostCharacter(sf::Int8 identifier, sf::Int8 color)
{
	if (color == 1)
	{
		return AddGhostCharacterWithColor(identifier, EColorType::kRed,
			m_level_info.m_red_player_rect,
			m_level_info.m_red_player_spawn_pos);
	}

	return AddGhostCharacterWithColor(identifier, EColorType::kBlue,
		m_level_info.m_blue_player_rect,
		m_level_info.m_blue_player_spawn_pos);
}

Character* WorldClient::GetClientCharacter() const
{
	return m_client_player;
}

void WorldClient::SetTeammate(Character* character)
{
	m_team_mate = character;
}

Character* WorldClient::GetTeammate() const
{
	return m_team_mate;
}

void WorldClient::UpdateCharacterTransparencies(sf::Int8 team_id) const
{
	for (Character* player : m_players)
	{
		const auto client_char = dynamic_cast<CharacterClient*>(player);
		if (client_char->GetTeamIdentifier() != team_id)
		{
			client_char->SetTransparent();
		}
		else
		{
			client_char->SetOpaque();
		}
	}
}

void WorldClient::UpdateCharacterTransparencies() const
{
	if (GetClientCharacter() != nullptr)
	{
		UpdateCharacterTransparencies(GetClientCharacter()->GetTeamIdentifier());
	}
}

void WorldClient::RespawnClientCharacter() const
{
	//Reset the position of the player to the position of the last checkpoint
	m_client_player->StopMovement();
	const auto& parts = m_checkpoint->GetParts();
	const PlatformPart* part = parts[parts.size() / 2];
	Vector3 position = part->GetLocation();
	position.mY -= static_cast<CharacterClient*>(m_client_player)->GetSize().height - 15.f;
	position.mX += part->GetSize().width / 2;
	m_client_player->SetLocation(position);

	//Reset all platforms to their initial type
	for (auto& platform : m_level_info.platforms)
	{
		platform->ResetToInitialType();
	}
}

void WorldClient::SetCheckpointToPlatformWithID(sf::Int8 platform_id)
{
	Platform* new_checkpoint = m_level_info.GetPlatformWithID(platform_id);

	if (new_checkpoint != nullptr)
	{
		m_checkpoint = new_checkpoint;
		m_checkpoint->SetType(EPlatformType::kCheckpointActivated);
	}
}

void WorldClient::OnReachedCheckpoint() const
{
	Platform* current_platform = m_client_player->GetCurrentPlatform();

	if (m_team_mate != nullptr && current_platform == m_team_mate->GetCurrentPlatform() && current_platform != m_checkpoint)
		NetworkManagerClient::sInstance->SendCheckpointReached(m_client_player->GetTeamIdentifier(), current_platform->GetID());
}

void WorldClient::OnReachedGoal() const
{
	NetworkManagerClient::sInstance->SendGoalReached(GetClientCharacter()->GetTeamIdentifier());
}

void WorldClient::OnClientPlayerDeath() const
{
	NetworkManagerClient::sInstance->SendTeamDeath(m_client_player->GetTeamIdentifier());
}
