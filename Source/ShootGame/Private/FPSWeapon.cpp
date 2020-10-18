// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSWeapon.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "ShootGame/ShootGame.h"
#include "Engine/Public/TimerManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AFPSWeapon::AFPSWeapon()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    // PrimaryActorTick.bCanEverTick = true;

    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    RootComponent = SkeletalMeshComponent;

    MuzzleSocketName = "MuzzleSocket";
    TracerTargetName = "Target";

    BaseDamage = 20.0f;

    RateOfFire = 600.0f;

    SetReplicates(true);

    NetUpdateFrequency = 66.0f;
    MinNetUpdateFrequency = 33.0f;
}

// Called when the game starts or when spawned
void AFPSWeapon::BeginPlay()
{
    Super::BeginPlay();

    TimeBetweenShots = 60.0f / RateOfFire;
}

void AFPSWeapon::ServerFire_Implementation()
{
    Fire();
}

bool AFPSWeapon::ServerFire_Validate()
{
    return true;
}

void AFPSWeapon::Fire()
{
    if (GetLocalRole() < ROLE_Authority)
    {
        ServerFire();
    }

    AActor* MyOwner = GetOwner();
    if (MyOwner)
    {
        FVector EyeLocation;
        FRotator EyeRotation;
        MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

        FVector ShotDirection = EyeRotation.Vector();
        FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

        FCollisionQueryParams QueryOParams;
        QueryOParams.AddIgnoredActor(MyOwner);
        QueryOParams.AddIgnoredActor(this);
        QueryOParams.bTraceComplex = true;
        QueryOParams.bReturnPhysicalMaterial = true;

        FVector TraceEndPoint = TraceEnd;

        EPhysicalSurface SurfaceType = SurfaceType_Default;

        FHitResult Hit;
        if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryOParams))
        {
            AActor* HitActor = Hit.GetActor();

            SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

            float CurrentDamage = BaseDamage;

            if (SurfaceType == SURFACE_FLESHVULNERABLE)
            {
                CurrentDamage *= 2.5f;
            }

            UGameplayStatics::ApplyPointDamage(HitActor, CurrentDamage, ShotDirection, Hit,
                                               MyOwner->GetInstigatorController(),
                                               this, DamageType);

            PlayImpactEffect(SurfaceType, Hit.ImpactPoint);

            TraceEndPoint = Hit.ImpactPoint;
        }

        PlayFireEffect(TraceEndPoint);

        if (GetLocalRole() == ROLE_Authority)
        {
            HitScanTrace.TraceTo = TraceEndPoint;
            HitScanTrace.SurfaceType = SurfaceType;
        }

        LastFireTime = GetWorld()->TimeSeconds;
    }
}

void AFPSWeapon::PlayFireEffect(FVector TraceEndPoint)
{
    if (MuzzleEffect)
    {
        UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, SkeletalMeshComponent, MuzzleSocketName);
    }

    if (TracerEffect)
    {
        const FVector MuzzleLocation = SkeletalMeshComponent->GetSocketLocation(MuzzleSocketName);

        UParticleSystemComponent* TracerComponent = UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(), TracerEffect, MuzzleLocation);
        if (TracerComponent)
        {
            TracerComponent->SetVectorParameter(TracerTargetName, TraceEndPoint);
        }
    }

    APawn* MyOwner = Cast<APawn>(GetOwner());
    if (MyOwner)
    {
        APlayerController* PlayerController = Cast<APlayerController>(MyOwner->GetController());
        if (PlayerController)
        {
            PlayerController->ClientPlayCameraShake(FireCameraShake);
        }
    }
}

void AFPSWeapon::PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
    UParticleSystem* SelectEffect = nullptr;
    switch (SurfaceType)
    {
    case SURFACE_FLESHDEFAULT:
    case SURFACE_FLESHVULNERABLE:
        SelectEffect = FleshImpactEffect;
        break;
    default:
        SelectEffect = DefaultImpactEffect;
        break;
    }

    if (SelectEffect)
    {
        const FVector MuzzleLocation = SkeletalMeshComponent->GetSocketLocation(MuzzleSocketName);

        FVector TraceDirection = ImpactPoint - MuzzleLocation;
        TraceDirection.Normalize();

        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectEffect, ImpactPoint,
                                                 TraceDirection.Rotation());
    }
}

void AFPSWeapon::OnRep_HitScanTrace()
{
    PlayFireEffect(HitScanTrace.TraceTo);

    PlayImpactEffect(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void AFPSWeapon::StartFire()
{
    const float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

    GetWorldTimerManager().SetTimer(TimerHandle, this, &AFPSWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void AFPSWeapon::StopFire()
{
    GetWorldTimerManager().ClearTimer(TimerHandle);
}

void AFPSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(AFPSWeapon, HitScanTrace, COND_SkipOwner);
}
