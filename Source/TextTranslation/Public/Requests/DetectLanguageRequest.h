// Copyright 2021 - Ivan Gordeev

#pragma once

#include "CoreMinimal.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Structures/TextTranslationStructures.h"
#include "DetectLanguageRequest.generated.h"

/**
* 
*/

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDetectLanguageResponse, bool, bSuccess, const FDetectLanguageResponse&, Response);

UCLASS()
class TEXTTRANSLATION_API UDetectLanguageRequest : public UOnlineBlueprintCallProxyBase
{
    GENERATED_BODY()

public:
    UDetectLanguageRequest(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(BlueprintAssignable, Category = "TextTranslation")
    FOnDetectLanguageResponse OnDetectLanguageResponse;

    /**
     * @brief 
     * @param Request request for text translation
     * @param InKeyAPI if the API key is specified here, then it will be used in priority, otherwise the key specified in the plugin settings will be used
     * @return UTranslateTextRequest object
     */
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", AdvancedDisplay = "InKeyAPI"), Category = "TextTranslation|Requests")
    static UDetectLanguageRequest* DetectLanguageRequest(const FDetectLanguageParams Request, const FString InKeyAPI);

    virtual void Activate() override;

private:
    UPROPERTY()
    FDetectLanguageParams RequestParams;

    UPROPERTY()
    FString Endpoint;

    UPROPERTY()
    FString KeyAPI;

    FHttpModule* Http;

    void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful);

    TArray<FDetectLanguages> ParseJsonDetectLanguage(TSharedPtr<FJsonObject> JsonObject, const TSharedRef<TJsonReader<>> JsonReader) const;

    FGoogleTranslateError ParseJsonDetectLanguageError(TSharedPtr<FJsonObject> JsonObject,
        const TSharedRef<TJsonReader<>> JsonReader) const;
};
