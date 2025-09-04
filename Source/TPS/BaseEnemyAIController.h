// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API ABaseEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Variables")
	APawn* Player;

	
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;
	
};
