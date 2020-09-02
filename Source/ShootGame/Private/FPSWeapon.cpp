// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSWeapon.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AFPSWeapon::AFPSWeapon()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    RootComponent = SkeletalMeshComponent;

    MuzzleSocketName = "MuzzleSocket";
    TracerTargetName = "Target";
}

// Called when the game starts or when spawned
void AFPSWeapon::BeginPlay()
{
    Super::BeginPlay();
}

void AFPSWeapon::Fire()
{
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

        FVector TraceEndPoint = TraceEnd;

        FHitResult Hit;
        if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryOParams))
        {
            AActor* HitActor = Hit.GetActor();

            UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, MyOwner->GetInstigatorController(),
                                               this, DamageType);

            if (ImpactEffect)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint,
                                                         Hit.ImpactNormal.Rotation());
            }

            TraceEndPoint = Hit.ImpactPoint;
        }

        if (MuzzleEffect)
        {
            UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, SkeletalMeshComponent, MuzzleSocketName);
        }

        if (TracerEffect)
        {
            FVector MuzzleLocation = SkeletalMeshComponent->GetSocketLocation(MuzzleSocketName);

            UParticleSystemComponent* TracerComponent = UGameplayStatics::SpawnEmitterAtLocation(
                GetWorld(), TracerEffect, MuzzleLocation);
            if (TracerComponent)
            {
                TracerComponent->SetVectorParameter(TracerTargetName, TraceEndPoint);
            }
        }
    }
}

// Called every frame
void AFPSWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
