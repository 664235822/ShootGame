// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPowerupActor.h"
#include "Net/UnrealNetwork.h"

AFPSPowerupActor::AFPSPowerupActor()
{
    SetReplicates(true);
}

void AFPSPowerupActor::BeginPlay()
{
    Super::BeginPlay();
}

void AFPSPowerupActor::OnTickPowerup()
{
    TicksProcessed++;

    OnPowerupTicked();

    if (TicksProcessed >= TotalNumberOfTicks)
    {
        OnExpired();

        bIsPowerupActive = false;
        OnRep_PowerupActive();

        GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
    }
}

void AFPSPowerupActor::OnRep_PowerupActive()
{
    OnPowerupStateChanged(bIsPowerupActive);
}

void AFPSPowerupActor::ActivatedPowerUp(AActor* OtherActor)
{
    OnActivated(OtherActor);

    bIsPowerupActive = true;
    OnRep_PowerupActive();

    if (PowerupInterval > 0.0f)
    {
        GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &AFPSPowerupActor::OnTickPowerup,
                                        PowerupInterval, true);
    }
    else
    {
        OnTickPowerup();
    }
}

void AFPSPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFPSPowerupActor, bIsPowerupActive);
}
