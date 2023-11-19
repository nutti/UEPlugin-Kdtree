/*!
 * Kdtree
 *
 * Copyright (c) 2019-2023 nutti
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "AsyncKdtreeBPLibrary.h"

#include "Async/AsyncWork.h"
#include "Engine/Engine.h"
#include "KdtreeBPLibrary.h"
#include "KdtreeInternal.h"
#include "Kismet/BlueprintAsyncActionBase.h"

struct FBuildKdtreeTaskParams
{
	FKdtree* Tree;
	TArray<FVector> Data;
};

class FBuildKdtreeTask : public FNonAbandonableTask
{
public:
	FBuildKdtreeTask(const FBuildKdtreeTaskParams& InParams) : Params(InParams)
	{
	}

	void DoWork()
	{
		KdtreeInternal::BuildKdtree(&Params.Tree->Internal, Params.Data);
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FBuildKdtreeTask, STATGROUP_ThreadPoolAsyncTasks);
	}

private:
	FBuildKdtreeTaskParams Params;
};

class FBuildKdtreeAction : public FPendingLatentAction
{
public:
	FLatentActionInfo LatentInfo;
	FAsyncTask<FBuildKdtreeTask>* Task;

	FBuildKdtreeAction(const FLatentActionInfo& InLatentInfo, FKdtree* Tree, const TArray<FVector>& Data)
		: LatentInfo(InLatentInfo), Task(nullptr)
	{
		FBuildKdtreeTaskParams Params;
		Params.Tree = Tree;
		Params.Data = Data;
		Task = new FAsyncTask<FBuildKdtreeTask>(Params);
		Task->StartBackgroundTask();
	}

	void UpdateOperation(FLatentResponse& Response) override
	{
		Response.FinishAndTriggerIf(Task->IsDone(), LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
	}
};

UAsyncKdtreeBPLibrary::UAsyncKdtreeBPLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UAsyncKdtreeBPLibrary::BuildKdtreeAsync(
	const UObject* WorldContextObject, FKdtree& Tree, const TArray<FVector>& Data, FLatentActionInfo LatentInfo)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		FLatentActionManager& LatentManager = World->GetLatentActionManager();
		if (LatentManager.FindExistingAction<FBuildKdtreeAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
		{
			FBuildKdtreeAction* NewAction = new FBuildKdtreeAction(LatentInfo, &Tree, Data);
			LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);
		}
	}
}

struct FCollectFromKdtreeTaskParams
{
	const FKdtree* Tree;
	FVector Center;
	float Radius;
	TArray<int>* Indices;
	TArray<FVector>* Data;
};

class FCollectFromKdtreeTask : public FNonAbandonableTask
{
public:
	FCollectFromKdtreeTask(const FCollectFromKdtreeTaskParams& InParams) : Params(InParams)
	{
	}

	void DoWork()
	{
		KdtreeInternal::CollectFromKdtree(Params.Tree->Internal, Params.Center, Params.Radius, Params.Indices);
		for (int Index = 0; Index < Params.Indices->Num(); ++Index)
		{
			Params.Data->Add(Params.Tree->Internal.Data[(*Params.Indices)[Index]]);
		}
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FCollectFromKdtreeTask, STATGROUP_ThreadPoolAsyncTasks);
	}

private:
	FCollectFromKdtreeTaskParams Params;
};

class FCollectFromKdtreeAction : public FPendingLatentAction
{
public:
	FLatentActionInfo LatentInfo;
	FAsyncTask<FCollectFromKdtreeTask>* Task;

	FCollectFromKdtreeAction(const FLatentActionInfo& InLatentInfo, const FKdtree* Tree, const FVector Center, float Radius,
		TArray<int>* Indices, TArray<FVector>* Data)
		: LatentInfo(InLatentInfo), Task(nullptr)
	{
		FCollectFromKdtreeTaskParams Params;
		Params.Tree = Tree;
		Params.Center = Center;
		Params.Radius = Radius;
		Params.Indices = Indices;
		Params.Data = Data;
		Task = new FAsyncTask<FCollectFromKdtreeTask>(Params);
		Task->StartBackgroundTask();
	}

	void UpdateOperation(FLatentResponse& Response) override
	{
		Response.FinishAndTriggerIf(Task->IsDone(), LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
	}
};

void UAsyncKdtreeBPLibrary::CollectFromKdtreeAsync(const UObject* WorldContextObject, const FKdtree& Tree, const FVector Center,
	float Radius, TArray<int>& Indices, TArray<FVector>& Data, FLatentActionInfo LatentInfo)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		FLatentActionManager& LatentManager = World->GetLatentActionManager();
		if (LatentManager.FindExistingAction<FCollectFromKdtreeAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
		{
			FCollectFromKdtreeAction* NewAction = new FCollectFromKdtreeAction(LatentInfo, &Tree, Center, Radius, &Indices, &Data);
			LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);
		}
	}
}
