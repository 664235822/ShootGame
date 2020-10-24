// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UFPSHealthComponent::UFPSHealthComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    // PrimaryComponentTick.bCanEverTick = true;

    DefaultHealth = 100.0f;

    SetIsReplicated(true);
}


// Called when the game starts
void UFPSHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    if (GetOwnerRole() == ROLE_Authority)
    {
        AActor* MyOwner = GetOwner();
        if (MyOwner)
        {
            MyOwner->OnTakeAnyDamage.AddDynamic(this, &UFPSHealthComponent::HandleTakeAnyDamage);
        }
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

void UFPSHealthComponent::OnRep_Health(float OldHealth)
{
    const float Damage = Health - OldHealth;

    OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void UFPSHealthComponent::Heal(float HealAmount)
{
    if (HealAmount <= 0 || Health <= 0)
    {
        return;
    }

    Health = FMath::Clamp(Health + HealAmount, 0.0f, DefaultHealth);

    OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}

void UFPSHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UFPSHealthComponent, Health);
}
