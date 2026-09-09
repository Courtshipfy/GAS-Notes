// 生命属性集 —— 对应教程第 1 步至第 3.2 节的 C++ 部分。
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "LabHealthAttributeSet.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FLabAttributeChangedEvent,
	UAttributeSet*, AttributeSet,
	float, OldValue,
	float, NewValue);

UCLASS()
class ABILITIESLAB_API ULabHealthAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	ULabHealthAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	// 当前生命值。属性发生网络同步时，OnRep 会负责刷新 GAS 内部状态并广播变化。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS_BASIC(ULabHealthAttributeSet, Health)

	// 生命上限同样使用 RepNotify，确保客户端血条能响应上限变化。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS_BASIC(ULabHealthAttributeSet, MaxHealth)

	// WBP_HealthBarWorld 可绑定此事件；Health 或 MaxHealth 变化时都会触发。
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FLabAttributeChangedEvent OnHealthChanged;

protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
};
