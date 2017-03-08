#pragma once

#include "TemplateData.generated.h"

// the role of the player
UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EPlayerState : uint8
{
	None		UMETA(DisplayName = "None"),
	Idle 		UMETA(DisplayName = "Idle"),
	Sprinting 	UMETA(DisplayName = "Sprinting"),
	walking		UMETA(DisplayName = "walking"),
	Zooming		UMETA(DisplayName = "Zooming"),
	Crouching	UMETA(DisplayName = "Crouching"),
};