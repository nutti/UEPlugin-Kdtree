// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/ObjectMacros.h"
#include "GameFramework/Actor.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "KdtreeBPLibrary.generated.h"

struct FKdtreeNode;

struct FKdtreeInternal
{
    TArray<FVector> Data;
    FKdtreeNode* Root;
};

USTRUCT(BlueprintType)
struct FKdtree
{
    GENERATED_USTRUCT_BODY()
    
    FKdtreeInternal Internal;
};

USTRUCT(BlueprintType)
struct FActorKdtree
{
    GENERATED_USTRUCT_BODY()
    
    TArray<AActor*> Actors;
    FKdtreeInternal Internal;
};

UCLASS()
class UKdtreeBPLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()
    
private:
    
    UFUNCTION(BluePrintCallable, Category="kd-tree")
    static void BuildKdtree(FKdtree& Tree, const TArray<FVector>& Data);
    
    UFUNCTION(BluePrintCallable, Category="kd-tree")
    static void BuildActorKdtree(FActorKdtree& Tree, const TArray<AActor*>& Actors);
    
    UFUNCTION(BluePrintCallable, Category="kd-tree")
    static void ClearKdtree(UPARAM(ref) FKdtree& Tree);
    
    UFUNCTION(BluePrintCallable, Category="kd-tree")
    static void ClearActorKdtree(UPARAM(ref) FActorKdtree& Tree);
    
    UFUNCTION(BluePrintCallable, Category="kd-tree")
    static void CollectFromKdtree(const FKdtree& Tree, const FVector& Center, float Radius,
                                  TArray<int>& Indices, TArray<FVector>& Data);
    
    UFUNCTION(BluePrintCallable, Category="kd-tree")
    static void CollectFromActorKdtree(const FActorKdtree& Tree, const FVector& Center,
                                       float Radius, TArray<int>& Indices, TArray<AActor*>& Data);
    
    UFUNCTION(BluePrintCallable, Category="kd-tree")
    static void ValidateKdtree(const FKdtree& Tree);

    UFUNCTION(BluePrintCallable, Category="kd-tree")
    static void ValidateActorKdtree(const FActorKdtree& Tree);

    UFUNCTION(BluePrintCallable, Category="kd-tree")
    static void DumpKdtreeToConsole(const FKdtree& Tree);

    UFUNCTION(BluePrintCallable, Category="kd-tree")
    static void DumpActorKdtreeToConsole(const FActorKdtree& Tree);
};
