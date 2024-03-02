#include "Enemy/LMAEnemyCharacter.h"
#include "Components/LMAHealthComponent.h"

ALMAEnemyCharacter::ALMAEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");
}

void ALMAEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
