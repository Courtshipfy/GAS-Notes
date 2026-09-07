// 可玩角色 —— 对应教程「第 0 步」：挂 ASC、实现 IAbilitySystemInterface、设置 Owner/Avatar
// 说明：本实验项目不使用第三人称模板内容，视觉用占位方块；移动/镜头为传统输入方案。
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "LabAbilitySystemComponent.h"
#include "AbilitiesLabCharacter.generated.h"

UCLASS()
class ABILITIESLAB_API AAbilitiesLabCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAbilitiesLabCharacter();

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// 联网健壮性：PC 可解析时刷新 ASC 的 ActorInfo（对应教程常见坑的修复）
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// —— GAS（第 0 步）——
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<ULabAbilitySystemComponent> LabAbilitySystemComp;

	// —— 镜头与占位视觉 ——
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> BodyMesh; // 占位方块，后续可换成真实网格

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
};
