// Copyright 2021 - Ivan Gordeev

#pragma once

#include "CoreMinimal.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Structures/TextTranslationStructures.h"
#include "Structures/YTextTranslationStructures.h"
#include "YDetectLanguageRequest.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnYDetectLanguageResponse, bool, bSuccess, const FDetectLanguageResponse&, Response);


UCLASS()
class TEXTTRANSLATION_API UYDetectLanguageRequest : public UOnlineBlueprintCallProxyBase
{
	GENERATED_BODY()

public:
    UYDetectLanguageRequest(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(BlueprintAssignable, Category = "TextTranslation")
    FOnYDetectLanguageResponse OnYDetectLanguageResponse;

    /**
     * @brief 
     * @param Request request for text translation
     * @param InKeyAPI if the API key is specified here, then it will be used in priority, otherwise the key specified in the plugin settings will be used
     * @param InFolderID if the FolderID is specified here, then it will be used in priority, otherwise the key specified in the plugin settings will be used
     * @return UTranslateTextRequest object
     */
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", AdvancedDisplay = "InKeyAPI,InFolderID"), Category = "TextTranslation|Requests")
    static UYDetectLanguageRequest* YDetectLanguageRequest(const FYDetectLanguageParams Request, const FString InKeyAPI, const FString InFolderID);

    virtual void Activate() override;

private:
    UPROPERTY()
    FYDetectLanguageParams RequestParams;

    UPROPERTY()
    FString Endpoint;

    UPROPERTY()
    FString KeyAPI;

    UPROPERTY()
    FString FolderID;

    FHttpModule* Http;

    void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful);

    TArray<FDetectLanguages> ParseJsonDetectLanguage(TSharedPtr<FJsonObject> JsonObject, const TSharedRef<TJsonReader<>> JsonReader) const;

    FGoogleTranslateError ParseJsonDetectLanguageError(TSharedPtr<FJsonObject> JsonObject,
        const TSharedRef<TJsonReader<>> JsonReader) const;
};
