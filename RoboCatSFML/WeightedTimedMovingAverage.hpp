#pragma once
class WeightedTimedMovingAverage
{
public:
	explicit WeightedTimedMovingAverage(const float inDuration = 5.f) :
		mTimeLastEntryMade(Timing::sInstance.GetTimef()), mValue(0.f),
		mDuration(inDuration)
	{
	}

	void UpdatePerSecond(const float inValue)
	{
		const float time = Timing::sInstance.GetTimef();
		const float timeSinceLastEntry = time - mTimeLastEntryMade;

		const float valueOverTime = inValue / timeSinceLastEntry;

		//now update our value by whatever amount of the duration that was..
		float fractionOfDuration = (timeSinceLastEntry / mDuration);
		if (fractionOfDuration > 1.f) { fractionOfDuration = 1.f; }

		mValue = mValue * (1.f - fractionOfDuration) + valueOverTime * fractionOfDuration;

		mTimeLastEntryMade = time;
	}

	void Update(const float inValue)
	{
		const float time = Timing::sInstance.GetTimef();
		const float timeSinceLastEntry = time - mTimeLastEntryMade;

		//now update our value by whatever amount of the duration that was..
		float fractionOfDuration = (timeSinceLastEntry / mDuration);
		if (fractionOfDuration > 1.f) { fractionOfDuration = 1.f; }

		mValue = mValue * (1.f - fractionOfDuration) + inValue * fractionOfDuration;

		mTimeLastEntryMade = time;
	}

	float GetValue() const { return mValue; }

private:
	float mTimeLastEntryMade;
	float mValue;
	float mDuration;
};
