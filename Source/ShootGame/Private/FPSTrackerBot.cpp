// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSTrackerBot.h"


#include "AudioDevice.h"
#include "FPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Runtime/NavigationSystem/Public/NavigationPath.h"
#include "GameFramework/Character.h"

// Sets default values
AFPSTrackerBot::AFPSTrackerBot()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    HealthComponent = CreateDefaultSubobject<UFPSHealthComponent>(TEXT("HealthComponent"));
    HealthComponent->OnHealthChanged.AddDynamic(this, &AFPSTrackerBot::OnHealthChanged);

    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SphereComponent->SetCollisionResponseToChannels(ECR_Ignore);
    SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
    SphereComponent->SetSphereRadius(ExplosionRadius);
    SphereComponent->SetupAttachment(RootComponent);

    AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
    AudioComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AFPSTrackerBot::BeginPlay()
{
    Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority)
    {
        NextPathPoint = GetNextPathPoint();
    }
}

FVector AFPSTrackerBot::GetNextPathPoint()
{
    ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0);
    UNavigationPath* NavigationPath = UNavigationSystemV1::FindPathToActorSynchronously(
        this, GetActorLocation(), Character);
    if (NavigationPath)
    {
        if (NavigationPath->PathPoints.Num() > 0)
        {
            return NavigationPath->PathPoints[1];
        }
    }

    return GetActorLocation();
}

void AFPSTrackerBot::OnHealthChanged(UFPSHealthComponent* OwningHealthComponent, float Health, float HealthDelta,
                                     const UDamageType* DamageType, AController* InstigatedBy,
                                     AActor* DamageCauser)
{
    if (MaterialInstance == nullptr)
    {
        MaterialInstance = MeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(
            0, MeshComponent->GetMaterial(0));
    }

    if (MaterialInstance)
    {
        MaterialInstance->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
    }

    if (Health <= 0)
    {
        SelfDestruct();
    }
}

void AFPSTrackerBot::SelfDestruct()
{
    if (bExploded)
    {
        return;
    }

    bExploded = true;

    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
    UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());

    MeshComponent->SetVisibility(false, true);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (GetLocalRole() == ROLE_Authority)
    {
        TArray<AActor*> IgnoreActors;
        IgnoreActors.Add(this);

        UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr,
                                            IgnoreActors, this, GetInstigatorController(), true);

        SetLifeSpan(2.0f);
    }
}

void AFPSTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (bStartSelfDestruction)
    {
        return;
    }

    AFPSCharacter* Character = Cast<AFPSCharacter>(OtherActor);
    if (Character)
    {
        if (GetLocalRole() == ROLE_Authority)
        {
            GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &AFPSTrackerBot::DamageSelf,
                                            SelfDamageInterval, true, 0.0f);
        }

        UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);

        bStartSelfDestruction = true;
    }
}

void AFPSTrackerBot::DamageSelf()
{
    UGameplayStatics::ApplyDamage(this, 20.0f, GetInstigatorController(), this, nullptr);
}

// Called every frame
void AFPSTrackerBot::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GetLocalRole() == ROLE_Authority && !bExploded)
    {
        const float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

        if (DistanceToTarget <= RequiredDistanceToTarget)
        {
            NextPathPoint = GetNextPathPoint();
        }
        else
        {
            FVector ForceDirection = NextPathPoint - GetActorLocation();
            ForceDirection.Normalize();

            ForceDirection *= 1000;

            MeshComponent->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
        }

        const float VolumeMultiplier = FMath::GetMappedRangeValueClamped(FVector2D(10.0f, 1000.0f), FVector2D(0, 2.0f),
                                                                         GetVelocity().Size());
        AudioComponent->SetVolumeMultiplier(VolumeMultiplier);
    }
}
