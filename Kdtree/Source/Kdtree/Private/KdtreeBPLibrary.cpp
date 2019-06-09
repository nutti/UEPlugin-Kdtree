// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "KdtreeBPLibrary.h"
#include "Kdtree.h"

#include <functional>

struct FKdtreeNode
{
    int Index = -1;
    int Axis = -1;
    FKdtreeNode* ChildLeft = nullptr;
    FKdtreeNode* ChildRight = nullptr;
};

namespace
{
    void CollectInternal(const FKdtreeInternal& Tree, const FKdtreeNode* Node, const FVector& Center,
                         float Radius, TArray<int>* Result)
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
            CollectInternal(Tree, Node->ChildLeft, Center, Radius, Result);
        }
        else
        {
            CollectInternal(Tree, Node->ChildRight, Center, Radius, Result);
        }
        
        float Diff = FMath::Abs(Center[Axis] - Current[Axis]);
        if (Diff < Radius)
        {
            if (Center[Axis] < Current[Axis])
            {
                CollectInternal(Tree, Node->ChildRight, Center, Radius, Result);
            }
            else
            {
                CollectInternal(Tree, Node->ChildLeft, Center, Radius, Result);
            }
        }
    }
    
    template <typename T>
    void NthElement(T* First, T* Nth, T* Last, std::function<bool(T, T)> Comparator)
    {
        T* MaxIndex = First;
        for (T* Index = First; Index <= Nth; Index++)
        {
            // *Index > *MaxIndex
            if (Comparator(*Index, *MaxIndex) == false)
            {
                MaxIndex = Index;
            }
        }
        
        for (T* Index = Nth + 1; Index < Last; Index++)
        {
            // *Index < *MaxIndex
            if (Comparator(*Index, *MaxIndex) == true)
            {
                // swap indices
                T OldIndex = *MaxIndex;
                *MaxIndex = *Index;
                *Index = OldIndex;
                
                // update max value
                MaxIndex = First;
                for (T* IndexTmp = First; IndexTmp <= Nth; IndexTmp++)
                {
                    if (Comparator(*IndexTmp, *MaxIndex) == false)
                    {
                        MaxIndex = IndexTmp;
                    }
                }
            }
        }
    }
    
    FKdtreeNode* BuildNode(const FKdtreeInternal& Tree, int* Indices, int NumData, int Depth)
    {
        if (NumData <= 0)
        {
            return nullptr;
        }
        
        const int Axis = Depth % 3;
        const int Middle = (NumData - 1) / 2;
        
        NthElement<int>(Indices, Indices + Middle, Indices + NumData, [&](int Lhs, int Rhs)
        {
            return Tree.Data[Lhs][Axis] < Tree.Data[Rhs][Axis];
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
    
    void ValidateKdtreeInternal(const FKdtreeInternal& Tree, const FKdtreeNode* Node, int Depth)
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
                UE_LOG(LogTemp, Fatal, TEXT("Kdtree is invalid: tree.Data[%d][%d](%f) < tree.Data[%d][%d](%f)"),
                       Node->Index, Axis, Tree.Data[Node->Index][Axis],
                       Node->Index, Axis, Tree.Data[NodeLeft->Index][Axis]);
            }
        }
        if (NodeRight)
        {
            if (Tree.Data[Node->Index][Axis] > Tree.Data[NodeRight->Index][Axis])
            {
                UE_LOG(LogTemp, Fatal, TEXT("Kdtree is invalid: tree.Data[%d][%d](%f) > tree.Data[%d][%d](%f)"),
                       Node->Index, Axis, Tree.Data[Node->Index][Axis],
                       Node->Index, Axis, Tree.Data[NodeLeft->Index][Axis]);
            }
        }
        
        if (NodeLeft)
        {
            ValidateKdtreeInternal(Tree, NodeLeft, Depth + 1);
        }
        if (NodeRight)
        {
            ValidateKdtreeInternal(Tree, NodeRight, Depth + 1);
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
        
        UE_LOG(LogTemp, Display, TEXT("[%d] value=(%f, %f, %f), axis=%d, child_left=%s, child_right=%s"),
               Node->Index,
               Tree.Data[Node->Index][0],
               Tree.Data[Node->Index][1],
               Tree.Data[Node->Index][2],
               Node->Axis,
               *Left,
               *Right);
    }
    
    void DumpKdTreeInternal(const FKdtreeInternal& Tree, const FKdtreeNode* Node)
    {
        if (Node == nullptr)
        {
            return;
        }
        
        DumpNode(Tree, Node);
        
        if (Node->ChildLeft)
        {
            DumpKdTreeInternal(Tree, Node->ChildLeft);
        }
        if (Node->ChildRight)
        {
            DumpKdTreeInternal(Tree, Node->ChildRight);
        }
    }
    
    void DumpKdTree(const FKdtreeInternal& Tree)
    {
        UE_LOG(LogTemp, Display, TEXT("========== DUMP FKdtree =========="));
        DumpKdTreeInternal(Tree, Tree.Root);
        UE_LOG(LogTemp, Display, TEXT("=================================="));
    }
    
} // namespace

UKdtreeBPLibrary::UKdtreeBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UKdtreeBPLibrary::BuildKdtree(FKdtree& Tree, const TArray<FVector>& Data)
{
    ClearKdtree(Tree);
    
    Tree.Internal.Data = Data;
    
    TArray<int> Indices;
    for (int Index = 0; Index < Data.Num(); ++Index)
    {
        Indices.Add(Index);
    }
    
    Tree.Internal.Root = BuildNode(Tree.Internal, Indices.GetData(), Indices.Num(), 0);
}

void UKdtreeBPLibrary::BuildActorKdtree(FActorKdtree& Tree, const TArray<AActor*>& Actors)
{
    ClearActorKdtree(Tree);
    
    Tree.Actors = Actors;
    
    for (int Index = 0; Index < Actors.Num(); ++Index)
    {
        Tree.Internal.Data.Add(Actors[Index]->GetActorLocation());
    }
    
    TArray<int> Indices;
    for (int Index = 0; Index < Actors.Num(); ++Index)
    {
        Indices.Add(Index);
    }
    
    Tree.Internal.Root = BuildNode(Tree.Internal, Indices.GetData(), Indices.Num(), 0);
}

void UKdtreeBPLibrary::ValidateKdtree(const FKdtree& Tree)
{
    ValidateKdtreeInternal(Tree.Internal, Tree.Internal.Root, 0);
}

void UKdtreeBPLibrary::ValidateActorKdtree(const FActorKdtree& Tree)
{
    ValidateKdtreeInternal(Tree.Internal, Tree.Internal.Root, 0);
}

void UKdtreeBPLibrary::ClearKdtree(FKdtree& Tree)
{
    ClearNode(Tree.Internal.Root);
    Tree.Internal.Data.Empty();
}

void UKdtreeBPLibrary::ClearActorKdtree(FActorKdtree& Tree)
{
    ClearNode(Tree.Internal.Root);
    Tree.Internal.Data.Empty();
    Tree.Actors.Empty();
}

void UKdtreeBPLibrary::CollectFromKdtree(const FKdtree& Tree, const FVector& Center, float Radius,
                                         TArray<int>& Indices, TArray<FVector>& Data)
{
    CollectInternal(Tree.Internal, Tree.Internal.Root, Center, Radius, &Indices);
    for (int Index = 0; Index < Indices.Num(); ++Index)
    {
        Data.Add(Tree.Internal.Data[Index]);
    }
}

void UKdtreeBPLibrary::CollectFromActorKdtree(const FActorKdtree& Tree, const FVector& Center,
                                              float Radius, TArray<int>& Indices, TArray<AActor*>& Data)
{
    CollectInternal(Tree.Internal, Tree.Internal.Root, Center, Radius, &Indices);
    for (int Index = 0; Index < Indices.Num(); ++Index)
    {
        Data.Add(Tree.Actors[Index]);
    }
}

void UKdtreeBPLibrary::DumpKdtreeToConsole(const FKdtree& Tree)
{
    DumpKdTreeInternal(Tree.Internal, Tree.Internal.Root);
}

void UKdtreeBPLibrary::DumpActorKdtreeToConsole(const FActorKdtree& Tree)
{
    DumpKdTreeInternal(Tree.Internal, Tree.Internal.Root);
}
