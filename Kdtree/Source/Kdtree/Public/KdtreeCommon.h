#pragma once

#include "UObject/ObjectMacros.h"

#include "KdtreeCommon.generated.h"

namespace KdtreeInternal
{
    struct FKdtreeNode;
}

struct FKdtreeInternal
{
    TArray<FVector> Data;
    KdtreeInternal::FKdtreeNode* Root;
};

USTRUCT(BlueprintType)
struct KDTREE_API FKdtree
{
    GENERATED_USTRUCT_BODY()
    
    FKdtreeInternal Internal;
};
