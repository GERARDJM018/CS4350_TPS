// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"
#include "../Player/PlayerCharacter.h"

AWeaponActor::AWeaponActor()
{
    GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    SetRootComponent(GunMesh);
    // Start with physics disabled (will be enabled when dropped)
    GunMesh->SetSimulatePhysics(false);
    GunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GunMesh->SetGenerateOverlapEvents(false);
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
    
    // Make sure the weapon is visible when equipped
    GunMesh->SetVisibility(true);
    SetActorHiddenInGame(false);
}


void AWeaponActor::UnEquip_Implementation()
{
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    OwningCharacter = nullptr;
    SetOwner(nullptr);
    // Don�t enable physics or collision here � leave it as-is
    GunMesh->SetSimulatePhysics(false);
    GunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GunMesh->SetGenerateOverlapEvents(false);
    
    // Hide the weapon when unequipped (will be shown again when re-equipped)
    GunMesh->SetVisibility(false);
    SetActorHiddenInGame(true);
}

void AWeaponActor::Drop_Implementation()
{
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    SetOwner(nullptr);
    OwningCharacter = nullptr;

    // Back to �world� behavior
    GunMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GunMesh->SetSimulatePhysics(true);
    GunMesh->SetGenerateOverlapEvents(true);
    GunMesh->SetSimulatePhysics(true);
}

