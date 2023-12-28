

#include "include\PerkEntryPointExtenderAPI.h"

float GetEffectiveCastSpeed(RE::ActorMagicCaster* a_AMC, RE::Actor* a,float perkfactor, int castingsource) {

        float workingperkfactor = perkfactor;
        RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a, &workingperkfactor, "CastTime", 2, {a_AMC->currentSpell}); // Channel 2 will be the all purpose channel for everything

        if (castingsource == 2) {
            RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a, &workingperkfactor, "CastTime", 3, {a_AMC->currentSpell}); // Channel 3 will be the Right Hand Channel
        } else if (castingsource == 1) {
            RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a, &workingperkfactor, "CastTime", 4, {a_AMC->currentSpell}); // Channel 4 will be the Left Hand Channel
        }

        return workingperkfactor;

}

float GetEffectiveDualCast(RE::ActorMagicCaster* a_AMC, RE::Actor* a, float perkfactor, float castingsource) {
        float workingperkfactor = perkfactor;
        RE::MagicItem* currentspell = a_AMC->currentSpell;
        if (castingsource == 2) {
            if (!a_AMC->currentSpell) {
                 currentspell = a->GetActorRuntimeData().selectedSpells[1];
            }
            RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a, &workingperkfactor, "DualCast", 3,
                                 {currentspell});  // Channel 3 will be the Right Hand Channel
        } else if (castingsource == 1) {
            if (!a_AMC->currentSpell) {
                 currentspell = a->GetActorRuntimeData().selectedSpells[0];
            }
            RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a, &workingperkfactor, "DualCast", 4,
                                 {currentspell});  // Channel 4 will be the Left Hand Channel
        }
        RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a, &workingperkfactor, "DualCast", 2,
            {currentspell});  // Channel 2 will be the all purpose channel for everything
        return workingperkfactor;
}
//
//void EvaluateDynamicMGEFs(RE::Actor* a_Actor) {
//RE::SendHUDMessage

//        auto ActiveEffects = a_Actor->AsMagicTarget()->GetActiveEffectList();
//        float updatefactor = 1.0f;
//        if (ActiveEffects) {
//            for (const auto& effect : *ActiveEffects) {
//                const auto& setting = effect ? effect->GetBaseObject() : nullptr;
//
//                if (setting) {
//                    //            logger::info("The effect we're checking is {}. The effect we're looking for is {}.",
//                    //            setting->formID,
//                    //                         akMagicEffect->formID);
//                    if (setting->HasKeyword("ASR_TestKeyword")) {
//                        updatefactor = 1.0f;
//                        //updatefactor = std::bit_cast<float>(effect->pad8C);
//                        //float basemagnitude = effect->effect->GetMagnitude();
//
//                        RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a_Actor, &updatefactor,
//                                             "DynamicMagnitude", 2,
//                                             {effect->spell});
//
//                        //float oldupdatefactor = std::bit_cast<float>(effect->pad8C);
//
//                        //effect->pad8C = updatefactor; 
//
//
//                        effect->magnitude = effect->magnitude * updatefactor;
//                    }
//                } 
//            }
//        }
//}




struct Hooks {
        struct Update {
        static void thunk(RE::ActorMagicCaster* a_AMC, float a_deltatime) {


            float updatedtime = a_deltatime;
            auto a = a_AMC->GetCasterAsActor();
            auto source = a_AMC->GetCastingSource();
            float perkfactor = 1.00f;

            if (source == RE::MagicSystem::CastingSource::kRightHand && (a_AMC->state.get() == RE::MagicCaster::State::kUnk02)) {

                //RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a, &perkfactor, "CastTime", 2, {a_AMC->currentSpell});
                perkfactor = GetEffectiveCastSpeed(a_AMC, a, perkfactor, 2);
                perkfactor = std::min((a_AMC->currentSpell->GetChargeTime()) / a_deltatime, perkfactor);
                perkfactor = std::max(0.05f, perkfactor);
                updatedtime = a_deltatime * perkfactor;
            }

            if (source == RE::MagicSystem::CastingSource::kLeftHand && (a_AMC->state.get() == RE::MagicCaster::State::kUnk02)) {
                

                //RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a, &perkfactor, "CastTime", 2, {a_AMC->currentSpell});
                perkfactor = GetEffectiveCastSpeed(a_AMC, a, perkfactor, 1);
                perkfactor = std::min((a_AMC->currentSpell->GetChargeTime()) / a_deltatime, perkfactor);
                perkfactor = std::max(0.05f, perkfactor);
                updatedtime = a_deltatime * perkfactor;
            }


            func(a_AMC, updatedtime);
        }
        static inline REL::Relocation<decltype(thunk)> func;
    };


        struct DualCasting {
        static bool thunk(RE::ActorMagicCaster* a_AMC) {
            auto a = a_AMC->GetCasterAsActor();
            auto source = a_AMC->GetCastingSource();
            float dualcastfactor = 1.00f;
            if (source == RE::MagicSystem::CastingSource::kRightHand) {
                //RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a, &dualcastfactor, "DualCast", 2,
                //                     {a_AMC->currentSpell});
                dualcastfactor = GetEffectiveDualCast(a_AMC, a, dualcastfactor, 2);
                if (dualcastfactor >= 5) {
                    //a_AMC->flags.set(RE::ActorMagicCaster::Flags::kDualCasting);
                    return true;
                }
            }

            if (source == RE::MagicSystem::CastingSource::kLeftHand) {
                //RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a, &dualcastfactor, "DualCast", 2,
                //                     {a_AMC->currentSpell});
                dualcastfactor = GetEffectiveDualCast(a_AMC, a, dualcastfactor, 1);
                if (dualcastfactor >= 5) {
                    //a_AMC->flags.set(RE::ActorMagicCaster::Flags::kDualCasting);
                    return true;
                }
            }
            return func(a_AMC);
        }
        static inline REL::Relocation<decltype(thunk)> func;
    };

    //struct PlayerCharacter_Update {
    //    static void thunk(RE::PlayerCharacter* a_player, float a_delta) {

    //        func(a_player, a_delta);

    //    }
    //    static inline REL::Relocation<decltype(thunk)> func;
    //};

    static void Install() {
        //stl::write_vfunc<RE::PlayerCharacter, 0xAD, PlayerCharacter_Update>();
        stl::write_vfunc<RE::ActorMagicCaster, 0x1D, Update>();
        stl::write_vfunc<RE::ActorMagicCaster, 0x16, DualCasting>();
    }
};

//void CastSpellMult(RE::StaticFunctionTag*, RE::Actor* akSource, RE::SpellItem* akSpell,
//                     RE::Actor* akTarget, float a_effectiveness) {
//    akSource->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)
//        ->CastSpellImmediate(akSpell, false, akTarget, a_effectiveness, false, 0.0f, nullptr);
//}
//
//void CastEnchantmentMult(RE::StaticFunctionTag*, RE::Actor* akSource, RE::EnchantmentItem* akEnchantment,
//                         RE::Actor* akTarget, float a_effectiveness) {
//    akSource->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)
//        ->CastSpellImmediate(akEnchantment, false, akTarget, a_effectiveness, false, 0.0f, nullptr);
//}
//
//void CastPotionMult(RE::StaticFunctionTag*, RE::Actor* akSource, RE::AlchemyItem* akPotion, RE::Actor* akTarget, float a_effectiveness) {
//    akSource->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)
//        ->CastSpellImmediate(akPotion, false, akTarget, a_effectiveness, false, 0.0f, nullptr);
//}
//
//
//void CastIngredientMult(RE::StaticFunctionTag*, RE::Actor* akSource, RE::IngredientItem* akIngredient,
//                    RE::Actor* akTarget, float a_effectiveness) {
//    akSource->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)
//        ->CastSpellImmediate(akIngredient, false, akTarget, a_effectiveness, false, 0.0f, nullptr);
//}

void CastSpellItemMult(RE::StaticFunctionTag*, RE::Actor* akSource, RE::SpellItem* akSpell,
                           RE::EnchantmentItem* akEnchantment,
                           RE::AlchemyItem* akPotion,
                           RE::IngredientItem* akIngredient,
                       RE::TESObjectREFR* akTarget, float a_effectiveness, float a_override) {
    RE::MagicItem* akMagic = nullptr;
    if (akSpell) {
       akMagic = akSpell;
    } else if (akEnchantment) {
       akMagic = akEnchantment;
    } else if (akPotion) {
       akMagic = akPotion;
    } else if (akIngredient) {
       akMagic = akIngredient;
    }
    auto akCastedMagic = akMagic;
    akSource->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)
        ->CastSpellImmediate(akCastedMagic, false, akTarget, a_effectiveness, false, a_override, akSource);
}

 
void CastSpellMult (RE::StaticFunctionTag*, RE::Actor* akSource, RE::TESForm* MagicItem,
                       RE::TESObjectREFR* akTarget, float a_effectiveness, float a_override) {

    RE::MagicItem* akCastedMagic = MagicItem->As<RE::MagicItem>();

    akSource->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)
        ->CastSpellImmediate(akCastedMagic, false, akTarget, a_effectiveness, false, a_override, akSource);
}


bool GetEffectWasDualCast(RE::StaticFunctionTag*, RE::ActiveEffect* a_activeEffect) {
    if (a_activeEffect) {
        if (a_activeEffect->flags.any(RE::ActiveEffect::Flag::kDual) == true) {
            return true;
        }
            return false;
    } else {
        return false;
    }
}

bool GetEnchantCostOverrideFlag(RE::StaticFunctionTag*, RE::EnchantmentItem* a_enchant) {
    if (a_enchant) {
        if (a_enchant->data.flags.any(RE::EnchantmentItem::EnchantmentFlag::kCostOverride) == true) {
            return true;
        }
        return false;
    } else {
        return false;
    }
}

void SetEnchantCostOverrideFlag(RE::StaticFunctionTag*, RE::EnchantmentItem* a_enchant, bool set) {
    if (a_enchant) {
        if (set == 1) {
            (a_enchant->data.flags.set(RE::EnchantmentItem::EnchantmentFlag::kCostOverride));

        } else if (set == 0) {
            (a_enchant->data.flags.reset(RE::EnchantmentItem::EnchantmentFlag::kCostOverride));
        }
    }
}

int GetEnchantCostOverrideValue(RE::StaticFunctionTag*, RE::EnchantmentItem* a_enchant) {
    if (a_enchant) {
        return a_enchant->data.costOverride;
    } else {
        return 11111;
    }
}

int GetEnchantChargeOverrideValue(RE::StaticFunctionTag*, RE::EnchantmentItem* a_enchant) {
    if (a_enchant) {
        return a_enchant->data.chargeOverride;
    } else {
        return 11111;
    }
}

void setEnchantCostOverrideValue(RE::StaticFunctionTag*, RE::EnchantmentItem* a_enchant, int CostOverride) {
    if (a_enchant) {
        a_enchant->data.costOverride = CostOverride;
    }
}

void setEnchantChargeOverrideValue(RE::StaticFunctionTag*, RE::EnchantmentItem* a_enchant, int ChargeOverride) {
    if (a_enchant) {  
    a_enchant->data.chargeOverride = ChargeOverride;
    }
}

void AdjustActiveEffectMagnitude(RE::StaticFunctionTag*, RE::ActiveEffect* a_activeEffect, float a_power) {
    if (a_activeEffect) {
        float magnitude = a_activeEffect->magnitude;
        a_activeEffect->magnitude = magnitude * a_power;
    }
}


void AdjustActiveEffectDuration(RE::StaticFunctionTag*, RE::ActiveEffect* a_activeEffect, float a_duration) {
    if (a_activeEffect) {
        float duration = a_activeEffect->duration;
        a_activeEffect->magnitude = duration * a_duration;
    }
}


bool PapyrusFunctions(RE::BSScript::IVirtualMachine* vm) {
    vm->RegisterFunction("CastSpellItemMult", "ASR_PapyrusFunctions", CastSpellItemMult);
    vm->RegisterFunction("CastSpellMult", "ASR_PapyrusFunctions", CastSpellMult);
    vm->RegisterFunction("GetEffectWasDualCast", "ASR_PapyrusFunctions", GetEffectWasDualCast);
    vm->RegisterFunction("AdjustActiveEffectMagnitude", "ASR_PapyrusFunctions", AdjustActiveEffectMagnitude);
    vm->RegisterFunction("AdjustActiveEffectDuration", "ASR_PapyrusFunctions", AdjustActiveEffectDuration);
    vm->RegisterFunction("GetEnchantCostOverrideFlag", "ASR_PapyrusFunctions", GetEnchantCostOverrideFlag);
    vm->RegisterFunction("SetEnchantCostOverrideFlag", "ASR_PapyrusFunctions", SetEnchantCostOverrideFlag);
    vm->RegisterFunction("GetEnchantCostOverrideValue", "ASR_PapyrusFunctions", GetEnchantCostOverrideValue);
    vm->RegisterFunction("GetEnchantChargeOverrideValue", "ASR_PapyrusFunctions", GetEnchantChargeOverrideValue);
    vm->RegisterFunction("setEnchantCostOverrideValue", "ASR_PapyrusFunctions", setEnchantCostOverrideValue);
    vm->RegisterFunction("setEnchantChargeOverrideValue", "ASR_PapyrusFunctions", setEnchantChargeOverrideValue);
    return true;
}



SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
    //SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message *message) {
    //    if (message->type == SKSE::MessagingInterface::kDataLoaded)
    //        RE::ConsoleLog::GetSingleton()->Print("Hello, world!");
    //});
    SKSE::GetPapyrusInterface()->Register(PapyrusFunctions);
    Hooks::Install();
    return true;
}