

#include "include\PerkEntryPointExtenderAPI.h"

struct Hooks {
        struct ShoutRecoveryHook {
        static void thunk(RE::AIProcess* a_AP, float RecoveryTime) {

            auto a_actor = a_AP->GetUserData();            
            float f_mult = a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kShoutRecoveryMult);  
            float basetime = 1.0f;
            float shoutrecoveryfactor = 100.0f;
            float shoutrecoverycap = 20.0f;
            float shoutrecoverymultiplier = 100.0f;
            
            if (a_AP->high->currentShoutVariation > 3) {
                basetime = a_AP->high->currentShout->variations[0].recoveryTime;
                RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a_actor, &shoutrecoveryfactor, "ShoutRecovery",
                                     2, {a_AP->high->currentShout->variations[0].spell});
                RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a_actor, &shoutrecoverycap, "ShoutRecovery",
                                     3, {a_AP->high->currentShout->variations[0].spell});
                RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a_actor, &shoutrecoverymultiplier,
                                     "ShoutRecovery",
                                     4, {a_AP->high->currentShout->variations[0].spell});
            } else {
                basetime = a_AP->high->currentShout->variations[a_AP->high->currentShoutVariation].recoveryTime;
                RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a_actor, &shoutrecoveryfactor, "ShoutRecovery",
                                     2,
                                     {a_AP->high->currentShout->variations[a_AP->high->currentShoutVariation].spell});
                RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a_actor, &shoutrecoverycap, "ShoutRecovery",
                                     3,
                                     {a_AP->high->currentShout->variations[a_AP->high->currentShoutVariation].spell});
                RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a_actor, &shoutrecoverymultiplier,
                                     "ShoutRecovery",
                                     4,
                                     {a_AP->high->currentShout->variations[a_AP->high->currentShoutVariation].spell});
            }
            float cappedshoutrecovery = (std::max(shoutrecoverycap, (f_mult*100.0f+shoutrecoveryfactor-100.0f)))/100.0f;
            RecoveryTime = basetime * shoutrecoverymultiplier * cappedshoutrecovery / 100.0f;

            func(a_AP, RecoveryTime);

        }
        static inline REL::Relocation<decltype(thunk)> func;
        };

    struct ShoutRecoveryMenuHook {
        static bool thunk(RE::GFxValue::ObjectInterface* a_start, void* a_data, const char* a_name,
                          const RE::GFxValue& a_value, bool a_isDObj) {
            auto a_actor = RE::PlayerCharacter::GetSingleton();
            float f_mult = a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kShoutRecoveryMult);

            RE::AIProcess* a_AP = a_actor->GetActorRuntimeData().currentProcess;
            RE::GFxValue selectedIndex, categoryList, inventoryLists;
            uint32_t menu_form = 0;
            auto* a_ui = RE::UI::GetSingleton();
            if (a_ui->IsMenuOpen(RE::MagicMenu::MENU_NAME)) {
                RE::ConsoleLog::GetSingleton()->Print("beep1");
                auto* magic_menu = static_cast<RE::MagicMenu*>(a_ui->GetMenu(RE::MagicMenu::MENU_NAME).get());
                if (magic_menu) {
                    RE::ConsoleLog::GetSingleton()->Print("beep2");
                    RE::GFxValue result;
                    magic_menu->uiMovie->GetVariable(&result,
                                                     "_root.Menu_mc.inventoryLists.itemList.selectedEntry.formId");
                    if (result.GetType() == RE::GFxValue::ValueType::kNumber) {
                        RE::ConsoleLog::GetSingleton()->Print("beep3");
                        menu_form = static_cast<std::uint32_t>(result.GetNumber());
                    }
                }
            }
            if (menu_form) {
                auto a_Shout = RE::TESForm::LookupByID(menu_form)->As<RE::TESShout>();
                if (a_Shout) {
                    float basetime = 1.0f;
                    float shoutrecoveryfactor = 0.0f;
                    float shoutrecoverycap = 0.0f;
                    float shoutrecoverymultiplier = 100.0f;

                    RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a_actor, &shoutrecoveryfactor,
                                         "ShoutRecovery", 2, {a_Shout->variations[0].spell});
                    RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a_actor, &shoutrecoverycap, "ShoutRecovery",
                                         3, {a_Shout->variations[0].spell});
                    RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a_actor, &shoutrecoverymultiplier,
                                         "ShoutRecovery", 4, {a_Shout->variations[0].spell});

                    basetime = a_Shout->variations[0].recoveryTime;
                    float cappedshoutrecovery =
                        (std::max(shoutrecoverycap, (f_mult * 100.0f + shoutrecoveryfactor))) / 100.0f;
                    float RecoveryTime1 = basetime * shoutrecoverymultiplier * cappedshoutrecovery / 100.0f;

                    shoutrecoveryfactor = 0.0f;
                    shoutrecoverycap = 0.0f;
                    shoutrecoverymultiplier = 100.0f;

                    RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a_actor, &shoutrecoveryfactor,
                                         "ShoutRecovery", 2, {a_Shout->variations[1].spell});
                    RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a_actor, &shoutrecoverycap, "ShoutRecovery",
                                         3, {a_Shout->variations[1].spell});

                    RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a_actor, &shoutrecoverymultiplier,
                                         "ShoutRecovery", 4, {a_Shout->variations[1].spell});
                    basetime = a_Shout->variations[1].recoveryTime;
                    cappedshoutrecovery =
                        (std::max(shoutrecoverycap, (f_mult * 100.0f + shoutrecoveryfactor))) / 100.0f;
                    float RecoveryTime2 = basetime * shoutrecoverymultiplier * cappedshoutrecovery / 100.0f;

                    shoutrecoveryfactor = 0.0f;
                    shoutrecoverycap = 0.0f;
                    shoutrecoverymultiplier = 100.0f;

                    RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a_actor, &shoutrecoveryfactor,
                                         "ShoutRecovery", 2, {a_Shout->variations[2].spell});
                    RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a_actor, &shoutrecoverycap, "ShoutRecovery",
                                         3, {a_Shout->variations[2].spell});

                    RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a_actor, &shoutrecoverymultiplier,
                                         "ShoutRecovery", 4, {a_Shout->variations[2].spell});
                    basetime = a_Shout->variations[2].recoveryTime;
                    cappedshoutrecovery =
                        (std::max(shoutrecoverycap, (f_mult * 100.0f + shoutrecoveryfactor))) / 100.0f;
                    float RecoveryTime3 = basetime * shoutrecoverymultiplier * cappedshoutrecovery / 100.0f;

                    char buffer[80];
                    sprintf_s(buffer, "%.0f, %.0f, %.0f", RecoveryTime1, RecoveryTime2, RecoveryTime3);
                    auto a_updatedvalue = const_cast<RE::GFxValue&>(a_value);
                    a_updatedvalue.SetString(buffer);

                    return func(a_start, a_data, a_name, a_updatedvalue, a_isDObj);

                } else {
                    return func(a_start, a_data, a_name, a_value, a_isDObj);
                }
            }
            return func(a_start, a_data, a_name, a_value, a_isDObj);
        }
        static inline REL::Relocation<decltype(thunk)> func;
    };



    static void Install() {
        REL::Relocation<std::uintptr_t> functionA{RELOCATION_ID(41737, 42818), REL::Relocate(0xC3, 0xC3)};
        stl::write_thunk_call<ShoutRecoveryHook>(functionA.address());

        REL::Relocation<std::uintptr_t> functionB{RELOCATION_ID(51020, 51898), REL::Relocate(0x459, 0x45B)};
        stl::write_thunk_call<ShoutRecoveryMenuHook>(functionB.address());

    }
};

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    Hooks::Install();
    return true;
}