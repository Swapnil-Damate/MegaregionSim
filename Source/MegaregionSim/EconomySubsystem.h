#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SQLiteDatabase.h"
#include "SQLitePreparedStatement.h"
#include "EconomySubsystem.generated.h"

UCLASS()
class MEGAREGIONSIM_API UEconomySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Economy")
	int32 GetPlayerBalance();

	UFUNCTION(BlueprintCallable, Category = "Economy")
	void AddFunds(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Economy")
	void GenerateRandomContract();

	UFUNCTION(BlueprintCallable, Category = "Economy")
	FString GetActiveContractDetails();

	UFUNCTION(BlueprintCallable, Category = "Economy")
	void CompleteContract();
	
	UFUNCTION(BlueprintCallable, Category = "Economy")
	void BuyUpgrade(FString UpgradeType);
	
	UFUNCTION(BlueprintCallable, Category = "Economy")
	float GetContractTimeRemaining() const;

	UFUNCTION(BlueprintCallable, Category = "Maintenance")
	void WashTrain(float& TrainDirtiness, int32 Cost);

	UPROPERTY(BlueprintReadWrite, Category = "Legacy Campaign")
	int32 CurrentReputation = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Legacy Campaign")
	int32 DepotLevel = 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "Upgrades")
	int32 SpeedUpgradeLevel = 0;
	
	UPROPERTY(BlueprintReadWrite, Category = "Upgrades")
	int32 BrakeUpgradeLevel = 0;

private:
	FSQLiteDatabase EconomyDB;
	
	FString ActiveContract;
	FString ActiveCargo;

	FTimerHandle ContractTimerHandle;
	float ContractStartTime;
	float ContractDuration;
	
	void OnContractExpired();

	int32 CoalDeliveryCount = 0;

	void InitializeDatabase();
};
