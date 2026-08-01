#include "CockpitInteractable.h"
#include "Engine/Engine.h"

UCockpitInteractable::UCockpitInteractable()
{
	// PrimaryComponentTick.bCanEverTick = true;
}

void UCockpitInteractable::BeginPlay()
{
	Super::BeginPlay();
}

void UCockpitInteractable::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCockpitInteractable::Interact()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Button Interacted!"));
	}
}
