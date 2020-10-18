// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPSHealthComponent.generated.h"

UCLASS( ClassGroup=(FPSCustom), meta=(BlueprintSpawnableComponent) )
class SHOOTGAME_API UFPSHealthComponent : public UActorComponent
{
    GENERATED_BODY()

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UFPSHealthComponent*, OwningHealthComponent,
                                                 float,
                                                 Health, float, HealthDelta, const class UDamageType*, DamageType,
                                                 class AController*, InstigatedBy, AActor*, DamageCauser);

public:
    // Sets default values for this component's properties
    UFPSHealthComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    UPROPERTY(ReplicatedUsing = OnRep_Health, EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
    float Health;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
    float DefaultHealth;

    UFUNCTION()
    void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
                             class AController* InstigatedBy, AActor* DamageCauser);

    UFUNCTION()
    void OnRep_Health(float OldHealth);
    
public:

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnHealthChangedSignature OnHealthChanged;
};
