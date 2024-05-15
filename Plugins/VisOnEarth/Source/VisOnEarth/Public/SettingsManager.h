#pragma once

#include "FlowFieldSettings.h"

// 在一个公共类或管理器中定义事件
DECLARE_DELEGATE_OneParam(FOnSettingsUpdatedDelegate, const FFlowFieldSettings&);

class SettingsManager
{
public:
	FOnSettingsUpdatedDelegate OnSettingsUpdated;

	void UpdateSettings(const FFlowFieldSettings& NewSettings)
	{
		OnSettingsUpdated.ExecuteIfBound(NewSettings);
	}
};



