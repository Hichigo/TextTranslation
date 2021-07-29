// Copyright 2021 - Ivan Gordeev


#include "Requests/DetectLanguageRequest.h"
#include "TextTranslation/Public/Settings/TextTranslationSettings.h"

DEFINE_LOG_CATEGORY(LogTextTranslation);

UDetectLanguageRequest::UDetectLanguageRequest(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    Http = &FHttpModule::Get();

    const UTextTranslationSettings* Settings = GetDefault<UTextTranslationSettings>();

    KeyAPI = Settings->GetKeyAPI();
    Endpoint = Settings->GetEndpoint();
}

UDetectLanguageRequest* UDetectLanguageRequest::DetectLanguageRequest(const FDetectLanguageParams Request)
{
    UDetectLanguageRequest* Proxy = NewObject<UDetectLanguageRequest>();
    Proxy->RequestParams = Request;

    return Proxy;
}

void UDetectLanguageRequest::Activate()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = Http->CreateRequest();
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UDetectLanguageRequest::OnResponseReceived);

    /*
    * It seems to me ugly to use a URL with a "key" parameter,
    * but I do not know how to use the key correctly
    */
    const FString Key = "?key=" + KeyAPI;
    const FString DetectLanguageURL = Endpoint + "/detect" + Key;

    HttpRequest->SetURL(DetectLanguageURL);
    HttpRequest->SetVerb("POST");
    HttpRequest->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
    HttpRequest->SetHeader(TEXT("Accepts"), TEXT("application/json"));
    HttpRequest->SetContentAsString(RequestParams.GetAsJsonString());
    HttpRequest->ProcessRequest();
}

void UDetectLanguageRequest::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful)
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

    OnDetectLanguageResponse.Broadcast(bSuccessResponse, DetectLanguageResponse);
}

TArray<FDetectLanguages> UDetectLanguageRequest::ParseJsonDetectLanguage(TSharedPtr<FJsonObject> JsonObject,
    const TSharedRef<TJsonReader<>> JsonReader) const
{
    TArray<FDetectLanguages> DetectionsResponse;

    if (JsonObject.IsValid() && FJsonSerializer::Deserialize(JsonReader, JsonObject))
    {
        const TSharedPtr<FJsonObject> Data = JsonObject->GetObjectField("data");

        TArray<TSharedPtr<FJsonValue>> Detections = Data->GetArrayField("detections");
        for (int32 i = 0; i < Detections.Num(); i++)
        {
            TArray<TSharedPtr<FJsonValue>> InnerDetections = Detections[i]->AsArray();

            FDetectLanguages DetectLanguages;

            for (int32 j = 0; j < InnerDetections.Num(); j++)
            {
                const TSharedPtr<FJsonObject> DetectionItem = InnerDetections[j]->AsObject();

                FDetectLanguage DetectLanguage;
                DetectLanguage.Confidence = DetectionItem->GetNumberField("confidence");
                DetectLanguage.Language = DetectionItem->GetStringField("language");
                DetectLanguage.bIsReliable = DetectionItem->GetBoolField("isReliable");

                DetectLanguages.DetectedLanguages.Push(DetectLanguage);
            }

            DetectionsResponse.Push(DetectLanguages);
        }
    }

    return DetectionsResponse;
}

FGoogleTranslateError UDetectLanguageRequest::ParseJsonDetectLanguageError(TSharedPtr<FJsonObject> JsonObject,
    const TSharedRef<TJsonReader<>> JsonReader) const
{
    FGoogleTranslateError Error;

    if (JsonObject.IsValid() && FJsonSerializer::Deserialize(JsonReader, JsonObject))
    {
        const TSharedPtr<FJsonObject> ErrorData = JsonObject->GetObjectField("error");

        Error.Code = ErrorData->GetNumberField("code");
        Error.Message = ErrorData->GetStringField("message");
        Error.Status = ErrorData->GetStringField("status");
    }

    return Error;
}
