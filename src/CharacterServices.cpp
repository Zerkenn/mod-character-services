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
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|TInterface/Icons/inv_misc_note_02:50:50|tChange My Name", GOSSIP_SENDER_MAIN, 1, "Confirm Name Change.", GetServiceCost(player, 1), false);
      if (sConfigMgr->GetOption<bool>("CharacterServices.AppearanceChange.Enable", false))
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|TInterface/Icons/ability_rogue_disguise:50:50|tChange My Appearance", GOSSIP_SENDER_MAIN, 2, "Confirm Appearance Change.", GetServiceCost(player, 2), false);
      if (sConfigMgr->GetOption<bool>("CharacterServices.RaceChange.Enable", false))
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|TInterface/Icons/inv_misc_grouplooking:50:50|tChange My Race", GOSSIP_SENDER_MAIN, 3, "Confirm Race Change.", GetServiceCost(player, 3), false);
      if (sConfigMgr->GetOption<bool>("CharacterServices.FactionChange.Enable", false))
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|TInterface/Icons/inv_bannerpvp_03:50:50|tChange My Faction", GOSSIP_SENDER_MAIN, 4, "Confirm Faction Change.", GetServiceCost(player, 4), false);

      SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());

      return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action) override
    {
      ClearGossipMenuFor(player);
      if (!player->HasEnoughMoney(GetServiceCost(player, 4)))
        {
          ChatHandler(player->GetSession()).PSendSysMessage("You do not have enough money for this service.");
          CloseGossipMenuFor(player);
          return false;
        }

      player->ModifyMoney(-GetServiceCost(player, action));

      switch (action)
      {
          case 1:
            player->SetAtLoginFlag(AT_LOGIN_RENAME);
            ChatHandler(player->GetSession()).PSendSysMessage("{} flagged for name change at the charcter screen. Please log out.", player->GetPlayerName());
            break;
          case 2:
            player->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
            ChatHandler(player->GetSession()).PSendSysMessage("{} flagged for appearance change at the charcter screen. Please log out.", player->GetPlayerName());
            break;
          case 3:
            player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
            ChatHandler(player->GetSession()).PSendSysMessage("{} flagged for race change at the charcter screen. Please log out.", player->GetPlayerName());
            break;
          case 4:
            player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
            ChatHandler(player->GetSession()).PSendSysMessage("{} flagged for faction change at the charcter screen. Please log out.", player->GetPlayerName());
            break;
      }
      CloseGossipMenuFor(player);

      return true;
    }

    uint32 GetServiceCost(Player* player, uint8 service)
    {
       bool dynamicCostEnabled = sConfigMgr->GetOption<bool>("CharacterServices.DynamicCost.Enable", false);
       double playerLevel = player->GetLevel();
       double dynamicRatio = (playerLevel / 80);
       switch (service)
       {
         case 1:
           {
             uint32 renameCost = sConfigMgr->GetOption<uint32>("CharacterServices.Cost.NameChange", 25) * 10000;
             return dynamicCostEnabled ? dynamicRatio * renameCost : renameCost;
           }
         case 2:
           {
             uint32 renameCost = sConfigMgr->GetOption<uint32>("CharacterServices.Cost.AppearanceChange", 50) * 10000;
             return dynamicCostEnabled ? dynamicRatio * renameCost : renameCost;
           }
         case 3:
           {
             uint32 renameCost = sConfigMgr->GetOption<uint32>("CharacterServices.Cost.RaceChange", 100) * 10000;
             return dynamicCostEnabled ? dynamicRatio * renameCost : renameCost;
           }
         case 4:
           {
             uint32 renameCost = sConfigMgr->GetOption<uint32>("CharacterServices.Cost.FactionChange", 250) * 10000;
             return dynamicCostEnabled ? dynamicRatio * renameCost : renameCost;
           }
         default:
           return 250;
       }
    }
};

void AddCharacterServicesScripts()
{
    new CharacterServices();
};
