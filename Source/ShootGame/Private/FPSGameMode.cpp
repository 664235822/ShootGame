// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameMode.h"

#include "FPSGameState.h"
#include "FPSHealthComponent.h"
#include "FPSPlayerState.h"
#include "Runtime/Engine/Public/EngineUtils.h"

AFPSGameMode::AFPSGameMode()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 1.0f;

    GameStateClass = AFPSGameState::StaticClass();
    PlayerStateClass = AFPSPlayerState::StaticClass();
}

void AFPSGameMode::StartPlay()
{
    Super::StartPlay();

    PrepareForNextWave();
}

void AFPSGameMode::StartWave()
{
    WaveCount++;

    NumberOfBotsToSpawn = 2 * WaveCount;

    GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &AFPSGameMode::SpawnBotTimerElapsed, 1.0f, true,
                                    0.0f);

    SetWaveState(EWaveState::WaveInProgress);
}

void AFPSGameMode::EndWave()
{
    GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

    SetWaveState(EWaveState::WaitingToComplete);
}

void AFPSGameMode::PrepareForNextWave()
{
    GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AFPSGameMode::StartWave, TimeBetweenWaves, false);

    SetWaveState(EWaveState::WaitingToStart);

    RespawnDeadPlayers();
}

void AFPSGameMode::SpawnBotTimerElapsed()
{
    SpawnNewBot();

    NumberOfBotsToSpawn--;

    if (NumberOfBotsToSpawn <= 0)
    {
        EndWave();
    }
}

void AFPSGameMode::GameOver()
{
    EndWave();

    SetWaveState(EWaveState::GameOver);
}

void AFPSGameMode::CheckWaveState()
{
    if (NumberOfBotsToSpawn > 0 || GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart))
    {
        return;
    }

    bool bIsAnyBotAlive = false;

    for (FConstPawnIterator Iterator = GetWorld()->GetPawnIterator(); Iterator; ++Iterator)
    {
        APawn* TestPawn = Iterator->Get();
        if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
        {
            continue;
        }

        UFPSHealthComponent* HealthComponent = Cast<UFPSHealthComponent>(
            TestPawn->GetComponentByClass(UFPSHealthComponent::StaticClass()));

        if (HealthComponent && HealthComponent->GetHealth() > 0)
        {
            bIsAnyBotAlive = true;
            break;
        }
    }

    if (!bIsAnyBotAlive)
    {
        SetWaveState(EWaveState::WaveComplete);

        PrepareForNextWave();
    }
}

void AFPSGameMode::CheckAnyPlayerAlive()
{
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* PlayerController = Iterator->Get();
        if (PlayerController && PlayerController->GetPawn())
        {
            APawn* PlayerPawn = PlayerController->GetPawn();
            UFPSHealthComponent* HealthComponent = Cast<UFPSHealthComponent>(
                PlayerPawn->GetComponentByClass(UFPSHealthComponent::StaticClass()));
            if (ensure(HealthComponent) && HealthComponent->GetHealth() > 0.0f)
            {
                return;
            }
        }
    }

    GameOver();
}

void AFPSGameMode::SetWaveState(EWaveState NewState)
{
    AFPSGameState* MyGameState = GetGameState<AFPSGameState>();
    if (ensureAlways(MyGameState))
    {
        MyGameState->SetWaveState(NewState);
    }
}

void AFPSGameMode::RespawnDeadPlayers()
{
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* PlayerController = Iterator->Get();
        if (PlayerController && PlayerController->GetPawn() == nullptr)
        {
            RestartPlayer(PlayerController);
        }
    }
}

void AFPSGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    CheckWaveState();

    CheckAnyPlayerAlive();
}
