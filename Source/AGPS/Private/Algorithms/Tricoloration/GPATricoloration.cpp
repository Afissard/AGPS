// Fill out your copyright notice in the Description page of Project Settings.


#include "Algorithms/Tricoloration/GPATricoloration.h"

/**
 * @brief Retrieve the map's corners based of the navmesh's triangulation.
 *
 * To understand this method, we need to understand the structure of a recast navmesh which is the following :
 * 
 * Tiles -> Polygons -> Triangles -> Vertices
 * 
 * First part : Loop to retrieve each vertex.
 * Second part : Retrieve a unique instance of each vertex.
 * Third part : Sets each vertices neighbors in sort that they are triangulated.
 * @return No return but sets Vertices, an array of Coloration vertices that represents the maps corners. 
 */
void UGPATricoloration::ComputeVertices()
{
	TArray<FNavTileRef> Tiles;
	TArray<FNavPoly> Polys;
	TArray<FVector> TilesLocations;

	// First Part :
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
				
				// Second Part :
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
			
			// Third Part :
			for (int l = 0; l < PolygonVertices.Num(); l++)
			{
				for (int m = 0; m < PolygonVertices.Num(); m++)
				{
					if (PolygonVertices[l].Id != PolygonVertices[m].Id )
					{
						if (l==0 || m == 0) // permet de trianguler sous la forme d'un eventail
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
						else if ( l - m == 1 || l - m == -1 )
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

/**
 * Return a specific coloration vertices of the navmesh based on its index.
 * If no element correspond to the selected index, return a nullptr.
 * 
 * @param Id Index of the wanted vertices.
 * 
 * @return A FColorationVertice that represents a vertices of the navmesh that is aware of its neighbors.
 */
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

