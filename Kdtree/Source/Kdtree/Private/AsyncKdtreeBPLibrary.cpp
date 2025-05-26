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

struct FCollectFromKdtreeSphereTaskParams
{
	const FKdtree* Tree;
	FVector Center;
	float Radius;
	TArray<int>* Indices;
	TArray<FVector>* Data;
};

class FCollectFromKdtreeSphereTask : public FNonAbandonableTask
{
public:
	FCollectFromKdtreeSphereTask(const FCollectFromKdtreeSphereTaskParams& InParams) : Params(InParams)
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
		RETURN_QUICK_DECLARE_CYCLE_STAT(FCollectFromKdtreeSphereTask, STATGROUP_ThreadPoolAsyncTasks);
	}

private:
	FCollectFromKdtreeSphereTaskParams Params;
};

class FCollectFromKdtreeSphereAction : public FPendingLatentAction
{
public:
	FLatentActionInfo LatentInfo;
	FAsyncTask<FCollectFromKdtreeSphereTask>* Task;

	FCollectFromKdtreeSphereAction(const FLatentActionInfo& InLatentInfo, const FKdtree* Tree, const FVector Center, float Radius,
		TArray<int>* Indices, TArray<FVector>* Data)
		: LatentInfo(InLatentInfo), Task(nullptr)
	{
		FCollectFromKdtreeSphereTaskParams Params;
		Params.Tree = Tree;
		Params.Center = Center;
		Params.Radius = Radius;
		Params.Indices = Indices;
		Params.Data = Data;
		Task = new FAsyncTask<FCollectFromKdtreeSphereTask>(Params);
		Task->StartBackgroundTask();
	}

	void UpdateOperation(FLatentResponse& Response) override
	{
		Response.FinishAndTriggerIf(Task->IsDone(), LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
	}
};

void UAsyncKdtreeBPLibrary::CollectFromKdtreeSphereAsync(const UObject* WorldContextObject, const FKdtree& Tree,
	const FVector Center, float Radius, TArray<int>& Indices, TArray<FVector>& Data, FLatentActionInfo LatentInfo)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		FLatentActionManager& LatentManager = World->GetLatentActionManager();
		if (LatentManager.FindExistingAction<FCollectFromKdtreeSphereAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
		{
			FCollectFromKdtreeSphereAction* NewAction =
				new FCollectFromKdtreeSphereAction(LatentInfo, &Tree, Center, Radius, &Indices, &Data);
			LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);
		}
	}
}

struct FCollectFromKdtreeBoxTaskParams
{
	const FKdtree* Tree;
	FBox Box;
	TArray<int>* Indices;
	TArray<FVector>* Data;
};

class FCollectFromKdtreeBoxTask : public FNonAbandonableTask
{
public:
	FCollectFromKdtreeBoxTask(const FCollectFromKdtreeBoxTaskParams& InParams) : Params(InParams)
	{
	}

	void DoWork()
	{
		KdtreeInternal::CollectFromKdtree(Params.Tree->Internal, Params.Box, Params.Indices);
		for (int Index = 0; Index < Params.Indices->Num(); ++Index)
		{
			Params.Data->Add(Params.Tree->Internal.Data[(*Params.Indices)[Index]]);
		}
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FCollectFromKdtreeBoxTask, STATGROUP_ThreadPoolAsyncTasks);
	}

private:
	FCollectFromKdtreeBoxTaskParams Params;
};

class FCollectFromKdtreeBoxAction : public FPendingLatentAction
{
public:
	FLatentActionInfo LatentInfo;
	FAsyncTask<FCollectFromKdtreeBoxTask>* Task;

	FCollectFromKdtreeBoxAction(
		const FLatentActionInfo& InLatentInfo, const FKdtree* Tree, const FBox Box, TArray<int>* Indices, TArray<FVector>* Data)
		: LatentInfo(InLatentInfo), Task(nullptr)
	{
		FCollectFromKdtreeBoxTaskParams Params;
		Params.Tree = Tree;
		Params.Box = Box;
		Params.Indices = Indices;
		Params.Data = Data;
		Task = new FAsyncTask<FCollectFromKdtreeBoxTask>(Params);
		Task->StartBackgroundTask();
	}

	void UpdateOperation(FLatentResponse& Response) override
	{
		Response.FinishAndTriggerIf(Task->IsDone(), LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
	}
};

void UAsyncKdtreeBPLibrary::CollectFromKdtreeBoxAsync(const UObject* WorldContextObject, const FKdtree& Tree, const FBox Box,
	TArray<int>& Indices, TArray<FVector>& Data, FLatentActionInfo LatentInfo)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		FLatentActionManager& LatentManager = World->GetLatentActionManager();
		if (LatentManager.FindExistingAction<FCollectFromKdtreeBoxAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
		{
			FCollectFromKdtreeBoxAction* NewAction = new FCollectFromKdtreeBoxAction(LatentInfo, &Tree, Box, &Indices, &Data);
			LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);
		}
	}
}
