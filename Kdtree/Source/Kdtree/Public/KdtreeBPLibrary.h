/*!
 * Kdtree
 *
 * Copyright (c) 2019-2023 nutti
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "GameFramework/Actor.h"
#include "KdtreeCommon.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"

#include "KdtreeBPLibrary.generated.h"

UCLASS()
class KDTREE_API UKdtreeBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BluePrintCallable, Category = "SpacialDataStructure|kd-tree")
	static void BuildKdtree(FKdtree& Tree, const TArray<FVector>& Data);

	UFUNCTION(BluePrintCallable, Category = "SpacialDataStructure|kd-tree")
	static void ClearKdtree(UPARAM(ref) FKdtree& Tree);

	UFUNCTION(BluePrintCallable, Category = "SpacialDataStructure|kd-tree")
	static void CollectFromKdtree(
		const FKdtree& Tree, const FVector Center, float Radius, TArray<int>& Indices, TArray<FVector>& Data);

	UFUNCTION(BluePrintCallable, Category = "SpacialDataStructure|kd-tree")
	static void ValidateKdtree(const FKdtree& Tree);

	UFUNCTION(BluePrintCallable, Category = "SpacialDataStructure|kd-tree")
	static void DumpKdtreeToConsole(const FKdtree& Tree);
};
