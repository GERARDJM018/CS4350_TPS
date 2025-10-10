// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Camera/PlayerCameraManager.h"
#include "../InventorySystem/ItemPickUpWrapper.h"
#include "../InventorySystem/Items/Interactable.h"
#include "../InventorySystem/Items/InventoryComponent.h"
#include "../InventorySystem/Items/Item.h"
#include "../InventorySystem/Items/WeaponItem.h"
#include "../InventorySystem/Weapon/Weapon.h"
#include "../InventorySystem/Weapon/WeaponActor.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
    Inventory->Capacity = 20;
    CurrHealth = MaxHealth;
    CurrSanity = MaxSanity;
}

void APlayerCharacter::UseItem(UItem* Item)
{
	if (!Item)
	{
		return;
	}
	
	Item->Use(this);
	// blueprint event
	Item->OnUse(this);
}

FVector APlayerCharacter::GetLookAtPoint() const
{
    const float Range = 15000.f;

    const FVector CamLoc = GetPawnViewLocation();          // camera/eyes
    const FRotator AimRot = GetBaseAimRotation();           // controller aim
    const FVector End = CamLoc + AimRot.Vector() * Range;

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(GetLookAtPoint), /*bTraceComplex=*/true, this);
    Params.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, End, ECC_Visibility, Params) && Hit.bBlockingHit)
    {
        return Hit.Location;
    }
    return End;
}

void APlayerCharacter::OnWeaponEquipped(UWeaponItem* Item, AWeaponActor* WeaponActor)
{
    if (!Item || !WeaponActor) return;

    TObjectPtr<AWeaponActor> SlotActor = nullptr;
    TObjectPtr<UWeaponItem> SlotItem = nullptr;

    // Get the current weapon in the slot
    switch (Item->WeaponType)
    {
    case EMyWeaponType::LongGun:
        SlotActor = PrimaryWeapon;
        SlotItem = PrimaryWeaponItem;
        break;
    case EMyWeaponType::Pistol:
        SlotActor = SecondaryWeapon;
        SlotItem = SecondaryWeaponItem;
        break;
    case EMyWeaponType::ColdWeapon:
        SlotActor = MeleeWeapon;
        SlotItem = MeleeWeaponItem;
        break;
    default:
        return;
    }

    // Check for desync: if we have an item without actor or actor without item, clean up the slot
    if ((SlotItem && !SlotActor) || (SlotActor && !SlotItem))
    {
        // Clear the slot to fix inconsistent state
        switch (Item->WeaponType)
        {
        case EMyWeaponType::LongGun:
            PrimaryWeapon = nullptr;
            PrimaryWeaponItem = nullptr;
            break;
        case EMyWeaponType::Pistol:
            SecondaryWeapon = nullptr;
            SecondaryWeaponItem = nullptr;
            break;
        case EMyWeaponType::ColdWeapon:
            MeleeWeapon = nullptr;
            MeleeWeaponItem = nullptr;
            break;
        }
        SlotActor = nullptr;
        SlotItem = nullptr;
    }

    // If clicking the *same* item that's already equipped → unequip it
    if (SlotItem == Item && SlotActor)
    {
        
        // Clear the weapon slot
        switch (Item->WeaponType)
        {
        case EMyWeaponType::LongGun:
            PrimaryWeapon = nullptr;
            PrimaryWeaponItem = nullptr;
            break;
        case EMyWeaponType::Pistol:
            SecondaryWeapon = nullptr;
            SecondaryWeaponItem = nullptr;
            break;
        case EMyWeaponType::ColdWeapon:
            MeleeWeapon = nullptr;
            MeleeWeaponItem = nullptr;
            break;
        }
        
        // Update active weapon slot to next available weapon
        if (this->PrimaryWeapon) {
            this->ActiveWeaponSlot = 0;
        }
        else if (this->SecondaryWeapon) {
            this->ActiveWeaponSlot = 1;
        }
        else if (this->MeleeWeapon) {
            this->ActiveWeaponSlot = 2;
        }
        else {
            this->ActiveWeaponSlot = -1;
        }
        return;
    }

    // Now equip the new weapon
    switch (Item->WeaponType)
    {
    case EMyWeaponType::LongGun:
        PrimaryWeapon = WeaponActor;
        PrimaryWeaponItem = Item;
        break;
    case EMyWeaponType::Pistol:
        SecondaryWeapon = WeaponActor;
        SecondaryWeaponItem = Item;
        break;
    case EMyWeaponType::ColdWeapon:
        MeleeWeapon = WeaponActor;
        MeleeWeaponItem = Item;
        break;
    }

    // Set the active weapon slot based on weapon type
    switch (Item->WeaponType)
    {
    case EMyWeaponType::LongGun:
        this->ActiveWeaponSlot = 0;
        break;
    case EMyWeaponType::Pistol:
        this->ActiveWeaponSlot = 1;
        break;
    case EMyWeaponType::ColdWeapon:
        this->ActiveWeaponSlot = 2;
        break;
    default:
        break;
    }

}

void APlayerCharacter::DropWeapon(UWeaponItem* WeaponItem)
{
    if (!WeaponItem || !Inventory) return;

    // Find the corresponding weapon actor (if it's currently spawned)
    AWeaponActor* WeaponActor = nullptr;
    int32 DroppedWeaponSlot = -1;
    
    if (WeaponItem == PrimaryWeaponItem)
    {
        WeaponActor = PrimaryWeapon;
        PrimaryWeapon = nullptr;
        PrimaryWeaponItem = nullptr;
        DroppedWeaponSlot = 0;
    }
    else if (WeaponItem == SecondaryWeaponItem)
    {
        WeaponActor = SecondaryWeapon;
        SecondaryWeapon = nullptr;
        SecondaryWeaponItem = nullptr;
        DroppedWeaponSlot = 1;
    }
    else if (WeaponItem == MeleeWeaponItem)
    {
        WeaponActor = MeleeWeapon;
        MeleeWeapon = nullptr;
        MeleeWeaponItem = nullptr;
        DroppedWeaponSlot = 2;
    }

    // Detach and enable physics on the actor if it exists in hand/holster
    if (WeaponActor)
    {
        WeaponActor->Drop();
    }

    // Spawn a pickup wrapper in the world at the drop location
    FVector dropLocation;
    FRotator dropRotation;
    if (WeaponActor)
    {
        // Drop at the actor's current location
        dropLocation = WeaponActor->GetActorLocation();
        dropRotation = WeaponActor->GetActorRotation();
        WeaponActor->Destroy();  // remove the actor from the character
    }
    else
    {
        // If the weapon actor wasn�t spawned (edge case), drop at player�s feet
        dropLocation = GetActorLocation() + GetActorForwardVector() * 50.f;
        dropRotation = FRotator::ZeroRotator;
    }

    FActorSpawnParameters SpawnParams;
    AItemPickUpWrapper* NewPickup = GetWorld()->SpawnActor<AItemPickUpWrapper>(AItemPickUpWrapper::StaticClass(), dropLocation, dropRotation, SpawnParams);
    if (NewPickup)
    {
        // Transfer the item to the wrapper
        NewPickup->WrappedItem = WeaponItem;
        // If desired, set auto-pickup or other properties:
        // NewPickup->bAutoPickUp = false; (maybe require manual pickup)
    }

    // Remove the item from inventory data
    Inventory->RemoveItem(WeaponItem);

    // Update active slot index if needed
    if (ActiveWeaponSlot == DroppedWeaponSlot)
    {
        // Find the next available weapon slot
        if (PrimaryWeaponItem)
            ActiveWeaponSlot = 0;
        else if (SecondaryWeaponItem)
            ActiveWeaponSlot = 1;
        else if (MeleeWeaponItem)
            ActiveWeaponSlot = 2;
        else
            ActiveWeaponSlot = -1; // No weapons available
    }
}

void APlayerCharacter::UnequipWeapon(UWeaponItem* WeaponItem)
{
    if (!WeaponItem) return;

    AWeaponActor* UnequippedWeapon = nullptr;

    if (WeaponItem == this->PrimaryWeaponItem) {
        UnequippedWeapon = this->PrimaryWeapon;
        this->PrimaryWeapon = nullptr;
        this->PrimaryWeaponItem = nullptr;
    }
    else if (WeaponItem == this->SecondaryWeaponItem) {
        UnequippedWeapon = this->SecondaryWeapon;
        this->SecondaryWeapon = nullptr;
        this->SecondaryWeaponItem = nullptr;
    }
    else if (WeaponItem == this->MeleeWeaponItem) {
        UnequippedWeapon = this->MeleeWeapon;
        this->MeleeWeapon = nullptr;
        this->MeleeWeaponItem = nullptr;
    }
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    // Assume "Interact" action mapping is bound to E in project settings:
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::Interact);
}

void APlayerCharacter::Interact()
{
    const float AimRange = 15000.f;  // how far we aim into the world
    const float InteractReach = 200.f;    // how far from the socket we can pick up
    const float InteractRadius = 32.f;     // sphere radius to be forgiving

    // 1) Camera-based aim (start from eyes, use control rotation)
    const FVector  CamStart = GetPawnViewLocation();
    const FRotator AimRot = GetBaseAimRotation();
    const FVector  CamEnd = CamStart + AimRot.Vector() * AimRange;

    // Trace from camera to figure out where we're looking
    FHitResult CamHit;
    FCollisionQueryParams CamParams(SCENE_QUERY_STAT(Interact_Aim), /*bTraceComplex=*/true, this);
    CamParams.AddIgnoredActor(this);

    const bool bCamHit = GetWorld()->LineTraceSingleByChannel(
        CamHit, CamStart, CamEnd, ECC_Visibility, CamParams);

    const FVector TargetPoint = (bCamHit && CamHit.bBlockingHit)
        ? CamHit.ImpactPoint
        : CamEnd;

    // 2) From the head/muzzle/etc., sweep toward that target
    const FVector SocketLocation = GetMesh()->GetSocketLocation(TEXT("head"));
    FVector Dir = (TargetPoint - SocketLocation).GetSafeNormal(KINDA_SMALL_NUMBER);
    if (Dir.IsNearlyZero())
    {
        Dir = GetActorForwardVector(); // fallback
    }
    const FVector SweepEnd = SocketLocation + Dir * InteractReach;

    // Sphere sweep so we don�t miss tiny pickups
    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(Interact_Sweep), /*bTraceComplex=*/true, this);
    Params.AddIgnoredActor(this);
    TArray<AActor*> AttachedActors;
    GetAttachedActors(AttachedActors);          // all actors attached to this one
    Params.AddIgnoredActors(AttachedActors);    // ignore them wholesale

    // If you have a dedicated �Interactable� object channel, use it here.
    FCollisionObjectQueryParams ObjParams; // default = all
    // e.g., ObjParams.AddObjectTypesToQuery(ECC_GameTraceChannel1); // Interactable

    const bool bHit = GetWorld()->SweepSingleByObjectType(
        Hit,
        SocketLocation,
        SweepEnd,
        FQuat::Identity,
        ObjParams,
        FCollisionShape::MakeSphere(InteractRadius),
        Params
    );

#if !(UE_BUILD_SHIPPING)
    DrawDebugLine(GetWorld(), CamStart, TargetPoint, FColor::Cyan, false, 1.f);
    DrawDebugCapsule(GetWorld(),
        (SocketLocation + SweepEnd) * 0.5f,              // mid
        InteractReach * 0.5f, InteractRadius,
        FRotationMatrix::MakeFromX(Dir).ToQuat(),
        bHit ? FColor::Green : FColor::Red, false, 1.f);
#endif

    if (bHit && Hit.GetActor())
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
        {
            IInteractable::Execute_Interact(HitActor, this);
        }
    }
}
