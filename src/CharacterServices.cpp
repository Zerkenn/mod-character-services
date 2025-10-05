/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "ScriptedGossip.h"

class CharacterServices : public CreatureScript
{
  private:
    static constexpr uint32 ACTION_NAME_CHANGE = GOSSIP_ACTION_INFO_DEF + 1;
    static constexpr uint32 ACTION_APPEARANCE_CHANGE = GOSSIP_ACTION_INFO_DEF + 2;
    static constexpr uint32 ACTION_RACE_CHANGE = GOSSIP_ACTION_INFO_DEF + 3;
    static constexpr uint32 ACTION_FACTION_CHANGE = GOSSIP_ACTION_INFO_DEF + 4;

  public:
    CharacterServices() : CreatureScript("character_services") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
      if (!sConfigMgr->GetOption<bool>("CharacterServices.Enable", false))
       return false;

      if (player->HasAtLoginFlag(AT_LOGIN_RENAME) || player->HasAtLoginFlag(AT_LOGIN_CUSTOMIZE) || player->HasAtLoginFlag(AT_LOGIN_CHANGE_RACE) ||player->HasAtLoginFlag(AT_LOGIN_CHANGE_FACTION))
      {
        ChatHandler(player->GetSession()).PSendSysMessage("You already have a pending service. Please log out to use it.");
        CloseGossipMenuFor(player);
        return false;
      }

      if (sConfigMgr->GetOption<bool>("CharacterServices.NameChange.Enable", false))
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|TInterface/Icons/inv_misc_note_02:50:50|tChange My Name", GOSSIP_SENDER_MAIN, ACTION_NAME_CHANGE, "Confirm Name Change.", GetServiceCost(player, ACTION_NAME_CHANGE), false);
      if (sConfigMgr->GetOption<bool>("CharacterServices.AppearanceChange.Enable", false))
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|TInterface/Icons/ability_rogue_disguise:50:50|tChange My Appearance", GOSSIP_SENDER_MAIN, ACTION_APPEARANCE_CHANGE, "Confirm Appearance Change.", GetServiceCost(player, ACTION_APPEARANCE_CHANGE), false);
      if (sConfigMgr->GetOption<bool>("CharacterServices.RaceChange.Enable", false))
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|TInterface/Icons/inv_misc_grouplooking:50:50|tChange My Race", GOSSIP_SENDER_MAIN, ACTION_RACE_CHANGE, "Confirm Race Change.", GetServiceCost(player, ACTION_RACE_CHANGE), false);
      if (sConfigMgr->GetOption<bool>("CharacterServices.FactionChange.Enable", false))
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|TInterface/Icons/inv_bannerpvp_03:50:50|tChange My Faction", GOSSIP_SENDER_MAIN, ACTION_FACTION_CHANGE, "Confirm Faction Change.", GetServiceCost(player, ACTION_FACTION_CHANGE), false);

      SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());

      return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action) override
    {
      ClearGossipMenuFor(player);

      switch (action) {
      case ACTION_NAME_CHANGE:
      case ACTION_APPEARANCE_CHANGE:
      case ACTION_RACE_CHANGE:
      case ACTION_FACTION_CHANGE:
        break;
      default:
        ChatHandler(player->GetSession()).PSendSysMessage("Invalid action chosen!");
        return false;
      }

      if (!player->HasEnoughMoney(GetServiceCost(player, action)))
        {
          ChatHandler(player->GetSession()).PSendSysMessage("You do not have enough money for this service.");
          CloseGossipMenuFor(player);
          return false;
        }

      player->ModifyMoney(-GetServiceCost(player, action));

      switch (action)
      {
          case ACTION_NAME_CHANGE:
            player->SetAtLoginFlag(AT_LOGIN_RENAME);
            ChatHandler(player->GetSession()).PSendSysMessage("{} flagged for name change at the character screen. Please log out.", player->GetPlayerName());
            break;
          case ACTION_APPEARANCE_CHANGE:
            player->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
            ChatHandler(player->GetSession()).PSendSysMessage("{} flagged for appearance change at the character screen. Please log out.", player->GetPlayerName());
            break;
          case ACTION_RACE_CHANGE:
            player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
            ChatHandler(player->GetSession()).PSendSysMessage("{} flagged for race change at the character screen. Please log out.", player->GetPlayerName());
            break;
          case ACTION_FACTION_CHANGE:
            player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
            ChatHandler(player->GetSession()).PSendSysMessage("{} flagged for faction change at the character screen. Please log out.", player->GetPlayerName());
            break;
          default:
            __builtin_unreachable();
      }
      CloseGossipMenuFor(player);

      return true;
    }

    uint32 GetServiceCost(Player* player, uint32 service)
    {
       bool dynamicCostEnabled = sConfigMgr->GetOption<bool>("CharacterServices.DynamicCost.Enable", false);
       double playerLevel = player->GetLevel();
       double dynamicRatio = (playerLevel / 80);
       switch (service)
       {
         case ACTION_NAME_CHANGE:
           {
             uint32 cost = sConfigMgr->GetOption<uint32>("CharacterServices.Cost.NameChange", 25) * 10000;
             return dynamicCostEnabled ? dynamicRatio * cost : cost;
           }
         case ACTION_APPEARANCE_CHANGE:
           {
             uint32 cost = sConfigMgr->GetOption<uint32>("CharacterServices.Cost.AppearanceChange", 50) * 10000;
             return dynamicCostEnabled ? dynamicRatio * cost : cost;
           }
         case ACTION_RACE_CHANGE:
           {
             uint32 cost = sConfigMgr->GetOption<uint32>("CharacterServices.Cost.RaceChange", 100) * 10000;
             return dynamicCostEnabled ? dynamicRatio * cost : cost;
           }
         case ACTION_FACTION_CHANGE:
           {
             uint32 cost = sConfigMgr->GetOption<uint32>("CharacterServices.Cost.FactionChange", 250) * 10000;
             return dynamicCostEnabled ? dynamicRatio * cost : cost;
           }
         default:
           __builtin_unreachable();
       }
    }
};

void AddCharacterServicesScripts()
{
    new CharacterServices();
};
