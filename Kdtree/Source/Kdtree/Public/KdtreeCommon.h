/*!
 * Kdtree
 *
 * Copyright (c) 2019-2023 nutti
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

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

/**
 * Enum to represent different collection methods for Kdtree
 */
UENUM(BlueprintType)
enum class EKdtreeCollectionType : uint8
{
	Circle UMETA(DisplayName = "Circle (Default)"),
	Box UMETA(DisplayName = "Box")
};
