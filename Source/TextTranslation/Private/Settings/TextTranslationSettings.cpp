// Copyright 2021 - Ivan Gordeev


#include "Settings/TextTranslationSettings.h"


UTextTranslationSettings::UTextTranslationSettings(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    GoogleAPI.Endpoint = "https://translation.googleapis.com/language/translate/v2";
    YandexAPI.Endpoint = "https://translate.api.cloud.yandex.net/translate/v2";
}

FString UTextTranslationSettings::GetGoogleKeyAPI() const
{
    return GoogleAPI.KeyAPI;
}

FString UTextTranslationSettings::GetGoogleEndpoint() const
{
    return GoogleAPI.Endpoint;
}

FString UTextTranslationSettings::GetYandexKeyAPI() const
{
    return YandexAPI.KeyAPI;
}

FString UTextTranslationSettings::GetYandexEndpoint() const
{
    return YandexAPI.Endpoint;
}

FString UTextTranslationSettings::GetYandexFolderID() const
{
    return FolderID;
}
