#include "ExpBarWidget.h"

void UExpBarWidget::NativeConstruct()
{
	completionDelegate.BindUFunction(this, FName("SetPercent"));
}

void UExpBarWidget::SetCompletion(double _percent)
{
	completionDelegate.Execute(_percent);
}
