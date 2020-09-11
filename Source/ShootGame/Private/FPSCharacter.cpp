// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SprintArmComponent"));
    SpringArmComponent->bUsePawnControlRotation = true;
    SpringArmComponent->SetupAttachment(RootComponent);

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent);

    FPSHealthComponent = CreateDefaultSubobject<UFPSHealthComponent>(TEXT("HealthComponent"));

    ZoomedFOV = 65.0f;
    ZoomInterpSpeed = 20.0f;

    WeaponSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
    Super::BeginPlay();

    DefaultFOV = CameraComponent->FieldOfView;
    FPSHealthComponent->OnHealthChanged.AddDynamic(this, &AFPSCharacter::OnHealthChanged);

    if (GetLocalRole() == ROLE_Authority)
    {
        FActorSpawnParameters SpawnParameters;
        SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        CurrentWeapon = GetWorld()->SpawnActor<AFPSWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator,
                                                           SpawnParameters);
        if (CurrentWeapon)
        {
            CurrentWeapon->SetOwner(this);
            CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
                                             WeaponSocketName);
        }
    }
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    float TargetFOV = bWantToZoom ? ZoomedFOV : DefaultFOV;
    float CurrentFOV = FMath::FInterpTo(CameraComponent->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
    CameraComponent->SetFieldOfView(CurrentFOV);
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);
    PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::AddControllerYawInput);

    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFPSCharacter::BeginCrouch);
    PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFPSCharacter::EndCrouch);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::Jump);

    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::StartFire);
    PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFPSCharacter::StopFire);

    PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AFPSCharacter::BeginZoom);
    PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AFPSCharacter::EndZoom);
}

FVector AFPSCharacter::GetPawnViewLocation() const
{
    if (CameraComponent)
    {
        return CameraComponent->GetComponentLocation();
    }

    return Super::GetPawnViewLocation();
}

void AFPSCharacter::MoveForward(float Value)
{
    AddMovementInput(GetActorForwardVector() * Value);
}

void AFPSCharacter::MoveRight(float Value)
{
    AddMovementInput(GetActorRightVector() * Value);
}

void AFPSCharacter::StartFire()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->StartFire();
    }
}

void AFPSCharacter::StopFire()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->StopFire();
    }
}

void AFPSCharacter::BeginCrouch()
{
    Crouch();
}

void AFPSCharacter::EndCrouch()
{
    UnCrouch();
}

void AFPSCharacter::BeginZoom()
{
    bWantToZoom = true;
}

void AFPSCharacter::EndZoom()
{
    bWantToZoom = false;
}

void AFPSCharacter::OnHealthChanged(UFPSHealthComponent* HealthComponent, float Health, float HealthDelta,
                                    const class UDamageType* DamageType, class AController* InstigatedBy,
                                    AActor* DamageCauser)
{
    if (Health <= 0.0f && !bDied)
    {
        bDied = true;

        GetMovementComponent()->StopMovementImmediately();

        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        DetachFromControllerPendingDestroy();

        SetLifeSpan(10.0f);
    }
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFPSCharacter, CurrentWeapon);
    DOREPLIFETIME(AFPSCharacter, bDied);
}
