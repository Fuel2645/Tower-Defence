// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "NodeStruct.generated.h"

USTRUCT()
struct FNode
{
	GENERATED_BODY()

	int Xpos = 0;
	int Ypos = 0;

	bool isBlocked = false;

	float GCost = 99999999.f;
	float HCost = 0.f;

	FIntPoint ParentNode = FIntPoint(-1, -1);

	const float FCost() { return GCost + HCost; };

};