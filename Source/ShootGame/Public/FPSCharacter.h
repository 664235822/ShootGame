// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSHealthComponent.h"
#include "FPSWeapon.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "FPSCharacter.generated.h"

UCLASS()
class SHOOTGAME_API AFPSCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AFPSCharacter();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void MoveForward(float Value);

    void MoveRight(float Value);

    void StartFire();

    void StopFire();

    void BeginCrouch();

    void EndCrouch();

    void BeginZoom();

    void EndZoom();

    UFUNCTION()
    void OnHealthChanged(UFPSHealthComponent* HealthComponent, float Health, float HealthDelta,
                         const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UFPSHealthComponent* FPSHealthComponent;

    UPROPERTY(Replicated)
    AFPSWeapon* CurrentWeapon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<AFPSWeapon> WeaponClass;

    UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
    FName WeaponSocketName;

    bool bWantToZoom;

    UPROPERTY(EditDefaultsOnly, Category = "Player")
    float ZoomedFOV;

    float DefaultFOV;

    UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1f, ClampMax = 100.0f))
    float ZoomInterpSpeed;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
    bool bDied = false;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual FVector GetPawnViewLocation() const override;
};
