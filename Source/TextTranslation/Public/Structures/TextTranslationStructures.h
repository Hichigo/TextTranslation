// Copyright 2021 - Ivan Gordeev

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"
#include "TextTranslationStructures.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogTextTranslation, Log, All);

USTRUCT(BlueprintType)
struct FDetectLanguageParams
{
    GENERATED_BODY()

public:
    // "q" param from google translate documentation
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    TArray<FString> Text;

    FDetectLanguageParams()
    {
        Text.Empty();
    }

    FString GetAsJsonString() const
    {
        TArray<TSharedPtr<FJsonValue>> Query;

        for (FString Str : Text)
        {
            Query.Add(MakeShareable(new FJsonValueString(Str)));
        }

        TSharedRef<FJsonObject> JsonRootObject = MakeShareable(new FJsonObject);
        JsonRootObject->SetArrayField("q", Query);

        FString ResultJsonString;
        const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResultJsonString);
        FJsonSerializer::Serialize(JsonRootObject, Writer);

        UE_LOG(LogTextTranslation, Warning, TEXT("Request detect language JSON string:\n%s"), *ResultJsonString)

        return ResultJsonString;
    }
};

USTRUCT(BlueprintType)
struct FDetectLanguage
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    float Confidence;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    FString Language;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    bool bIsReliable;

    FDetectLanguage()
    {
        Confidence = -1.0f;
        Language = "None";
        bIsReliable = false;
    }
};

USTRUCT(BlueprintType)
struct FDetectLanguages
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    TArray<FDetectLanguage> DetectedLanguages;

    FDetectLanguages()
    {
        DetectedLanguages.Empty();
    }
};

USTRUCT(BlueprintType)
struct FGoogleTranslateError
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    int32 Code;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    FString Message;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    FString Reason;

    FGoogleTranslateError()
    {
        Code = -1;
        Message = "None";
        Reason = "None";
    }
};

USTRUCT(BlueprintType)
struct FDetectLanguageResponse
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    TArray<FDetectLanguages> Languages;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    FGoogleTranslateError Error;

    FDetectLanguageResponse()
    {
        Languages.Empty();
        Error = FGoogleTranslateError();
    }
};

USTRUCT(BlueprintType)
struct FTranslateTextParams
{
    GENERATED_BODY()

public:
    // "q" param from google translate documentation
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    TArray<FString> Text;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    FString TargetLanguage;

    // "text" or "HTML"
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    FString Format;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    FString SourceLanguage;

    // "nmt" or "base"
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    FString Model;

    FTranslateTextParams()
    {
        Text.Empty();
        TargetLanguage = "";
        Format = "text";
        SourceLanguage = "";
        Model = "";
    }

    FString GetAsJsonString() const
    {
        TArray<TSharedPtr<FJsonValue>> Query;

        for (FString Str : Text)
        {
            Query.Add(MakeShareable(new FJsonValueString(Str)));
        }

        TSharedRef<FJsonObject> JsonRootObject = MakeShareable(new FJsonObject);
        JsonRootObject->SetArrayField("q", Query);
        JsonRootObject->SetStringField("target", TargetLanguage);
        JsonRootObject->SetStringField("format", Format);
        JsonRootObject->SetStringField("source", SourceLanguage);
        JsonRootObject->SetStringField("model", Model);

        FString ResultJsonString;
        const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResultJsonString);
        FJsonSerializer::Serialize(JsonRootObject, Writer);

        UE_LOG(LogTextTranslation, Warning, TEXT("Request translate text JSON string:\n%s"), *ResultJsonString)

        return ResultJsonString;
    }
};

USTRUCT(BlueprintType)
struct FTranslatedText
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    FString TranslatedText;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    FString DetectedSourceLanguage;

    FTranslatedText()
    {
        TranslatedText = "None";
        DetectedSourceLanguage = "";
    }
};

USTRUCT(BlueprintType)
struct FTranslatedTextResponse
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    TArray<FTranslatedText> TranslatedText;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextTranslation")
    FGoogleTranslateError Error;

    FTranslatedTextResponse()
    {
        TranslatedText.Empty();
        Error = FGoogleTranslateError();
    }
};

