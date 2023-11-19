/*!
 * Kdtree
 *
 * Copyright (c) 2019-2023 nutti
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "KdtreeBPLibrary.h"

namespace KdtreeInternal
{
struct FKdtreeNode
{
	int Index = -1;
	int Axis = -1;
	FKdtreeNode* ChildLeft = nullptr;
	FKdtreeNode* ChildRight = nullptr;
};

void BuildKdtree(FKdtreeInternal* Tree, const TArray<FVector>& Data);
void ClearKdtree(FKdtreeInternal* Tree);
void CollectFromKdtree(const FKdtreeInternal& Tree, const FVector& Center, float Radius, TArray<int>* Result);
void ValidateKdtree(const FKdtreeInternal& Tree);
void DumpKdTree(const FKdtreeInternal& Tree);
}	 // namespace KdtreeInternal
