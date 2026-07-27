#include "ContractManager.h"
#include "Math/UnrealMathUtility.h"

void UContractManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	PlayerBalance = 5000; // Starting capital
	bHasActiveContract = false;
}

void UContractManager::GenerateNewContract()
{
	int32 RandomType = FMath::RandRange(0, 2);
	if (RandomType == 0)
	{
		ActiveContractName = TEXT("Haul 500 Tons of Coal to the Valley");
		ActiveContractReward = 2500;
	}
	else if (RandomType == 1)
	{
		ActiveContractName = TEXT("Morning Metro Commute - Rush Hour");
		ActiveContractReward = 1500;
	}
	else
	{
		ActiveContractName = TEXT("High Speed Express - Maintain 300km/h");
		ActiveContractReward = 5000;
	}
	
	bHasActiveContract = true;
}

void UContractManager::CompleteContract(float CargoIntegrityPercent)
{
	if (!bHasActiveContract) return;
	
	// Reward is scaled based on how carefully they drove (e.g. didn't spill cargo or injure passengers)
	float FinalReward = ActiveContractReward * (CargoIntegrityPercent / 100.0f);
	PlayerBalance += FMath::RoundToInt(FinalReward);
	
	bHasActiveContract = false;
	ActiveContractName = TEXT("None");
}

FString UContractManager::GetActiveContractString() const
{
	if (bHasActiveContract)
	{
		return FString::Printf(TEXT("%s - Reward: $%d"), *ActiveContractName, ActiveContractReward);
	}
	return TEXT("No Active Contract");
}
