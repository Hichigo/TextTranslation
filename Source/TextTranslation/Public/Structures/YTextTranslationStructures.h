// Copyright 2021 - Ivan Gordeev

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"
#include "YTextTranslationStructures.generated.h"


USTRUCT(BlueprintType)
struct FYTranslateTextParams
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "YTextTranslation")
    TArray<FString> Text;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "YTextTranslation")
    FString TargetLanguage;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "YTextTranslation")
    FString FolderID;

    FYTranslateTextParams()
    {
        Text.Empty();
        TargetLanguage = "";
        FolderID = "";
    }

    FString GetAsJsonString() const
    {
        TArray<TSharedPtr<FJsonValue>> Query;

        for (FString Str : Text)
        {
            Query.Add(MakeShareable(new FJsonValueString(Str)));
        }

        TSharedRef<FJsonObject> JsonRootObject = MakeShareable(new FJsonObject);
        JsonRootObject->SetArrayField("texts", Query);
        JsonRootObject->SetStringField("targetLanguageCode", TargetLanguage);
        JsonRootObject->SetStringField("folderId", FolderID);

        FString ResultJsonString;
        const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResultJsonString);
        FJsonSerializer::Serialize(JsonRootObject, Writer);

        UE_LOG(LogTextTranslation, Warning, TEXT("Request translate text JSON string:\n%s"), *ResultJsonString)

        return ResultJsonString;
    }
};

USTRUCT(BlueprintType)
struct FYDetectLanguageParams
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    FString Text;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "YTextTranslation")
    FString FolderID;

    FYDetectLanguageParams()
    {
        Text = "";
        FolderID = "";
    }

    FString GetAsJsonString() const
    {
        TSharedRef<FJsonObject> JsonRootObject = MakeShareable(new FJsonObject);
        JsonRootObject->SetStringField("text", Text);

        FString ResultJsonString;
        const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResultJsonString);
        FJsonSerializer::Serialize(JsonRootObject, Writer);

        UE_LOG(LogTextTranslation, Warning, TEXT("Request detect language JSON string:\n%s"), *ResultJsonString)

        return ResultJsonString;
    }
};