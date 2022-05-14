#pragma once
enum class ReplicationAction
{
	RA_Create,
	RA_Update,
	RA_Destroy,
	RA_RPC,
	RA_MAX
};

class ReplicationManagerTransmissionData;


struct ReplicationCommand
{
	ReplicationCommand(): mAction(ReplicationAction::RA_Create)
	{
	}

	explicit ReplicationCommand(const uint32_t inInitialDirtyState) :
		mDirtyState(inInitialDirtyState),
		mAction(ReplicationAction::RA_Create)
	{
	}

	//if the create is ack'd, we can demote to just an update...
	void HandleCreateAckd()
	{
		if (mAction == ReplicationAction::RA_Create) { mAction = ReplicationAction::RA_Update; }
	}

	void AddDirtyState(const uint32_t inState) { mDirtyState |= inState; }
	void SetDestroy() { mAction = ReplicationAction::RA_Destroy; }

	bool HasDirtyState() const
	{
		return (mAction == ReplicationAction::RA_Destroy) || (mDirtyState != 0);
	}

	void SetAction(const ReplicationAction inAction) { mAction = inAction; }
	ReplicationAction GetAction() const { return mAction; }
	uint32_t GetDirtyState() const { return mDirtyState; }
	inline void ClearDirtyState(uint32_t inStateToClear);

private:
	uint32_t mDirtyState{};
	ReplicationAction mAction;
};

inline void ReplicationCommand::ClearDirtyState(const uint32_t inStateToClear)
{
	mDirtyState &= ~inStateToClear;

	if (mAction == ReplicationAction::RA_Destroy)
	{
		mAction = ReplicationAction::RA_Update;
	}
}
