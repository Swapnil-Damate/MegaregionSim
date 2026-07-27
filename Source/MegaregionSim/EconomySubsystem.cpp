#include "EconomySubsystem.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

void UEconomySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	InitializeDatabase();
}

void UEconomySubsystem::Deinitialize()
{
	if (EconomyDB.IsValid())
	{
		EconomyDB.Close();
	}
	Super::Deinitialize();
}

void UEconomySubsystem::InitializeDatabase()
{
	FString DbPath = FPaths::ProjectSavedDir() / TEXT("MegaregionDB.sqlite");
	
	// Create Saved directory if it doesn't exist
	IFileManager::Get().MakeDirectory(*FPaths::ProjectSavedDir(), true);

	if (EconomyDB.Open(*DbPath, ESQLiteDatabaseOpenMode::ReadWriteCreate))
	{
		// Create tables if they don't exist
		FString CreateTableQuery = TEXT("CREATE TABLE IF NOT EXISTS PlayerWallet (Id INTEGER PRIMARY KEY, Balance INTEGER);");
		EconomyDB.Execute(*CreateTableQuery);

		// Initialize wallet if empty
		FString CheckWalletQuery = TEXT("SELECT count(*) FROM PlayerWallet;");
		FSQLitePreparedStatement CheckStmt;
		if (CheckStmt.Create(EconomyDB, *CheckWalletQuery))
		{
			if (CheckStmt.Step() != ESQLitePreparedStatementStepResult::Row)
			{
				FString InsertQuery = TEXT("INSERT INTO PlayerWallet (Id, Balance) VALUES (1, 10000);"); // Start with $10k
				EconomyDB.Execute(*InsertQuery);
			}
			CheckStmt.Destroy();
		}
	}
}

int32 UEconomySubsystem::GetPlayerBalance()
{
	int32 Balance = 0;
	if (EconomyDB.IsValid())
	{
		FString Query = TEXT("SELECT Balance FROM PlayerWallet WHERE Id=1;");
		FSQLitePreparedStatement Stmt;
		if (Stmt.Create(EconomyDB, *Query))
		{
			if (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
			{
				Stmt.GetColumnValueByIndex(0, Balance);
			}
			Stmt.Destroy();
		}
	}
	return Balance;
}

void UEconomySubsystem::AddFunds(int32 Amount)
{
	int32 Current = GetPlayerBalance();
	int32 NewBalance = Current + Amount;

	if (EconomyDB.IsValid())
	{
		FString UpdateQuery = FString::Printf(TEXT("UPDATE PlayerWallet SET Balance=%d WHERE Id=1;"), NewBalance);
		EconomyDB.Execute(*UpdateQuery);
	}
}
