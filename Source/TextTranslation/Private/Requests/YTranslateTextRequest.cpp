// Copyright 2021 - Ivan Gordeev


#include "Requests/YTranslateTextRequest.h"
#include "TextTranslation/Public/Settings/TextTranslationSettings.h"

UYTranslateTextRequest::UYTranslateTextRequest(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    Http = &FHttpModule::Get();

    const UTextTranslationSettings* Settings = GetDefault<UTextTranslationSettings>();

    KeyAPI = Settings->GetYandexKeyAPI();
    Endpoint = Settings->GetYandexEndpoint();
    FolderID = Settings->GetYandexFolderID();
}

UYTranslateTextRequest* UYTranslateTextRequest::YTranslateTextRequest(const FYTranslateTextParams Request, const FString InKeyAPI, const FString InFolderID)
{
    UYTranslateTextRequest* Proxy = NewObject<UYTranslateTextRequest>();
    Proxy->RequestParams = Request;

    if (!InKeyAPI.IsEmpty())
    {
        Proxy->KeyAPI = InKeyAPI;
    }

    if (!InFolderID.IsEmpty())
    {
        Proxy->FolderID = InFolderID;
    }

    return Proxy;
}

void UYTranslateTextRequest::Activate()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = Http->CreateRequest();
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UYTranslateTextRequest::OnResponseReceived);

    RequestParams.FolderID = FolderID;

    const FString Auth = FString::Printf(TEXT("Api-Key %s"), *KeyAPI);
    const FString TranslateURL = FString::Printf(TEXT("%s/translate"), *Endpoint);
    
    HttpRequest->SetURL(TranslateURL);
    HttpRequest->SetVerb("POST");
    HttpRequest->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
    HttpRequest->SetHeader(TEXT("Authorization"), Auth);
    HttpRequest->SetContentAsString(RequestParams.GetAsJsonString());
    HttpRequest->ProcessRequest();
}

void UYTranslateTextRequest::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful)
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
        UE_LOG(LogTextTranslation, Error, TEXT("Translate Error %s"), *Response->GetContentAsString())
    }

    OnYTranslateTextResponse.Broadcast(bSuccessResponse, TranslatedTextResponse);
}

TArray<FTranslatedText> UYTranslateTextRequest::ParseJsonTranslatedText(TSharedPtr<FJsonObject> JsonObject,
    const TSharedRef<TJsonReader<>> JsonReader) const
{
    TArray<FTranslatedText> TranslatedTextArray;

    if (JsonObject.IsValid() && FJsonSerializer::Deserialize(JsonReader, JsonObject))
    {
        TArray<TSharedPtr<FJsonValue>> Translations = JsonObject->GetArrayField("translations");
        for (int32 i = 0; i < Translations.Num(); i++)
        {
            const TSharedPtr<FJsonObject> TranslationItem = Translations[i]->AsObject();

            FTranslatedText TranslatedText;
            TranslatedText.TranslatedText = TranslationItem->GetStringField("text");
            TranslatedText.DetectedSourceLanguage = TranslationItem->GetStringField("detectedLanguageCode");

            TranslatedTextArray.Push(TranslatedText);
        }
    }

    return TranslatedTextArray;
}

FGoogleTranslateError UYTranslateTextRequest::ParseJsonTranslateLanguageError(TSharedPtr<FJsonObject> JsonObject,
    const TSharedRef<TJsonReader<>> JsonReader) const
{
    FGoogleTranslateError Error;

    if (JsonObject.IsValid() && FJsonSerializer::Deserialize(JsonReader, JsonObject))
    {
        Error.Code = JsonObject->GetIntegerField("code");
        Error.Message = JsonObject->GetStringField("message");
    }

    return Error;
}
