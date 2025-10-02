// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponItem.h"
#include "../Player/PlayerCharacter.h"
#include "../Weapon/WeaponActor.h"


void UWeaponItem::Use(APlayerCharacter* PC) {
	if (!PC) return;
	this->OnUse(PC);
}