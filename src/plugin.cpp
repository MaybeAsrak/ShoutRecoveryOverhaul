

#include "include\PerkEntryPointExtenderAPI.h"


struct Hooks {
        struct Update {
        static void thunk(RE::ActorMagicCaster* a_AMC, float a_deltatime) {


            float updatedtime = a_deltatime;
            auto a = a_AMC->GetCasterAsActor();
            auto source = a_AMC->GetCastingSource();
            float perkfactor = 1.00f;

            if (source == RE::MagicSystem::CastingSource::kRightHand && (a_AMC->state.get() == RE::MagicCaster::State::kUnk02)) {

                RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a, &perkfactor, "CastTime", 2,
                                     {a_AMC->currentSpell});

                 updatedtime = a_deltatime * perkfactor;
            }

            if (source == RE::MagicSystem::CastingSource::kLeftHand && (a_AMC->state.get() == RE::MagicCaster::State::kUnk02)) {
                
                RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a, &perkfactor, "CastTime", 2,
                                     {a_AMC->currentSpell});

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
                RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a, &dualcastfactor, "DualCast", 2,
                                     {a_AMC->currentSpell});
                if (dualcastfactor >= 5) {
                    a_AMC->flags.set(RE::ActorMagicCaster::Flags::kDualCasting);
                    return true;
                }
            }

            if (source == RE::MagicSystem::CastingSource::kLeftHand) {
                RE::HandleEntryPoint(RE::PerkEntryPoint::kModSpellCost, a, &dualcastfactor, "DualCast", 2,
                                     {a_AMC->currentSpell});
                if (dualcastfactor >= 5) {
                    a_AMC->flags.set(RE::ActorMagicCaster::Flags::kDualCasting);
                    return true;
                }
            }
            return func(a_AMC);
        }
        static inline REL::Relocation<decltype(thunk)> func;
    };

    static void Install() {
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
                        RE::Actor* akTarget, float a_effectiveness) {
    RE::MagicItem* akMagic = nullptr;
    if (akSpell) {
       akMagic = akSpell;
    } else if (akEnchantment){
       akMagic = akEnchantment;
    } else if (akPotion) {
       akMagic = akPotion;
    } else if (akIngredient) {
       akMagic = akIngredient;
    }
    auto akCastedMagic = akMagic;
    akSource->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)
        ->CastSpellImmediate(akCastedMagic, false, akTarget, a_effectiveness, false, 0.0f, akSource);
}

bool GetEffectWasDualCast(RE::StaticFunctionTag*, RE::ActiveEffect* a_activeEffect) {
    
    if (a_activeEffect->flags.any(RE::ActiveEffect::Flag::kDual) == true) {
        return true;
    }
    return false;
}

void AdjustActiveEffectMagnitude(RE::StaticFunctionTag*, RE::ActiveEffect* a_activeEffect, float a_power) {
    float magnitude = a_activeEffect->magnitude;
    a_activeEffect->magnitude = magnitude * a_power;
}


void AdjustActiveEffectDuration(RE::StaticFunctionTag*, RE::ActiveEffect* a_activeEffect, float a_duration) {
    float duration = a_activeEffect->duration; 
    a_activeEffect->magnitude = duration * a_duration;
}


bool PapyrusFunctions(RE::BSScript::IVirtualMachine* vm) {
    vm->RegisterFunction("AdjustActiveEffectMagnitude", "ASR_PapyrusFunctions", CastSpellItemMult);
    vm->RegisterFunction("AdjustActiveEffectMagnitude", "ASR_PapyrusFunctions", GetEffectWasDualCast);
    vm->RegisterFunction("AdjustActiveEffectMagnitude", "ASR_PapyrusFunctions", AdjustActiveEffectMagnitude);
    vm->RegisterFunction("AdjustActiveEffectMagnitude", "ASR_PapyrusFunctions", AdjustActiveEffectDuration);
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