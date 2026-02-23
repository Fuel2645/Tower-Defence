// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NodeStruct.h"
#include "GridManager.generated.h"

UCLASS()
class TOWERDEFENCE_API AGridManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GridWidth = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GridHeight = 50;


	float TileSize = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint Start;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint End;

	UFUNCTION(BlueprintCallable)
	bool FindPath(TArray<FIntPoint>& OutPath);
	bool CanPlaceTower(FIntPoint& Pos);
	void SetIsBlocked(FIntPoint& Pos, bool isBlocked);



	UFUNCTION(BlueprintCallable)
	bool TryPlaceTower(FIntPoint Pos);

	UFUNCTION(BlueprintCallable)
	FIntPoint WorldToGrid(FVector WorldPos);

	UFUNCTION(BlueprintCallable)
	bool TempPlaceTower(FIntPoint Pos, TArray<FIntPoint>& OutPath);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	TArray<FNode> Grid;

	void Initalise();
	bool isValidPos(int x, int y);
	FNode& GetNode(int x, int y);
	float Heuristic(FIntPoint& A, FIntPoint& B);
	void GetNeightbours(FIntPoint& Node, TArray<FIntPoint>& Out);
};
