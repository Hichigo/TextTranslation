// Copyright 2021 - Ivan Gordeev


#include "Settings/TextTranslationSettings.h"


UTextTranslationSettings::UTextTranslationSettings(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    GoogleAPI.Endpoint = "https://translation.googleapis.com/language/translate/v2";
}

FString UTextTranslationSettings::GetGoogleEndpoint() const
{
    return GoogleAPI.Endpoint;
}

FString UTextTranslationSettings::GetGoogleKeyAPI() const
{
    return GoogleAPI.KeyAPI;
}
