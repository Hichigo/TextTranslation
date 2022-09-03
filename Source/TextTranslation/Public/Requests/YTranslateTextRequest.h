// Copyright 2021 - Ivan Gordeev

#pragma once

#include "CoreMinimal.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Structures/TextTranslationStructures.h"
#include "Structures/YTextTranslationStructures.h"
#include "YTranslateTextRequest.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnYTranslateTextResponse, bool, bSuccess, const FTranslatedTextResponse&, Response);

UCLASS()
class TEXTTRANSLATION_API UYTranslateTextRequest : public UOnlineBlueprintCallProxyBase
{
	GENERATED_BODY()

public:
    UYTranslateTextRequest(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(BlueprintAssignable, Category = "YandexTranslate")
    FOnYTranslateTextResponse OnYTranslateTextResponse;

    /**
     * @brief 
     * @param Request request for text translation
     * @param InKeyAPI if the API key is specified here, then it will be used in priority, otherwise the key specified in the plugin settings will be used
     * @param InFolderID if the FolderID is specified here, then it will be used in priority, otherwise the key specified in the plugin settings will be used
     * @return UTranslateTextRequest object
     */
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", AdvancedDisplay = "InKeyAPI,InFolderID"), Category = "YandexTranslate|Requests")
    static UYTranslateTextRequest* YTranslateTextRequest(const FYTranslateTextParams Request, const FString InKeyAPI, const FString InFolderID);

    virtual void Activate() override;

private:
    UPROPERTY()
    FYTranslateTextParams RequestParams;

    UPROPERTY()
    FString TargetLanguage;

    UPROPERTY()
    FString Endpoint;

    UPROPERTY()
    FString KeyAPI;

    UPROPERTY()
    FString FolderID;

    FHttpModule* Http;

    void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful);

    TArray<FTranslatedText> ParseJsonTranslatedText(TSharedPtr<FJsonObject> JsonObject, const TSharedRef<TJsonReader<>> JsonReader) const;

    FGoogleTranslateError ParseJsonTranslateLanguageError(TSharedPtr<FJsonObject> JsonObject, const TSharedRef<TJsonReader<>> JsonReader) const;
};
