// 角色属性集 —— 对应教程第 1 步至第 4.3 节的 C++ 部分。
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
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// 当前生命值只能由属性集结算，GE 的 Modifier 下拉框不会再显示它。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Health", meta = (HideFromModifiers))
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS_BASIC(ULabHealthAttributeSet, Health)

	// 生命上限同样使用 RepNotify，确保客户端血条能响应上限变化。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS_BASIC(ULabHealthAttributeSet, MaxHealth)

	// 伤害元属性：GE/Execution 写入正数伤害，PostGameplayEffectExecute 将其换算为 Health。
	// 它只用于一次结算的中转，不需要复制，并会在消费后立即归零。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS_BASIC(ULabHealthAttributeSet, Damage)

	// WBP_HealthBarWorld 可绑定此事件；Health 或 MaxHealth 变化时都会触发。
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FLabAttributeChangedEvent OnHealthChanged;

	// 技能消耗使用的当前能量。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Energy, Category = "Energy")
	FGameplayAttributeData Energy;
	ATTRIBUTE_ACCESSORS_BASIC(ULabHealthAttributeSet, Energy)

	// 能量上限。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxEnergy, Category = "Energy")
	FGameplayAttributeData MaxEnergy;
	ATTRIBUTE_ACCESSORS_BASIC(ULabHealthAttributeSet, MaxEnergy)

	// Energy 或 MaxEnergy 变化时广播，后续能量条可以按需绑定。
	UPROPERTY(BlueprintAssignable, Category = "Energy")
	FLabAttributeChangedEvent OnEnergyChanged;

protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_Energy(const FGameplayAttributeData& OldEnergy);

	UFUNCTION()
	void OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy);
};
