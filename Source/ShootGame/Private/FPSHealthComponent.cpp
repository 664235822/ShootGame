// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSHealthComponent.h"

// Sets default values for this component's properties
UFPSHealthComponent::UFPSHealthComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    // PrimaryComponentTick.bCanEverTick = true;

    DefaultHealth = 100.0f;
}


// Called when the game starts
void UFPSHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* MyOwner = GetOwner();
    if (MyOwner)
    {
        MyOwner->OnTakeAnyDamage.AddDynamic(this, &UFPSHealthComponent::HandleTakeAnyDamage);
    }

    Health = DefaultHealth;
}

void UFPSHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                              AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.0f)
    {
        return;
    }

    Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

    OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}
