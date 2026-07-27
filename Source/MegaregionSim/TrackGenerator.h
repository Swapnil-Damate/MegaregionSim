#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrackGenerator.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class MEGAREGIONSIM_API ATrackGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	ATrackGenerator();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rails")
	UBoxComponent* LeftRail;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rails")
	UBoxComponent* RightRail;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rails")
	UBoxComponent* CrossTies;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rails")
	class UStaticMeshComponent* BaseMesh;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rails")
	class UStaticMeshComponent* LeftRailMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rails")
	class UStaticMeshComponent* RightRailMesh;
};
