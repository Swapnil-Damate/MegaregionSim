#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CockpitInteractable.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MEGAREGIONSIM_API UCockpitInteractable : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCockpitInteractable();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact();
};
