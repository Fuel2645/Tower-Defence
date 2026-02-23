// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"

// Sets default values
AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();
	Initalise();
}

void AGridManager::Initalise()
{
	Grid.SetNum(GridHeight * GridWidth);

	for (int x = 0; x < GridWidth; x++)
	{
		for (int y = 0; y < GridHeight; y++)
		{
			Grid[y * GridWidth + x].Xpos = x;
			Grid[y * GridWidth + x].Ypos = y;
			Grid[y * GridWidth + x].isBlocked = false;
		}
	}

}

bool AGridManager::isValidPos(int x, int y)
{
	return x>=0 && x < GridWidth && y>=0 && y < GridHeight;
}

FNode& AGridManager::GetNode(int x, int y)
{
	check(isValidPos(x, y));
	return Grid[y * GridWidth + x];
}

float AGridManager::Heuristic(FIntPoint& A, FIntPoint& B)
{
	return FMath::Abs(A.X -B.X) + FMath::Abs(A.Y-B.Y);
}

void AGridManager::GetNeightbours(FIntPoint& Node, TArray<FIntPoint>& Out)
{
	FIntPoint Directions[4] =
	{
		FIntPoint(1,0),
		FIntPoint(-1,0),
		FIntPoint(0,1),
		FIntPoint(0,-1),
	};

	for (FIntPoint& Direction : Directions)
	{
		if (isValidPos(Node.X + Direction.X, Node.Y + Direction.Y))
		{
			Out.Add(FIntPoint(Node.X + Direction.X, Node.Y + Direction.Y));
		}
	}

}
bool AGridManager::FindPath(TArray<FIntPoint>& OutPath)
{
	OutPath.Empty();

	TArray<FIntPoint> OpenSet;
	TSet<FIntPoint> ClosedSet;

	for (FNode& Node : Grid)
	{
		Node.GCost = 9999999999.f;
		Node.HCost = 0.f;
		Node.ParentNode = FIntPoint(-1, -1);
	}

	OpenSet.Add(Start);

	GetNode(Start.X, Start.Y).GCost = 0.f;
	GetNode(Start.X, Start.Y).HCost = Heuristic(Start,End);

	while (OpenSet.Num() > 0)
	{
		int CurIndex = 0;
		FIntPoint CurPos = OpenSet[0];

		for (int z = 1; z < OpenSet.Num(); z++)
		{
			FNode& A = GetNode(OpenSet[z].X, OpenSet[z].Y);
			FNode& B = GetNode(CurPos.X, CurPos.Y);

			if (A.FCost() < B.FCost() || (A.FCost() == B.FCost() && A.HCost < B.HCost))
			{
				CurIndex = z;
				CurPos = OpenSet[z];
			}
		}

		OpenSet.RemoveAt(CurIndex);
		ClosedSet.Add(CurPos);

		if (CurPos == End)
		{
			FIntPoint TracePos = End;

			while (TracePos != Start)
			{
				if (!isValidPos(TracePos.X, TracePos.Y))
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Invalid Position"));
					return false;
				}


				OutPath.Add(TracePos);

				if (GetNode(TracePos.X, TracePos.Y).ParentNode == FIntPoint(-1, -1))
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Invalid Parent"));
					return false;
				}
				TracePos = GetNode(TracePos.X, TracePos.Y).ParentNode;


			}

			OutPath.Add(Start);
			Algo::Reverse(OutPath);
			return true;
		}

		TArray<FIntPoint> Neightbours;
		GetNeightbours(CurPos, Neightbours);

		for (FIntPoint& Neighbour : Neightbours)
		{
			if (ClosedSet.Contains(Neighbour))
				continue;

			FNode& NeighboursNode = GetNode(Neighbour.X, Neighbour.Y);

			if (NeighboursNode.isBlocked)
				continue;

			float newGCost = GetNode(CurPos.X, CurPos.Y).GCost + 1.f;

			if (newGCost < NeighboursNode.GCost)
			{
				NeighboursNode.GCost = newGCost;
				NeighboursNode.HCost = Heuristic(Neighbour, End);
				NeighboursNode.ParentNode = CurPos;

				if (!OpenSet.Contains(Neighbour))
				{
					OpenSet.Add(Neighbour);
				}
			}
		}
	}


	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SOmethig went wrong"));
	return false;
}

bool AGridManager::CanPlaceTower(FIntPoint& Pos)
{
	if (!isValidPos(Pos.X, Pos.Y))
		return false;

	FNode& NodeToCheck = GetNode(Pos.X, Pos.Y);

	if (NodeToCheck.isBlocked)
		return false;

	NodeToCheck.isBlocked = true;

	TArray<FIntPoint> tempPath;
	bool validPath = FindPath(tempPath);

	if (validPath)
	{
		return true;
	}
	else
	{
		NodeToCheck.isBlocked = false;
		return false;
	}
}

void AGridManager::SetIsBlocked(FIntPoint& Pos, bool isBlocked)
{
	Grid[Pos.Y * GridWidth + Pos.X].isBlocked = isBlocked;
}

bool AGridManager::TryPlaceTower(FIntPoint Pos)
{
	if (CanPlaceTower(Pos))
	{
		SetIsBlocked(Pos, true);
		return true;
	}

	return false;

}

FIntPoint AGridManager::WorldToGrid(FVector WorldPos)
{
	return(FIntPoint(FMath::FloorToInt(WorldPos.X / TileSize), FMath::FloorToInt(WorldPos.Y / TileSize)));
}

bool AGridManager::TempPlaceTower(FIntPoint Pos, TArray<FIntPoint>& OutPath)
{
	if (CanPlaceTower(Pos))
	{
		SetIsBlocked(Pos, true);
		FindPath(OutPath);
		SetIsBlocked(Pos, false);
		return true;
	}

	return false;
}
