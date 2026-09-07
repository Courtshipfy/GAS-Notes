#include "AbilitiesLabCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AAbilitiesLabCharacter::AAbilitiesLabCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// ===== GAS：挂能力系统组件（第 0 步）=====
	LabAbilitySystemComp = CreateDefaultSubobject<ULabAbilitySystemComponent>(TEXT("AbilitySystem"));

	// ===== 占位可见体（无模板网格时的简易替身）=====
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetCapsuleComponent());
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMeshAsset.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeMeshAsset.Object);
	}
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
	BodyMesh->SetRelativeScale3D(FVector(1.0f, 0.8f, 1.8f));

	// ===== 第三人称镜头 =====
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetCapsuleComponent());
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 60.0f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// 移动：面朝移动方向、旋转由控制器驱动
	bUseControllerRotationYaw = false;
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement = true;
		MoveComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	}
}

UAbilitySystemComponent* AAbilitiesLabCharacter::GetAbilitySystemComponent() const
{
	return LabAbilitySystemComp;
}

void AAbilitiesLabCharacter::BeginPlay()
{
	Super::BeginPlay();

	// ===== 第 0 步：设置 Owner 与 Avatar =====
	// 本实验把角色同时作为属主（Owner）与替身（Avatar）
	if (LabAbilitySystemComp)
	{
		LabAbilitySystemComp->InitAbilityActorInfo(this, this);
	}
}

void AAbilitiesLabCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// 多人健壮性：Possess 后刷新 ASC 的 ActorInfo（让 PC 可被解析）
	if (LabAbilitySystemComp)
	{
		LabAbilitySystemComp->RefreshAbilityActorInfo();
	}
}

void AAbilitiesLabCharacter::UnPossessed()
{
	Super::UnPossessed();
	if (LabAbilitySystemComp)
	{
		LabAbilitySystemComp->RefreshAbilityActorInfo();
	}
}

void AAbilitiesLabCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AAbilitiesLabCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAbilitiesLabCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AAbilitiesLabCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AAbilitiesLabCharacter::LookUp);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}

void AAbilitiesLabCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AAbilitiesLabCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AAbilitiesLabCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AAbilitiesLabCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}
