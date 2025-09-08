// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../InventorySystem//Items/InventoryComponent.h"
#include "PlayerCharacter.generated.h"

class UInventoryComponent;

UCLASS()
class TPS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int MaxHealth;


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	int currHealth;

};
