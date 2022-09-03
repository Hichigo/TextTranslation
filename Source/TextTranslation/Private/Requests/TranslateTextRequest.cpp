// Copyright 2021 - Ivan Gordeev


#include "Requests/TranslateTextRequest.h"
#include "TextTranslation/Public/Settings/TextTranslationSettings.h"


UTranslateTextRequest::UTranslateTextRequest(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    Http = &FHttpModule::Get();

    const UTextTranslationSettings* Settings = GetDefault<UTextTranslationSettings>();

    KeyAPI = Settings->GetGoogleKeyAPI();
    Endpoint = Settings->GetGoogleEndpoint();
}

UTranslateTextRequest* UTranslateTextRequest::TranslateTextRequest(const FTranslateTextParams Request, const FString InKeyAPI)
{
    UTranslateTextRequest* Proxy = NewObject<UTranslateTextRequest>();
    Proxy->RequestParams = Request;

    if (!InKeyAPI.IsEmpty())
    {
        Proxy->KeyAPI = InKeyAPI;
    }

    return Proxy;
}

void UTranslateTextRequest::Activate()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = Http->CreateRequest();
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UTranslateTextRequest::OnResponseReceived);

    /*
     * It seems to me ugly to use a URL with a "key" parameter,
     * but I do not know how to use the key correctly
     */
    const FString Key = "?key=" + KeyAPI;
    const FString TranslateURL = Endpoint + Key;

    HttpRequest->SetURL(TranslateURL);
    HttpRequest->SetVerb("POST");
    HttpRequest->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
    HttpRequest->SetHeader(TEXT("Accepts"), TEXT("application/json"));
    HttpRequest->SetContentAsString(RequestParams.GetAsJsonString());
    HttpRequest->ProcessRequest();
}

void UTranslateTextRequest::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful)
{
    FTranslatedTextResponse TranslatedTextResponse;

    const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(
        Response->GetContentAsString()
    );

    const bool bSuccessResponse = Response->GetResponseCode() == 200;

    if (bSuccessResponse)
    {
        TranslatedTextResponse.TranslatedText = ParseJsonTranslatedText(JsonObject, JsonReader);
        UE_LOG(LogTextTranslation, Warning, TEXT("Translate Response: %s"), *Response->GetContentAsString())
    }
    else
    {
        TranslatedTextResponse.Error = ParseJsonTranslateLanguageError(JsonObject, JsonReader);
        UE_LOG(LogTextTranslation, Error, TEXT("Translate Error: %s"), *Response->GetContentAsString())
    }

    OnTranslateTextResponse.Broadcast(bSuccessResponse, TranslatedTextResponse);
}

TArray<FTranslatedText> UTranslateTextRequest::ParseJsonTranslatedText(TSharedPtr<FJsonObject> JsonObject,
    const TSharedRef<TJsonReader<>> JsonReader) const
{
    TArray<FTranslatedText> TranslatedTextArray;

    if (JsonObject.IsValid() && FJsonSerializer::Deserialize(JsonReader, JsonObject))
    {
        const TSharedPtr<FJsonObject> Data = JsonObject->GetObjectField("data");

        TArray<TSharedPtr<FJsonValue>> Translations = Data->GetArrayField("translations");
        for (int32 i = 0; i < Translations.Num(); i++)
        {
            const TSharedPtr<FJsonObject> TranslationItem = Translations[i]->AsObject();

            FTranslatedText TranslatedText;
            TranslatedText.TranslatedText = TranslationItem->GetStringField("translatedText");
            TranslatedText.DetectedSourceLanguage = TranslationItem->GetStringField("detectedSourceLanguage");

            TranslatedTextArray.Push(TranslatedText);
        }
    }

    return TranslatedTextArray;
}

FGoogleTranslateError UTranslateTextRequest::ParseJsonTranslateLanguageError(TSharedPtr<FJsonObject> JsonObject,
    const TSharedRef<TJsonReader<>> JsonReader) const
{
    FGoogleTranslateError Error;

    if (JsonObject.IsValid() && FJsonSerializer::Deserialize(JsonReader, JsonObject))
    {
        const TSharedPtr<FJsonObject> ErrorData = JsonObject->GetObjectField("error");

        Error.Code = ErrorData->GetNumberField("code");
        Error.Message = ErrorData->GetStringField("message");
        Error.Reason = ErrorData->GetStringField("reason");
    }

    return Error;
}
