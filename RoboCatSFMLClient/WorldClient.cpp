#include "ColorJumpClientPCH.hpp"

WorldClient::WorldClient()
	: World(),
	  m_checkpoint(nullptr)
{
}

void WorldClient::LoadLevel()
{
	LevelManager::sInstance->SetIsMultiplayer(true);
	LevelManager::LevelData level_data = LevelManager::sInstance->GetCurrentLevelData();

	LevelLoaderClient loader{};
	m_level_info = loader.LoadLevel(level_data);
}

void WorldClient::Update()
{
	World::Update();

	CheckClientCollisions();
	DestroyPlayerOutsideView();

	while (!m_command_queue.IsEmpty())
	{
		Command command = m_command_queue.Pop();
		for (auto& go : mGameObjects)
		{
			const unsigned int category = go->GetCategory();
			if (category == command.category)
			{
				command.action(*go);
				break;
			}
		}
	}


	if (m_client_player != nullptr)
	{
		sf::Vector2f camera_pos = m_camera.getCenter();
		camera_pos.x = m_client_player->GetLocation().mX;
		m_camera.setCenter(camera_pos);
	}
}

void WorldClient::CheckClientCollisions() const
{
	if (m_client_player == nullptr)
		return;

	const sf::FloatRect client_bounds = m_client_player->GetBoundingRect();
	RayGround* client_ray = m_client_player->GetRay();
	const sf::FloatRect client_ray_bounds = client_ray->GetBoundingRect();
	std::set<GameObject::GOPair> ray_collision_pairs;
	bool collide = false;

	for (const auto& go : mGameObjects)
	{
		//Ignore yourself and your ray ground object
		if (go.get() == m_client_player || go.get() == client_ray)
			continue;

		sf::FloatRect go_bounds = go->GetBoundingRect();

		//Character Collisions
		if (go_bounds.intersects(client_bounds))
		{
			//Client Character Collision!
			const GameObject::GOPair collision_pair(go.get(), m_client_player);

			if (CollisionHandler::PlatformCollision(collision_pair,
			                                        [this] { OnReachedCheckpoint(); },
			                                        [this] { OnReachedGoal(); }, this))
				continue;

			CollisionHandler::TrapCollision(collision_pair, [this] { OnClientPlayerDeath(); });
		}

		//Ray Ground Collisions
		if (go_bounds.intersects(client_ray_bounds))
		{
			const auto platform_part = dynamic_cast<PlatformPart*>(go.get());
			if (platform_part != nullptr)
			{
				if (CollisionHandler::IsPlayerAtHisPlatform(
					*m_client_player, platform_part->GetPlatform()))
				{
					//Client Ray Collision!
					collide = true;
				}
			}
		}
	}

	if (!collide)
	{
		client_ray->SetFalling();
	}
}

//Checks if the client player goes outside of the camera bounds
void WorldClient::DestroyPlayerOutsideView() const
{
	if (m_client_player == nullptr)
		return;

	if (!m_client_player->IsDead() && m_client_player->GetLocation().mY > m_camera.getCenter().y +
		m_camera.getSize().y / 2)
	{
		OnClientPlayerDeath();
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

Character* WorldClient::AddCharacter(const int player_id, const EColorType color)
{
	Debug("Player " + std::to_string(player_id) + " color is " + std::to_string(
		static_cast<int>(color)));

	std::shared_ptr<CharacterClient> player;
	if (color == kRed)
	{
		player.reset(new CharacterClient(color, m_level_info.m_red_player_rect));
		player->SetLocation(m_level_info.m_red_player_spawn_pos);
	}
	else
	{
		player.reset(new CharacterClient(color, m_level_info.m_blue_player_rect));
		player->SetLocation(m_level_info.m_blue_player_spawn_pos);
	}

	player->InitRay();
	player->SetIdentifier(player_id);

	m_players.emplace_back(player.get());
	AddGameObject(player);

	Character* character = m_players.back();
	m_client_player = dynamic_cast<CharacterClient*>(character);
	UpdateCharacterTransparencies(character->GetTeamIdentifier());
	return character;
}

Character* WorldClient::AddGhostCharacter(const int player_id, const EColorType color)
{
	std::shared_ptr<GhostCharacterClient> ghost_char;
	if (color == kRed)
	{
		ghost_char.reset(new GhostCharacterClient(color, m_level_info.m_red_player_rect));
		ghost_char->SetLocation(m_level_info.m_red_player_spawn_pos);
	}
	else
	{
		ghost_char.reset(new GhostCharacterClient(color, m_level_info.m_blue_player_rect));
		ghost_char->SetLocation(m_level_info.m_blue_player_spawn_pos);
	}


	ghost_char->SetIdentifier(player_id);

	m_players.emplace_back(ghost_char.get());
	AddGameObject(ghost_char);
	return m_players.back();
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

void WorldClient::UpdateCharacterTransparencies(const int team_id) const
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
	position.mY -= static_cast<float>(m_client_player->GetSize().height - 15);
	position.mX += static_cast<float>(part->GetSize().width) / 2.f;
	m_client_player->SetLocation(position);
	m_client_player->SetIsDead(false);

	//Reset all platforms to their initial type
	for (auto& platform : m_level_info.platforms)
	{
		platform->ResetToInitialType();
	}
}

void WorldClient::SetCheckpointToPlatformWithID(const int platform_id)
{
	Platform* new_checkpoint = m_level_info.GetPlatformWithID(platform_id);

	if (new_checkpoint != nullptr)
	{
		m_checkpoint = new_checkpoint;
		m_checkpoint->SetType(kCheckpointActivated);
		AudioManager::sInstance->Play("Checkpoint");
	}
}

Character* WorldClient::GetCharacter(const int player_id) const
{
	for (Character* character : m_players)
	{
		if (character->GetPlayerID() == player_id)
		{
			return character;
		}
	}
	return nullptr;
}

void WorldClient::SetPlatformOnCharacter(Character* character, const int platform_id) const
{
	for (const auto& platform : m_level_info.platforms)
	{
		if (platform->GetID() == platform_id)
		{
			character->SetGrounded(platform.get());
			break;
		}
	}
}

void WorldClient::UpdatePlatform(const int id, const int platform_id,
                                 const EPlatformType platform_type)
{
	for (auto& platform : m_level_info.platforms)
	{
		if (platform->GetID() == platform_id)
		{
			if (platform_type == kCheckpointActivated)
			{
				if (GetClientCharacter()->GetPlayerID() == id)
				{
					platform->SetType(platform_type);
				}
			}
			else
			{
				platform->SetType(platform_type);
			}

			//Initialize the first checkpoint (spawn platform)
			if (m_checkpoint == nullptr)
				m_checkpoint = platform.get();
		}
	}
}

void WorldClient::OnReachedCheckpoint() const
{
	Platform* current_platform = m_client_player->GetCurrentPlatform();

	if (m_team_mate != nullptr && current_platform == m_team_mate->GetCurrentPlatform() &&
		current_platform != m_checkpoint)
	{
		NetworkManagerClient::sInstance->SetState(
		   NetworkManagerClient::NetworkClientState::NCS_Checkpoint_Reached);
	}
}

void WorldClient::OnReachedGoal()
{
	NetworkManagerClient::sInstance->SetState(
		NetworkManagerClient::NetworkClientState::NCS_Goal_Reached);
}

void WorldClient::OnClientPlayerDeath() const
{
	if (!m_client_player->IsDead())
	{
		m_client_player->SetIsDead(true);
		NetworkManagerClient::sInstance->SetState(
			NetworkManagerClient::NetworkClientState::NCS_Team_Death);
		AudioManager::sInstance->Play("Death");
	}
}


CommandQueue& WorldClient::GetCommandQueue()
{
	return m_command_queue;
}

void WorldClient::Debug(const std::string& message) const
{
	const string in_format = "World Client: " + message;
	LOG(in_format.c_str(), 0)
}

void WorldClient::RemoveCharacter(const int player_id)
{
	Character* character = GetCharacter(player_id);

	if (character)
	{
		shared_ptr<GameObject> toDelete;
		for (auto& go : mGameObjects)
		{
			if (go.get() == character)
			{
				toDelete = go;
			}
		}

		mGameObjects.erase(std::find(mGameObjects.begin(), mGameObjects.end(), toDelete));
		m_players.erase(std::find(m_players.begin(), m_players.end(), character));
	}
}
