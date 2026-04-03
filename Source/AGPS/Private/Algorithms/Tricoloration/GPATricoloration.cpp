// Fill out your copyright notice in the Description page of Project Settings.


#include "Algorithms/Tricoloration/GPATricoloration.h"

void UGPATricoloration::ComputeVertices()
{
	TArray<FNavTileRef> Tiles;
	TArray<FNavPoly> Polys;
	TArray<FVector> TilesLocations;

	NavMesh->GetAllNavMeshTiles(Tiles);
	for (int i = 0; i < Tiles.Num(); i++)
	{
		NavMesh->GetPolysInTile(Tiles[i], Polys);
		for (int j = 0; j < Polys.Num(); j++)
		{
			NavMesh->GetPolyVerts(Polys[j].Ref, TilesLocations);
			TArray<FColorationVertice> PolygonVertices;
			for (int k = 0; k < TilesLocations.Num(); k++)
			{
				bool bIsRegistered = false;
				for (int l = 0; l < Vertices.Num(); l++)
				{
					if (Vertices[l].Location == TilesLocations[k])
					{
						bIsRegistered = true;
						PolygonVertices.Add(Vertices[l]);
					}
				}
				if (!bIsRegistered)
				{
					FColorationVertice NewVertice;
					NewVertice.Id = Vertices.Num();
					NewVertice.Location = TilesLocations[k];
					Vertices.Add(NewVertice);
					PolygonVertices.Add(NewVertice);
				}
				UE_LOG(LogTemp, Warning, TEXT("Vertice : %f, %f, %f"), TilesLocations[k].X, TilesLocations[k].Y, TilesLocations[k].Z)
			}
			for (int l = 0; l < PolygonVertices.Num(); l++)
			{
				for (int m = 0; m < PolygonVertices.Num(); m++)
				{
					if (PolygonVertices[l].Id != PolygonVertices[m].Id )
					{
						if ((l == 1 && m == 3) || (l == 3 && m == 1) ) {} // C'est pas beau mais ça permet d'enlever les connections qui ne font pas partie de celle des triangles (jusqu'à preuve du contraire)
						else if (!PolygonVertices[l].Links.Contains(PolygonVertices[m].Id))
						{
							for (int n = 0; n < Vertices.Num(); n++)
							{
								if (Vertices[n].Id == PolygonVertices[l].Id)
								{
									Vertices[n].Links.Add(PolygonVertices[m].Id);
									break;
								}
							}
						}
					}
				}
			}
		}
	}
}

FColorationVertice* UGPATricoloration::FindById(int32 Id)
{
	for (FColorationVertice& Element : Vertices)
	{
		if (Element.Id == Id)
		{
			return &Element;
		}
	}
	return nullptr;
}



bool UGPATricoloration::IsColorValid(int32 VertexIndex, int32 Color)
{
	FColorationVertice& V = Vertices[VertexIndex];

	for (int32 NeighborId : V.Links)
	{
		if (FColorationVertice* Neighbor = FindById(NeighborId))
		{
			if (Neighbor->Color == Color)
			{
				return false;
			}
		}
	}

	return true;
}

bool UGPATricoloration::ColorGraph(int32 VertexIndex)
{
	if (VertexIndex >= Vertices.Num())
	{
		return true;
	}

	FColorationVertice& V = Vertices[VertexIndex];

	for (int32 Color = 1; Color <= 3; Color++)
	{
		if (IsColorValid(VertexIndex, Color))
		{
			V.Color = Color;

			if (ColorGraph(VertexIndex + 1))
			{
				return true;
			}

			V.Color = 0;
		}
	}
	return false;
}


void UGPATricoloration::ResetColors()
{
	for (FColorationVertice& V : Vertices)
	{
		V.Color = 0;
	}
}

UWorld* UGPATricoloration::GetEditorWorld()
{
	return GEditor
		? GEditor->GetEditorWorldContext().World()
		: nullptr;
}

void UGPATricoloration::DebugView()
{
	UWorld* World = GetEditorWorld();
	
	for (FColorationVertice Vs : Vertices)
	{
		FVector StartLocation = Vs.Location;
		TArray<FColorationVertice> Links;
		for (int32 Neighbors : Vs.Links)
		{
			FColorationVertice* Neighbor = FindById(Neighbors);
			FVector EndLocation = Neighbor->Location;
			DrawDebugLine(World, StartLocation, EndLocation, FColor::Red, true,-1,0, 2 );
			
		}
	}
}

TArray<FGuardSpawnSetting> UGPATricoloration::ComputeGuardLocations(ARecastNavMesh* InNavMesh)
{
	TArray<FGuardSpawnSetting> GuardLocation = TArray<FGuardSpawnSetting>();
	NavMesh = InNavMesh;
	
	ComputeVertices();
	
	for (FColorationVertice& V : Vertices)
	{
		V.Color = 0;
	}

	if (ColorGraph(0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Graph successfully 3-colored"));

		for (const FColorationVertice& V : Vertices)
		{
			UE_LOG(LogTemp, Warning, TEXT("Node %d -> Color %d"), V.Id, V.Color);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Graph is NOT 3-colorable"));
	}

	int32 LessColor;

	int32 Count1 = 0;
	int32 Count2 = 0;
	int32 Count3 = 0;

	for (const FColorationVertice& V : Vertices)
	{
		switch (V.Color)
		{
		case 1: Count1++;
			break;
		case 2: Count2++;
			break;
		case 3: Count3++;
			break;
		default: break;
		}
	}

	if (Count1 <= Count2 && Count1 <= Count3)
		LessColor = 1;
	else if (Count2 <= Count1 && Count2 <= Count3)
		LessColor = 2;
	else
		LessColor = 3;

	UE_LOG(LogTemp, Error, TEXT("Less color : %i"), LessColor);
	
	for (FColorationVertice& V : Vertices)
	{
		if (V.Color == LessColor)
		{
			FGuardSpawnSetting NewGuard;
			 NewGuard.SpawnLocation = V.Location;
			 NewGuard.SpawnRotation = FRotator(0, 0, 0);
			GuardLocation.Add(NewGuard);
		}
	}
	
	return GuardLocation;
}

