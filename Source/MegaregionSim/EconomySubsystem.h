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

private:
	FSQLiteDatabase EconomyDB;
	
	FString ActiveContract;

	void InitializeDatabase();
};
