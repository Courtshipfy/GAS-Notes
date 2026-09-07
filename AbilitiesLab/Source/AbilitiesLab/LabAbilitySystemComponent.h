// 能力系统组件（ASC）子类 —— 对应教程「第 0 步：创建 AbilitySystemComponent 子类」
// 提前派生：后续要覆写虚函数 / 加项目专用辅助函数时不用再改挂载点
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "LabAbilitySystemComponent.generated.h"

UCLASS()
class ABILITIESLAB_API ULabAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
};
