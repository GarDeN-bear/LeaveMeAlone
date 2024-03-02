#include "Enemy/LMAAIController.h"
#include "Enemy/LMAEnemyCharacter.h"
#include "Enemy/LMAEnemyStrongCharacter.h"

void ALMAAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	const auto AICharacter = Cast<ALMAEnemyCharacter>(InPawn);
	if (AICharacter)
	{
		RunBehaviorTree(AICharacter->BehaviorTreeAsset);
	}
}