// Copyright 2021 - Ivan Gordeev

#pragma once

#include "CoreMinimal.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Structures/TextTranslationStructures.h"
#include "TranslateTextRequest.generated.h"

/**
* 
*/

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTranslateTextResponse, bool, bSuccess, const FTranslatedTextResponse&, Response);

UCLASS()
class TEXTTRANSLATION_API UTranslateTextRequest : public UOnlineBlueprintCallProxyBase
{
    GENERATED_BODY()

public:
    UTranslateTextRequest(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(BlueprintAssignable, Category = "GoogleTranslate")
    FOnTranslateTextResponse OnTranslateTextResponse;

    /**
     * @brief 
     * @param Request request for text translation
     * @param InKeyAPI if the API key is specified here, then it will be used in priority, otherwise the key specified in the plugin settings will be used
     * @return UTranslateTextRequest object
     */
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", AdvancedDisplay = "InKeyAPI"), Category = "GoogleTranslate|Requests")
    static UTranslateTextRequest* TranslateTextRequest(const FTranslateTextParams Request, const FString InKeyAPI);

    virtual void Activate() override;

private:
    UPROPERTY()
    FTranslateTextParams RequestParams;

    UPROPERTY()
    FString TargetLanguage;

    UPROPERTY()
    FString Endpoint;

    UPROPERTY()
    FString KeyAPI;

    FHttpModule* Http;

    void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful);

    TArray<FTranslatedText> ParseJsonTranslatedText(TSharedPtr<FJsonObject> JsonObject, const TSharedRef<TJsonReader<>> JsonReader) const;

    FGoogleTranslateError ParseJsonTranslateLanguageError(TSharedPtr<FJsonObject> JsonObject, const TSharedRef<TJsonReader<>> JsonReader) const;
};
