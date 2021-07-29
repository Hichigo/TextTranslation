// Copyright 2021 - Ivan Gordeev

#pragma once

#include "CoreMinimal.h"
#include "TextTranslationSettings.generated.h"

/**
* 
*/
UCLASS(Config=Engine)
class TEXTTRANSLATION_API UTextTranslationSettings : public UObject
{
    GENERATED_BODY()

public:
    UTextTranslationSettings(const FObjectInitializer& ObjectInitializer);

    FString GetKeyAPI() const;

    FString GetEndpoint() const;

private:
    UPROPERTY(Config, EditAnywhere, Category="Translate Application")
    FString Endpoint;

    UPROPERTY(Config, EditAnywhere, Category="Translate Application")
    FString KeyAPI;
};
