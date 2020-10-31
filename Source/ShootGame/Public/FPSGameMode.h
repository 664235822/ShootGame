// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FPSGameState.h"
#include "GameFramework/GameModeBase.h"
#include "FPSGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*,
                                               KillerController);

/**
 * 
 */
UCLASS()
class SHOOTGAME_API AFPSGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:

    AFPSGameMode();

    virtual void StartPlay() override;

protected:

    UFUNCTION(BlueprintImplementableEvent, Category = "Game Mode")
    void SpawnNewBot();

    void SpawnBotTimerElapsed();

    void StartWave();

    void EndWave();

    void PrepareForNextWave();

    void GameOver();

    FTimerHandle TimerHandle_BotSpawner;

    FTimerHandle TimerHandle_NextWaveStart;

    int32 NumberOfBotsToSpawn;

    int32 WaveCount;

    UPROPERTY(EditDefaultsOnly, Category = "Game Mode")
    float TimeBetweenWaves = 2.0f;

    void CheckWaveState();

    void CheckAnyPlayerAlive();

    void SetWaveState(EWaveState NewState);

    void RespawnDeadPlayers();

public:

    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(BlueprintAssignable, Category = "Game Mode")
    FOnActorKilled OnActorKilled;
};
