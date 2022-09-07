// Fill out your copyright notice in the Description page of Project Settings.


#include "BobCharacter.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "BobAttributeComponent.h"
#include "BobInteractComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystemComponent.h>
#include "BobActionComponent.h"

// Sets default values
ABobCharacter::ABobCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractComp = CreateDefaultSubobject<UBobInteractComponent>("InteractComp");

	AttributeComp = CreateDefaultSubobject<UBobAttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<UBobActionComponent>("ActionComp");

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;
}

void ABobCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AttributeComp->OnHealthChanged.AddDynamic(this, &ABobCharacter::OnHealthChanged);
}

FVector ABobCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

void ABobCharacter::MoveForward(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll= 0.0f;

	
	AddMovementInput(ControlRot.Vector(), Value);
}

void ABobCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

	AddMovementInput(RightVector, Value);
}



void ABobCharacter::PrimaryAttack() // damage
{
	ActionComp->StartActionByName(this, "PrimaryAttack");
}

void ABobCharacter::SecondaryAttack() // teleport
{
	ActionComp->StartActionByName(this, "SecondaryAttack");
}

void ABobCharacter::UltimateAttack() // black hole
{
	ActionComp->StartActionByName(this, "UltimateAttack");
}

void ABobCharacter::SprintStart()
{
	ActionComp->StartActionByName(this, "Sprint");
}

void ABobCharacter::SprintStop()
{
	ActionComp->StopActionByName(this, "Sprint");
}



void ABobCharacter::OnHealthChanged(AActor* InstigatorActor, UBobAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
	}
	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
		SetActorEnableCollision(false);
		GetMesh()->SetCollisionProfileName("Ragdoll");

		//lifespan
		SetLifeSpan(3.0f);
	}
}



void ABobCharacter::PrimaryInteract()
{
	InteractComp->PrimaryInteract();
}




// Called to bind functionality to input
void ABobCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{


	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABobCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABobCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ABobCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ABobCharacter::SecondaryAttack);
	PlayerInputComponent->BindAction("UltimateAttack", IE_Pressed, this, &ABobCharacter::UltimateAttack);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABobCharacter::Jump);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ABobCharacter::PrimaryInteract);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ABobCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ABobCharacter::SprintStop);
}

void ABobCharacter::HealSelf(float Amount/* = 100 */)
{
	AttributeComp->ApplyHeathChange(this, Amount);
}

