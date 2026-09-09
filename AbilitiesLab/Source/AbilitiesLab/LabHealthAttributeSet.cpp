#include "LabHealthAttributeSet.h"

#include "Net/UnrealNetwork.h"

ULabHealthAttributeSet::ULabHealthAttributeSet()
{
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
}

void ULabHealthAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Always 通知可让客户端预测值在服务器回传相同数值时也正确对账。
	DOREPLIFETIME_CONDITION_NOTIFY(ULabHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULabHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void ULabHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}

void ULabHealthAttributeSet::PostAttributeChange(
	const FGameplayAttribute& Attribute,
	float OldValue,
	float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetHealthAttribute() || Attribute == GetMaxHealthAttribute())
	{
		OnHealthChanged.Broadcast(this, OldValue, NewValue);
	}
}

void ULabHealthAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULabHealthAttributeSet, Health, OldHealth);
	OnHealthChanged.Broadcast(this, OldHealth.GetCurrentValue(), GetHealth());
}

void ULabHealthAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULabHealthAttributeSet, MaxHealth, OldMaxHealth);
	OnHealthChanged.Broadcast(this, OldMaxHealth.GetCurrentValue(), GetMaxHealth());
}
