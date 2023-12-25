scriptName ASR_PapyrusFunctions hidden

Function CastSpellItemMult(Actor akSource, Spell akSpell, Enchantment akEnchantment, Potion akPotion, Ingredient akIngredient, Actor akTarget, float a_effectiveness) global native
; Will try to cast Spell then enchantment then potion then ingredient depending on if the previous form is nullptr

bool Function GetEffectWasDualCast(ActiveMagicEffect a_activeEffect) global native

Function AdjustActiveEffectMagnitude(ActiveMagicEffect a_activeEffect, Float a_power) global native

Function AdjustActiveEffectDuration(ActiveMagicEffect a_activeEffect, Float a_duration) global native
; remaining duration 

