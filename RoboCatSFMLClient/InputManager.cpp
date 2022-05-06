#include "RoboCatClientPCH.hpp"

unique_ptr<InputManager> InputManager::sInstance;

namespace
{
	float kTimeBetweenInputSamples = 0.03f;
}

void InputManager::StaticInit()
{
	sInstance.reset(new InputManager());
}


namespace
{
	void UpdateDesireVariableFromKey(EInputAction inInputAction, bool& ioVariable)
	{
		if (inInputAction == EIA_Pressed)
		{
			ioVariable = true;
		}
		else if (inInputAction == EIA_Released)
		{
			ioVariable = false;
		}
	}

	void UpdateDesireFloatFromKey(EInputAction inInputAction, float& ioVariable)
	{
		if (inInputAction == EIA_Pressed)
		{
			ioVariable = 1.f;
		}
		else if (inInputAction == EIA_Released)
		{
			ioVariable = 0.f;
		}
	}
}

void InputManager::HandleInput(EInputAction inInputAction, int inKeyCode)
{
	switch (inKeyCode)
	{
	case sf::Keyboard::A:
		UpdateDesireFloatFromKey(inInputAction, mCurrentState.mDesiredLeftAmount);
		break;
	case sf::Keyboard::D:
		UpdateDesireFloatFromKey(inInputAction, mCurrentState.mDesiredRightAmount);
		break;
	case sf::Keyboard::W:
		UpdateDesireFloatFromKey(inInputAction, mCurrentState.mDesiredForwardAmount);
		break;
	case sf::Keyboard::S:
		UpdateDesireFloatFromKey(inInputAction, mCurrentState.mDesiredBackAmount);
		break;
	case sf::Keyboard::K:
		UpdateDesireVariableFromKey(inInputAction, mCurrentState.mIsShooting);
		break;
	case sf::Keyboard::Add:
	case sf::Keyboard::Equal:
		{
			float latency = NetworkManagerClient::sInstance->GetSimulatedLatency();
			latency += 0.1f;
			if (latency > 0.5f)
			{
				latency = 0.5f;
			}
			NetworkManagerClient::sInstance->SetSimulatedLatency(latency);
			break;
		}
	case sf::Keyboard::Subtract:
		{
			float latency = NetworkManagerClient::sInstance->GetSimulatedLatency();
			latency -= 0.1f;
			if (latency < 0.0f)
			{
				latency = 0.0f;
			}
			NetworkManagerClient::sInstance->SetSimulatedLatency(latency);
			break;
		}
	}
}


InputManager::InputManager() :
	mNextTimeToSampleInput(0.f),
	mPendingMove(nullptr)
{
}

const Move& InputManager::SampleInputAsMove()
{
	return mMoveList.AddMove(GetState(), Timing::sInstance.GetFrameStartTime());
}

bool InputManager::IsTimeToSampleInput()
{
	float time = Timing::sInstance.GetFrameStartTime();
	if (time > mNextTimeToSampleInput)
	{
		mNextTimeToSampleInput = mNextTimeToSampleInput + kTimeBetweenInputSamples;
		return true;
	}

	return false;
}

void InputManager::Update()
{
	if (IsTimeToSampleInput())
	{
		mPendingMove = &SampleInputAsMove();
	}
}
