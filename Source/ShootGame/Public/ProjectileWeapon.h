// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSWeapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTGAME_API AProjectileWeapon : public AFPSWeapon
{
    GENERATED_BODY()

protected:

    virtual void Fire() override;
    
    UPROPERTY(EditDefaultsOnly, Category = "ProjectileWeapon")
    TSubclassOf<AActor> ProjectileClass;
};
