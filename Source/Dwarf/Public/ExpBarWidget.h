#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExpBarWidget.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSetCompletion, double, Percentage);

UCLASS()
class DWARF_API UExpBarWidget : public UUserWidget
{
	GENERATED_BODY()

	UFUNCTION()
	virtual void NativeConstruct() override;

public:
	UPROPERTY()
	FOnSetCompletion completionDelegate;

	UFUNCTION()
	void SetCompletion(double _percent);
};
