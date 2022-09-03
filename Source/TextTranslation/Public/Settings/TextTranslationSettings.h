// Copyright 2021 - Ivan Gordeev

#pragma once

#include "CoreMinimal.h"
#include "TextTranslationSettings.generated.h"

USTRUCT(BlueprintType)
struct FTranslateAPI
{
    GENERATED_BODY()

    UPROPERTY(Config, EditAnywhere, Category="Translate API")
    FString Endpoint;

    UPROPERTY(Config, EditAnywhere, Category="Translate API")
    FString KeyAPI;
};

UCLASS(Config=Engine)
class TEXTTRANSLATION_API UTextTranslationSettings : public UObject
{
    GENERATED_BODY()

public:
    UTextTranslationSettings(const FObjectInitializer& ObjectInitializer);

    FString GetGoogleKeyAPI() const;
    FString GetGoogleEndpoint() const;

    FString GetYandexKeyAPI() const;
    FString GetYandexEndpoint() const;
    FString GetYandexFolderID() const;

private:
    UPROPERTY(Config, EditAnywhere, Category="Translate Application|Google")
    FTranslateAPI GoogleAPI;

    UPROPERTY(Config, EditAnywhere, Category="Translate Application|Yandex")
    FTranslateAPI YandexAPI;

    UPROPERTY(Config, EditAnywhere, Category="Translate Application|Yandex")
    FString FolderID;
};
