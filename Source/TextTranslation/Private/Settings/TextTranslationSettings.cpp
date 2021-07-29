// Copyright 2021 - Ivan Gordeev


#include "Settings/TextTranslationSettings.h"


UTextTranslationSettings::UTextTranslationSettings(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer),
      Endpoint("https://translation.googleapis.com/language/translate/v2")
{
}

FString UTextTranslationSettings::GetEndpoint() const
{
    return Endpoint;
}

FString UTextTranslationSettings::GetKeyAPI() const
{
    return KeyAPI;
}
