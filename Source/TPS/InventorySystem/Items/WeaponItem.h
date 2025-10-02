// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "WeaponItem.generated.h"

class APlayerCharacter;
class AWeaponActor;

/**
 * 
 */
UCLASS()
class TPS_API UWeaponItem : public UItem
{

	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AWeaponActor> WeaponActorClass;

protected:

	UPROPERTY(Transient)
	TObjectPtr<AWeaponActor> RuntimeActor = nullptr;
	virtual void Use(APlayerCharacter* PlayerCharacter) override;

};