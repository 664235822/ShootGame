// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSWeapon.generated.h"

USTRUCT()
struct FHitScanTrace
{
    GENERATED_BODY()
    ;

    UPROPERTY()
    TEnumAsByte<EPhysicalSurface> SurfaceType;

    UPROPERTY()
    FVector_NetQuantize TraceTo;
};

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

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerFire();

    virtual void Fire();

    void PlayFireEffect(FVector TraceEndPoint);

    void PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* SkeletalMeshComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<UDamageType> DamageType;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float BaseDamage = 20.0f;;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName MuzzleSocketName = "MuzzleSocket";

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName TracerTargetName = "Target";

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
    float RateOfFire = 600.0f;

    float TimeBetweenShots;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0.0f))
    float BulletSpread = 1.0f;

    UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
    FHitScanTrace HitScanTrace;

    UFUNCTION()
    void OnRep_HitScanTrace();

public:

    void StartFire();

    void StopFire();
};
