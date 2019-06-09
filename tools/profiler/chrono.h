#pragma once

#include <chrono>

#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "chrono.generated.h"


USTRUCT(BlueprintType)
struct FChrono {
    GENERATED_USTRUCT_BODY()
    
    std::chrono::system_clock::time_point tp;
};


UCLASS(BlueprintType, meta = (IsBlueprintBase = true))
class UNAC2_API UChronoAPI : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
private:
    
    UFUNCTION(BluePrintCallable)
    static FChrono GetNowChrono();
    
    UFUNCTION(BluePrintCallable)
    static float GetDiffMicrosecons(const FChrono& c1, const FChrono& c2);
};
