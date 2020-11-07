// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSHealthComponent.h"

#include "FPSGameMode.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UFPSHealthComponent::UFPSHealthComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    // PrimaryComponentTick.bCanEverTick = true;

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
    if (Damage <= 0.0f || bIsDead)
    {
        return;
    }

    if (DamageCauser != DamagedActor && IsFriendly(DamageCauser, DamagedActor))
    {
        return;
    }

    Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

    bIsDead = Health <= 0.0f;

    OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

    if (bIsDead)
    {
        AFPSGameMode* GameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            GameMode->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
        }
    }
}

void UFPSHealthComponent::OnRep_Health(float OldHealth)
{
    const float Damage = Health - OldHealth;

    OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

float UFPSHealthComponent::GetHealth() const
{
    return Health;
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

bool UFPSHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
    if (ActorA == nullptr || ActorB == nullptr)
    {
        return true;
    }

    UFPSHealthComponent* HealthComponentA = Cast<UFPSHealthComponent>(
        ActorA->GetComponentByClass(UFPSHealthComponent::StaticClass()));
    UFPSHealthComponent* HealthComponentB = Cast<UFPSHealthComponent>(
        ActorB->GetComponentByClass(UFPSHealthComponent::StaticClass()));

    if (HealthComponentA == nullptr || HealthComponentB == nullptr)
    {
        return true;
    }

    return HealthComponentA->TeamNumber == HealthComponentB->TeamNumber;
}

void UFPSHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UFPSHealthComponent, Health);
}
