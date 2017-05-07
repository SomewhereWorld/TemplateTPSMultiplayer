#pragma once

#include "TemplateData.generated.h"

// the state of the player
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

// the power of the player
UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EPlayerPower : uint8
{
	None		UMETA(DisplayName = "None"),
	Sprinter 	UMETA(DisplayName = "Sprinter"),
	Tank 		UMETA(DisplayName = "Tank"),
	MarksMan	UMETA(DisplayName = "DPS"),
};

// the state of the client
UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EClientState : uint8
{
	None		UMETA(DisplayName = "None"),
	Alive 		UMETA(DisplayName = "Alive"),
	Dead 		UMETA(DisplayName = "Dead"),
	Stuck		UMETA(DisplayName = "Stuck"),
};

USTRUCT(BlueprintType)
struct FLobbyControllerInfos
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template Data")
	FString name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template Data")
	int indexPos; // the position of the player in the team tab (-1,0,1)

	FLobbyControllerInfos()
	{
		name = "";
		indexPos = 0;
	}
};