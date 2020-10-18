// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FPSHealthComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "Sound/SoundCue.h"

#include "FPSTrackerBot.generated.h"

UCLASS()
class SHOOTGAME_API AFPSTrackerBot : public APawn
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    AFPSTrackerBot();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UFPSHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* SphereComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UAudioComponent* AudioComponent;

    FVector NextPathPoint;

    FVector GetNextPathPoint();

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    float MovementForce = 1000.0f;

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    bool bUseVelocityChange = true;

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    float RequiredDistanceToTarget = 100;

    UFUNCTION()
    void OnHealthChanged(UFPSHealthComponent* OwningHealthComponent, float Health, float HealthDelta,
                         const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


    UMaterialInstanceDynamic* MaterialInstance;

    void SelfDestruct();

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    UParticleSystem* ExplosionEffect;

    bool bExploded = false;

    bool bStartSelfDestruction = false;

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    float ExplosionRadius = 200.0f;

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    float ExplosionDamage = 40.0f;

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    float SelfDamageInterval = 0.5f;

    FTimerHandle TimerHandle_SelfDamage;

    void DamageSelf();

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    USoundCue* SelfDestructSound;

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
    USoundCue* ExplodeSound;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
