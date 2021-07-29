// Copyright 2021 - Ivan Gordeev

#include "TextTranslation.h"

#include "TextTranslation/Public/Settings/TextTranslationSettings.h"

#if WITH_EDITOR
    #include "ISettingsModule.h"
    #include "ISettingsSection.h"
#endif


#define LOCTEXT_NAMESPACE "FGoogleTranslateModule"

void FTextTranslationModule::StartupModule()
{

#if WITH_EDITOR
    // register settings
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

    if (SettingsModule != nullptr)
    {
        ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "TextTranslation",
            LOCTEXT("TcpMessagingSettingsName", "Text Translation"),
            LOCTEXT("TcpMessagingSettingsDescription", "Configure the Text Translation plug-in."),
            GetMutableDefault<UTextTranslationSettings>()
        );

        if (SettingsSection.IsValid())
        {
            SettingsSection->OnModified().BindRaw(this, &FTextTranslationModule::HandleSettingsSaved);
        }
    }
#endif // WITH_EDITOR
}

void FTextTranslationModule::ShutdownModule()
{
#if WITH_EDITOR
    // unregister settings
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

    if (SettingsModule != nullptr)
    {
        SettingsModule->UnregisterSettings("Project", "Plugins", "TextTranslation");
    }
#endif
}

bool FTextTranslationModule::HandleSettingsSaved()
{
    return true;
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTextTranslationModule, TextTranslation)