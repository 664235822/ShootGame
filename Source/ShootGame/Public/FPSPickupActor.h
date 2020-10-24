// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "FPSPowerupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/Actor.h"
#include "FPSPickupActor.generated.h"

UCLASS()
class SHOOTGAME_API AFPSPickupActor : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AFPSPickupActor();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USphereComponent* SphereComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UDecalComponent* DecalComponent;

    UPROPERTY(EditInstanceOnly, Category = "PickupActor")
    TSubclassOf<AFPSPowerupActor> PowerupClass;

    void Respawn();

    AFPSPowerupActor* PowerupInstance;

    UPROPERTY(EditInstanceOnly, Category = "PickupActor")
    float CooldownDuration = 10.0f;

    FTimerHandle TimerHandle_RespawnTimer;

public:

    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
