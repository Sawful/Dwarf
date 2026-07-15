#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;

UCLASS()
class DWARF_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* IdleButton;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* RunButton;
	virtual void NativeConstruct() override;
};
