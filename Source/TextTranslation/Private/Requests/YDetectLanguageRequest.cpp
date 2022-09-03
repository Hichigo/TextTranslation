// Copyright 2021 - Ivan Gordeev


#include "Requests/YDetectLanguageRequest.h"
#include "TextTranslation/Public/Settings/TextTranslationSettings.h"

UYDetectLanguageRequest::UYDetectLanguageRequest(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    Http = &FHttpModule::Get();

    const UTextTranslationSettings* Settings = GetDefault<UTextTranslationSettings>();

    KeyAPI = Settings->GetYandexKeyAPI();
    Endpoint = Settings->GetYandexEndpoint();
    FolderID = Settings->GetYandexFolderID();
}

UYDetectLanguageRequest* UYDetectLanguageRequest::YDetectLanguageRequest(const FYDetectLanguageParams Request, const FString InKeyAPI,
    const FString InFolderID)
{
    UYDetectLanguageRequest* Proxy = NewObject<UYDetectLanguageRequest>();
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

void UYDetectLanguageRequest::Activate()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = Http->CreateRequest();
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UYDetectLanguageRequest::OnResponseReceived);

    RequestParams.FolderID = FolderID;

    const FString Auth = FString::Printf(TEXT("Api-Key %s"), *KeyAPI);
    const FString DetectURL = FString::Printf(TEXT("%s/detect"), *Endpoint);

    HttpRequest->SetURL(DetectURL);
    HttpRequest->SetVerb("POST");
    HttpRequest->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
    HttpRequest->SetHeader(TEXT("Authorization"), Auth);
    HttpRequest->SetContentAsString(RequestParams.GetAsJsonString());
    HttpRequest->ProcessRequest();
}

void UYDetectLanguageRequest::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful)
{
    FDetectLanguageResponse DetectLanguageResponse;

    const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(
        Response->GetContentAsString()
        );

    const bool bSuccessResponse = Response->GetResponseCode() == 200;

    if (bSuccessResponse)
    {
        DetectLanguageResponse.Languages = ParseJsonDetectLanguage(JsonObject, JsonReader);
        UE_LOG(LogTextTranslation, Warning, TEXT("Detect Language Response: %s"), *Response->GetContentAsString())
    }
    else
    {
        DetectLanguageResponse.Error = ParseJsonDetectLanguageError(JsonObject, JsonReader);
        UE_LOG(LogTextTranslation, Error, TEXT("Detect Language Error: %s"), *Response->GetContentAsString())
    }

    OnYDetectLanguageResponse.Broadcast(bSuccessResponse, DetectLanguageResponse);
}

TArray<FDetectLanguages> UYDetectLanguageRequest::ParseJsonDetectLanguage(TSharedPtr<FJsonObject> JsonObject,
    const TSharedRef<TJsonReader<>> JsonReader) const
{
    TArray<FDetectLanguages> DetectionsResponse;

    if (JsonObject.IsValid() && FJsonSerializer::Deserialize(JsonReader, JsonObject))
    {
        FDetectLanguage DetectLanguage;
        DetectLanguage.Language = JsonObject->GetStringField("languageCode");

        FDetectLanguages DetectLanguages;
        DetectLanguages.DetectedLanguages.Push(DetectLanguage);

        DetectionsResponse.Push(DetectLanguages);
    }

    return DetectionsResponse;
}

FGoogleTranslateError UYDetectLanguageRequest::ParseJsonDetectLanguageError(TSharedPtr<FJsonObject> JsonObject,
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
