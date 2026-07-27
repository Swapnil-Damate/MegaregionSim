#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ContractManager.generated.h"

UCLASS()
class MEGAREGIONSIM_API UContractManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Generates a new driving contract
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void GenerateNewContract();

	// Finishes current contract and awards money
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void CompleteContract(float CargoIntegrityPercent);

	UFUNCTION(BlueprintCallable, Category = "Economy")
	FString GetActiveContractString() const;

	UPROPERTY(BlueprintReadOnly, Category = "Economy")
	int32 PlayerBalance;

private:
	FString ActiveContractName;
	int32 ActiveContractReward;
	bool bHasActiveContract;
};
