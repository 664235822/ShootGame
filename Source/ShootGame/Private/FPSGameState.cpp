// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"

#include "Net/UnrealNetwork.h"

void AFPSGameState::OnRep_WaveState(EWaveState OldState)
{
    WaveStateChanged(WaveState, OldState);
}

void AFPSGameState::SetWaveState(EWaveState NewWaveState)
{
    if (GetLocalRole() == ROLE_Authority)
    {
        const EWaveState OldState = WaveState;

        WaveState = NewWaveState;
        OnRep_WaveState(OldState);
    }
}

void AFPSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFPSGameState, WaveState);
}
