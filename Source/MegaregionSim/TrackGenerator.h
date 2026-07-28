#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "TrackGenerator.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class MEGAREGIONSIM_API ATrackGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	ATrackGenerator();

	virtual void BeginPlay() override;

	void GenerateTrack();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rails")
	UInstancedStaticMeshComponent* CrossTiesISM;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rails")
	UInstancedStaticMeshComponent* LeftRailISM;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rails")
	UInstancedStaticMeshComponent* RightRailISM;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rails")
	UBoxComponent* TrackCollision;
};
