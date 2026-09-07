// 游戏模式：把默认出生 Pawn 设为本项目的 GAS 角色
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AbilitiesLabGameMode.generated.h"

UCLASS()
class ABILITIESLAB_API AAbilitiesLabGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAbilitiesLabGameMode();
};
