#include "LabHealthAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

ULabHealthAttributeSet::ULabHealthAttributeSet()
{
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitDamage(0.0f);
	InitEnergy(100.0f);
	InitMaxEnergy(100.0f);
}

void ULabHealthAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Always 通知可让客户端预测值在服务器回传相同数值时也正确对账。
	DOREPLIFETIME_CONDITION_NOTIFY(ULabHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULabHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULabHealthAttributeSet, Energy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULabHealthAttributeSet, MaxEnergy, COND_None, REPNOTIFY_Always);
}

void ULabHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxEnergy());
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
	else if (Attribute == GetEnergyAttribute() || Attribute == GetMaxEnergyAttribute())
	{
		OnEnergyChanged.Broadcast(this, OldValue, NewValue);
	}
}

void ULabHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Damage 是一次性中转值；先读取并归零，避免它在后续效果中累积。
		const float DamageValue = GetDamage();
		SetDamage(0.0f);

		if (DamageValue > 0.0f)
		{
			const float NewHealthValue = FMath::Clamp(
				GetHealth() - DamageValue,
				0.0f,
				GetMaxHealth());
			SetHealth(NewHealthValue);
		}
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

void ULabHealthAttributeSet::OnRep_Energy(const FGameplayAttributeData& OldEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULabHealthAttributeSet, Energy, OldEnergy);
	OnEnergyChanged.Broadcast(this, OldEnergy.GetCurrentValue(), GetEnergy());
}

void ULabHealthAttributeSet::OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULabHealthAttributeSet, MaxEnergy, OldMaxEnergy);
	OnEnergyChanged.Broadcast(this, OldMaxEnergy.GetCurrentValue(), GetMaxEnergy());
}
