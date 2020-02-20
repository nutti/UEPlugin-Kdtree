// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/ObjectMacros.h"
#include "GameFramework/Actor.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KdtreeCommon.h"

#include "KdtreeBPLibrary.generated.h"


UCLASS()
class KDTREE_API UKdtreeBPLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()
    
public:
    
    UFUNCTION(BluePrintCallable, Category="SpacialDataStructure|kd-tree")
    static void BuildKdtree(FKdtree& Tree, const TArray<FVector>& Data);

    UFUNCTION(BluePrintCallable, Category="SpacialDataStructure|kd-tree")
    static void ClearKdtree(UPARAM(ref) FKdtree& Tree);
    
    UFUNCTION(BluePrintCallable, Category="SpacialDataStructure|kd-tree")
    static void CollectFromKdtree(const FKdtree& Tree, const FVector Center, float Radius,
                                  TArray<int>& Indices, TArray<FVector>& Data);
    
    UFUNCTION(BluePrintCallable, Category="SpacialDataStructure|kd-tree")
    static void ValidateKdtree(const FKdtree& Tree);

    UFUNCTION(BluePrintCallable, Category="SpacialDataStructure|kd-tree")
    static void DumpKdtreeToConsole(const FKdtree& Tree);

};
