/*!
 * Kdtree
 *
 * Copyright (c) 2019-2023 nutti
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "KdtreeInternal.h"

#include "KdtreeBPLibrary.h"

namespace KdtreeInternal
{
namespace
{
template <typename T>
void Swap(T* Var1, T* Var2)
{
	T Tmp = *Var1;
	*Var1 = *Var2;
	*Var2 = Tmp;
}

template <typename T>
void QuickSort(T* First, T* Last, TFunctionRef<int(T, T)> Comparator)
{
	T* Left = First;
	T* Right = Last;
	T Pivot = *First;

	while (true)
	{
		while (Left <= Last && Comparator(*Left, Pivot) == 1)
		{
			Left++;
		}
		while (Right >= First && Comparator(*Right, Pivot) == -1)
		{
			Right--;
		}
		if (Left > Right)
		{
			break;
		}
		Swap(Left, Right);
		Left++;
		Right--;
	}

	if (First < Left - 1)
	{
		QuickSort(First, Left - 1, Comparator);
	}
	if (Left < Last)
	{
		QuickSort(Left, Last, Comparator);
	}
}

template <typename T>
void NthElement(T* First, T* Nth, T* Last, TFunctionRef<int(T, T)> Comparator)
{
	// TODO: Use faster algorithm such as introselect.
	QuickSort(First, Last - 1, Comparator);
}

FKdtreeNode* BuildNode(const FKdtreeInternal& Tree, int* Indices, int NumData, int Depth)
{
	if (NumData <= 0)
	{
		return nullptr;
	}

	const int Axis = Depth % 3;
	const int Middle = (NumData - 1) / 2;

	NthElement<int>(Indices, Indices + Middle, Indices + NumData, [&](int Lhs, int Rhs) {
		if (Tree.Data[Lhs][Axis] < Tree.Data[Rhs][Axis])
		{
			return 1;
		}
		else if (Tree.Data[Lhs][Axis] > Tree.Data[Rhs][Axis])
		{
			return -1;
		}

		return 0;
	});

	FKdtreeNode* NewNode = new FKdtreeNode();
	NewNode->Index = Indices[Middle];
	NewNode->Axis = Axis;
	NewNode->ChildLeft = BuildNode(Tree, Indices, Middle, Depth + 1);
	NewNode->ChildRight = BuildNode(Tree, Indices + Middle + 1, NumData - Middle - 1, Depth + 1);

	return NewNode;
}

void ClearNode(FKdtreeNode* Node)
{
	if (Node == nullptr)
	{
		return;
	}

	if (Node->ChildLeft)
	{
		ClearNode(Node->ChildLeft);
	}
	if (Node->ChildRight)
	{
		ClearNode(Node->ChildRight);
	}

	delete Node;
}

void ValidateKdtree(const FKdtreeInternal& Tree, const FKdtreeNode* Node, int Depth)
{
	if (Node == nullptr)
	{
		return;
	}

	int Axis = Node->Axis;
	const FKdtreeNode* NodeLeft = Node->ChildLeft;
	const FKdtreeNode* NodeRight = Node->ChildRight;
	if (NodeLeft)
	{
		if (Tree.Data[Node->Index][Axis] < Tree.Data[NodeLeft->Index][Axis])
		{
			UE_LOG(LogTemp, Error, TEXT("Kdtree is invalid: tree.Data[%d][%d](%f) < tree.Data[%d][%d](%f)"), Node->Index, Axis,
				Tree.Data[Node->Index][Axis], NodeLeft->Index, Axis, Tree.Data[NodeLeft->Index][Axis]);
		}
	}
	if (NodeRight)
	{
		if (Tree.Data[Node->Index][Axis] > Tree.Data[NodeRight->Index][Axis])
		{
			UE_LOG(LogTemp, Error, TEXT("Kdtree is invalid: tree.Data[%d][%d](%f) > tree.Data[%d][%d](%f)"), Node->Index, Axis,
				Tree.Data[Node->Index][Axis], NodeRight->Index, Axis, Tree.Data[NodeRight->Index][Axis]);
		}
	}

	if (NodeLeft)
	{
		ValidateKdtree(Tree, NodeLeft, Depth + 1);
	}
	if (NodeRight)
	{
		ValidateKdtree(Tree, NodeRight, Depth + 1);
	}
}

void DumpNode(const FKdtreeInternal& Tree, const FKdtreeNode* Node)
{
	if (Node == nullptr)
	{
		return;
	}

	FString Left = "null";
	FString Right = "null";

	if (Node->ChildLeft)
	{
		Left = FString::FromInt(Node->ChildLeft->Index);
	}
	if (Node->ChildRight)
	{
		Right = FString::FromInt(Node->ChildRight->Index);
	}

	UE_LOG(LogTemp, Display, TEXT("[%d] value=(%f, %f, %f), axis=%d, child_left=%s, child_right=%s"), Node->Index,
		Tree.Data[Node->Index][0], Tree.Data[Node->Index][1], Tree.Data[Node->Index][2], Node->Axis, *Left, *Right);
}

void DumpKdTree(const FKdtreeInternal& Tree, const FKdtreeNode* Node)
{
	if (Node == nullptr)
	{
		return;
	}

	DumpNode(Tree, Node);

	if (Node->ChildLeft)
	{
		DumpKdTree(Tree, Node->ChildLeft);
	}
	if (Node->ChildRight)
	{
		DumpKdTree(Tree, Node->ChildRight);
	}
}

void CollectFromKdtree(
	const FKdtreeInternal& Tree, const FKdtreeNode* Node, const FVector& Center, float Radius, TArray<int>* Result)
{
	if (Node == nullptr)
	{
		return;
	}

	const FVector& Current = Tree.Data[Node->Index];
	if (FVector::DistSquared(Center, Current) < Radius * Radius)
	{
		Result->Add(Node->Index);
	}

	int Axis = Node->Axis;
	if (Center[Axis] < Current[Axis])
	{
		CollectFromKdtree(Tree, Node->ChildLeft, Center, Radius, Result);
	}
	else
	{
		CollectFromKdtree(Tree, Node->ChildRight, Center, Radius, Result);
	}

	float Diff = FMath::Abs(Center[Axis] - Current[Axis]);
	if (Diff < Radius)
	{
		if (Center[Axis] < Current[Axis])
		{
			CollectFromKdtree(Tree, Node->ChildRight, Center, Radius, Result);
		}
		else
		{
			CollectFromKdtree(Tree, Node->ChildLeft, Center, Radius, Result);
		}
	}
}
}	 // namespace

void BuildKdtree(FKdtreeInternal* Tree, const TArray<FVector>& Data)
{
	ClearKdtree(Tree);

	Tree->Data = Data;

	TArray<int> Indices;
	for (int Index = 0; Index < Data.Num(); ++Index)
	{
		Indices.Add(Index);
	}

	Tree->Root = BuildNode(*Tree, Indices.GetData(), Indices.Num(), 0);
}

void ClearKdtree(FKdtreeInternal* Tree)
{
	ClearNode(Tree->Root);
	Tree->Data.Empty();
}

void CollectFromKdtree(const FKdtreeInternal& Tree, const FVector& Center, float Radius, TArray<int>* Result)
{
	CollectFromKdtree(Tree, Tree.Root, Center, Radius, Result);
}

void ValidateKdtree(const FKdtreeInternal& Tree)
{
	ValidateKdtree(Tree, Tree.Root, 0);
}

void DumpKdTree(const FKdtreeInternal& Tree)
{
	UE_LOG(LogTemp, Display, TEXT("========== DUMP FKdtree =========="));
	DumpKdTree(Tree, Tree.Root);
	UE_LOG(LogTemp, Display, TEXT("=================================="));
}

}	 // namespace KdtreeInternal
