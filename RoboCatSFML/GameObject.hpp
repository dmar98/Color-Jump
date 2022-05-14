#pragma once
#define CLASS_IDENTIFICATION(in_code, in_class) \
enum {kClassId = (in_code)}; \
virtual uint32_t GetClassId() const {return kClassId;} \
static GameObject* CreateInstance() { return static_cast<GameObject*>(new in_class());}

class GameObject
{
public:
	typedef std::pair<GameObject*, GameObject*> GOPair;
	CLASS_IDENTIFICATION('GOBJ', GameObject)

	GameObject();
	virtual ~GameObject() = default;

	virtual uint32_t GetAllStateMask() const { return 0; }

	virtual void Update();

	virtual void HandleDying()
	{
	}

	void SetIndexInWorld(const int inIndex) { mIndexInWorld = inIndex; }
	int GetIndexInWorld() const { return mIndexInWorld; }

	void SetRotation(float inRotation);
	float GetRotation() const { return mRotation; }

	void SetScale(const float inScale) { mScale = inScale; }
	float GetScale() const { return mScale; }


	const Vector3& GetLocation() const { return mLocation; }
	void SetLocation(const Vector3& inLocation) { mLocation = inLocation; }

	Vector3 GetForwardVector() const;

	bool DoesWantToDie() const { return mDoesWantToDie; }
	void SetDoesWantToDie(const bool inWants) { mDoesWantToDie = inWants; }

	int GetNetworkId() const { return mNetworkId; }
	void SetNetworkId(int inNetworkId);

	virtual unsigned int GetCategory() const;
	virtual sf::FloatRect GetBoundingRect() const;
private:
	Vector3 mLocation;

	float mRotation;
	float mScale;
	int mIndexInWorld;

	bool mDoesWantToDie;
	int mNetworkId;
};

using GameObjectPtr = shared_ptr<GameObject>;
