// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSWeapon.generated.h"

UCLASS()
class SHOOTGAME_API AFPSWeapon : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AFPSWeapon();


protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual void Fire();

    void PlayFireEffect(FVector TraceEndPoint);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* SkeletalMeshComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<UDamageType> DamageType;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float BaseDamage;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName MuzzleSocketName;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName TracerTargetName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* MuzzleEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* DefaultImpactEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* FleshImpactEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* TracerEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<UCameraShake> FireCameraShake;

    FTimerHandle TimerHandle;

    float LastFireTime;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float RateOfFire;

    float TimeBetweenShots;

public:

    void StartFire();

    void StopFire();
};