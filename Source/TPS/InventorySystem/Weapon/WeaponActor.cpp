// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"
#include "../Player/PlayerCharacter.h"

AWeaponActor::AWeaponActor()
{
    GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    SetRootComponent(GunMesh);
    GunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GunMesh->SetSimulatePhysics(true);
    GunMesh->SetGenerateOverlapEvents(true);
}

void AWeaponActor::Equip_Implementation(APlayerCharacter* PC)
{
    if (!PC) return;

    OwningCharacter = PC;
    SetOwner(PC);

    // Turn off physics/collision while held
    GunMesh->SetSimulatePhysics(false);
    GunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GunMesh->SetGenerateOverlapEvents(false);
}


void AWeaponActor::Unequip_Implementation()
{
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    SetOwner(nullptr);
    OwningCharacter = nullptr;

    // Back to “world” behavior
    GunMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GunMesh->SetSimulatePhysics(true);
    GunMesh->SetGenerateOverlapEvents(true);
}