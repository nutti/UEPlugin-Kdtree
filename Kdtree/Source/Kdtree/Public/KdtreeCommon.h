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
