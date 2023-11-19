/*!
 * Kdtree
 *
 * Copyright (c) 2019-2023 nutti
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "Engine/LatentActionManager.h"
#include "KdtreeBPLibrary.h"
#include "KdtreeCommon.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LatentActions.h"

#include "AsyncKdtreeBPLibrary.generated.h"

UCLASS()
class KDTREE_API UAsyncKdtreeBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable,
		meta = (WorldContextObject = "WorldContextObject", Latent, LatentInfo = "LatentInfo", HidePin = "WorldContextObject",
			DefaultToSelf = "WorldContextObject"),
		Category = "SpacialDataStructure|kd-tree")
	static void BuildKdtreeAsync(
		const UObject* WorldContextObject, FKdtree& Tree, const TArray<FVector>& Data, FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintCallable,
		meta = (WorldContextObject = "WorldContextObject", Latent, LatentInfo = "LatentInfo", HidePin = "WorldContextObject",
			DefaultToSelf = "WorldContextObject"),
		Category = "SpacialDataStructure|kd-tree")
	static void CollectFromKdtreeAsync(const UObject* WorldContextObject, const FKdtree& Tree, const FVector Center, float Radius,
		TArray<int>& Indices, TArray<FVector>& Data, FLatentActionInfo LatentInfo);
};
