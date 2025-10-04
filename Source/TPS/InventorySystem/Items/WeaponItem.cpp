// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponItem.h"
#include "../Player/PlayerCharacter.h"
#include "../Weapon/WeaponActor.h"


void UWeaponItem::Use(APlayerCharacter* PC) {
	if (!PC || !this->WeaponActorClass) return;

	UWorld* WorldCtx = PC->GetWorld();
	AWeaponActor* SpawnedWeapon = this->RuntimeActor;

	if (!SpawnedWeapon) {
		SpawnedWeapon = WorldCtx->SpawnActor<AWeaponActor>(this->WeaponActorClass);

		SpawnedWeapon->SourceItem = this;
		SpawnedWeapon->OwningCharacter = PC;
		SpawnedWeapon->SetOwner(PC);

		SpawnedWeapon->CurrentAmmo = this->CurrentAmmo;
		SpawnedWeapon->MaxAmmo = this->MaxAmmo;
		SpawnedWeapon->SanityCost = this->SanityCost;
		SpawnedWeapon->WeaponType = this->WeaponType;
		this->RuntimeActor = SpawnedWeapon;
	}


	USkeletalMeshComponent* CharacterMesh = PC->GetMesh();

	SpawnedWeapon->Equip(PC);

	if (CharacterMesh && SpawnedWeapon->GunMesh) {
		FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
		SpawnedWeapon->AttachToComponent(CharacterMesh, AttachRules, FName(TEXT("ik_hand_r")));
	}

	PC->OnWeaponEquipped(this, SpawnedWeapon);
}