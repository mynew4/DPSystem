#include "ScriptPCH.h"
#include <sstream>
#include <string>
#include <vector>

std::string getString(std::string string, uint32 number);
std::string getDPString(std::string string, uint32 number, uint8 quality);
uint32 getDPFromID(uint32 item_id);

bool canRun = true;

class dp_items
{
public:
	dp_items()	{}
	dp_items(uint32 item_id, uint8 item_type, std::string item_name, uint8 quality, uint32 dp_cost)
	{
		this->item_id = item_id;
		this->dp_cost = dp_cost;
		this->quality = quality;
		this->item_type = item_type;
		this->item_name = item_name;
	}
	inline uint32	getItemID()		{ return item_id; }
	inline uint32	getDPCost()		{ return dp_cost; }
	inline uint8	getQuality()	{ return quality; }
	inline uint8	getItemType()	{ return item_type; }
	inline std::string getName()	{ return item_name; }

private:
	uint32 item_id, dp_cost;
	uint8 quality, item_type;
	std::string item_name;
};

std::vector<dp_items> items;

class load_dp : public WorldScript
{
public:
	load_dp() : WorldScript("load_dp") {}

	void OnStartup() {
		QueryResult loadQuery = WorldDatabase.Query(loadStmnt.c_str());
		Field *fields = loadQuery->Fetch();
		if (fields) {
			do
			{
				dp_items item = dp_items(fields[0].GetUInt32(), fields[1].GetUInt8(), fields[2].GetString(), fields[3].GetUInt8(), fields[4].GetUInt32());
				items.push_back(item);
			} while (loadQuery->NextRow());
			TC_LOG_INFO("server.loading", "DP Items Loaded Successfully.");
		}
		else
		{
			canRun = false;
		}
		QueryResult testQuery = LoginDatabase.Query("SELECT * from account LIMIT 1;");
		if (testQuery->GetFieldCount() < 25)
		{
			TC_LOG_INFO("server.loading", "ERROR: Missing dp_balance column.");
			canRun = false;
		}
		else
		{
			TC_LOG_INFO("server.loading", "DP System Loaded Successfully.");
		}		
	}

private:
	std::string loadStmnt = "SELECT * from dp_items;";

};

class gossip_dp : public CreatureScript {
public:
	gossip_dp() : CreatureScript("gossip_dp") { }

	uint32 getDP(Player* player)
	{
		QueryResult dQuery = LoginDatabase.Query(getString(dpQuery, player->GetSession()->GetAccountId()).c_str());
		Field *field = dQuery->Fetch();
		return field[0].GetUInt32();
	}

	bool OnGossipHello(Player* player, Creature* creature) 
	{
		if (canRun)
		{
			if (player->IsInCombat())
			{
				player->GetSession()->SendAreaTriggerMessage("You are in combat");
				return false;
			}

			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, getString(currentDP, getDP(player)).c_str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Preview Donation Items", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Donation Weapons", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Donation Armor", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Donation Items(misc)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Donation Spells", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Nevermind", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
			player->SEND_GOSSIP_MENU(1, creature->GetGUID());
			return true;
		}
		else
		{
			return false;
		}

	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 actions)
	{
		player->PlayerTalkClass->ClearMenus();
		if (sender == GOSSIP_SENDER_MAIN) {
			switch (actions)
			{
			case GOSSIP_ACTION_INFO_DEF + 1:
				creature->Whisper(getString(currentDP, getDP(player)), LANG_UNIVERSAL, player, false);
				player->PlayerTalkClass->SendCloseGossip();
				break;
			case GOSSIP_ACTION_INFO_DEF + 2:
				for (unsigned int i = 0; i < items.size(); i++){
					if (items.at(i).getItemType() == 1)
						player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, getDPString(items.at(i).getName(), items.at(i).getDPCost(), items.at(i).getQuality()).c_str(), GOSSIP_SENDER_MAIN+1, items.at(i).getItemID());
				}
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Back", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
				player->SEND_GOSSIP_MENU(1, creature->GetGUID());
					\
				break;
			case GOSSIP_ACTION_INFO_DEF + 3:
				for (unsigned int i = 0; i < items.size(); i++) {
					if (items.at(i).getItemType() == 2)
						player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, getDPString(items.at(i).getName(), items.at(i).getDPCost(), items.at(i).getQuality()).c_str(), GOSSIP_SENDER_MAIN+1, items.at(i).getItemID());
				}
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Back", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
				player->SEND_GOSSIP_MENU(1, creature->GetGUID());
				break;
			case GOSSIP_ACTION_INFO_DEF + 4:
				for (unsigned int i = 0; i < items.size(); i++){
					if (items.at(i).getItemType() == 3)
						player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, getDPString(items.at(i).getName(), items.at(i).getDPCost(), items.at(i).getQuality()).c_str(), GOSSIP_SENDER_MAIN+1, items.at(i).getItemID());
				}
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Back", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
				player->SEND_GOSSIP_MENU(1, creature->GetGUID());
				break;
			case GOSSIP_ACTION_INFO_DEF + 5:
				for (unsigned int i = 0; i < items.size(); i++){
					if (items.at(i).getItemType() == 4)
						player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, getDPString(items.at(i).getName(), items.at(i).getDPCost(), items.at(i).getQuality()).c_str(), GOSSIP_SENDER_MAIN+2, items.at(i).getItemID());
				}
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Back", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
				player->SEND_GOSSIP_MENU(1, creature->GetGUID());
				break;
			case GOSSIP_ACTION_INFO_DEF + 6:
				player->PlayerTalkClass->SendCloseGossip();
				break;
			case GOSSIP_ACTION_INFO_DEF + 7:
				player->PlayerTalkClass->SendCloseGossip();
				SendListInventory(player, creature);
				break;
			case GOSSIP_ACTION_INFO_DEF + 8:
				player->PlayerTalkClass->ClearMenus();
				OnGossipHello(player, creature);
				break;
			}
		}
		else if (sender == GOSSIP_SENDER_MAIN + 1)
		{
			uint32 dpCost = getDPFromID(actions);
			uint32 dpBAL = getDP(player);
			if (dpCost <= dpBAL)
			{
				if (player->AddItem(actions, 1)) {
					LoginDatabase.PExecute(accountQuery.c_str(), (dpBAL - dpCost), player->GetSession()->GetAccountId());
					player->GetSession()->SendNotification("SUCCESS - Your item has been added, thank you for your purchase.");
				}
				else
				{
					player->GetSession()->SendNotification("ERROR - An error occurred, item is unique or your bags are full. Contact staff with any questions.");
				}
			}
			else
			{
				creature->Whisper(notEnoughDP, LANG_UNIVERSAL, player, false);
				player->GetSession()->SendNotification("ERROR - Not enough DP for transaction.");
			}
			player->PlayerTalkClass->SendCloseGossip();
		}
		else if (sender == GOSSIP_SENDER_MAIN + 2)
		{
			uint32 dpCost = getDPFromID(actions);
			uint32 dpBAL = getDP(player);
			if (dpCost <= dpBAL)
			{
				if (!player->HasSpell(actions)){
					player->LearnSpell(actions, false, false);
					LoginDatabase.PExecute(accountQuery.c_str(), (dpBAL - dpCost), player->GetSession()->GetAccountId());
					player->GetSession()->SendNotification("SUCCESS - You have been taught the selected spell.");

				}
				else
				{
					player->GetSession()->SendNotification("ERROR - You already know this spell.");
				}
			}
			else
			{
				creature->Whisper(notEnoughDP, LANG_UNIVERSAL, player, false);
				player->GetSession()->SendNotification("ERROR - Not enough DP for transaction.");
			}
			player->PlayerTalkClass->SendCloseGossip();
		}

		return true;
	}

	// Do NOT modify anything in SendListInventory, may break previewing or allow players to buy items
	void SendListInventory(Player *player, Creature *creature) 
	{
		// remove fake death
		if (player->HasUnitState(UNIT_STATE_DIED))
			player->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);
		if (creature->HasUnitState(UNIT_STATE_MOVING))
			creature->StopMoving();

		VendorItemData *vItems = new VendorItemData();
		for (uint32 i = 0; i < items.size(); i++) // Allows us to create a vendor window on the fly.
		{
			if (items.at(i).getItemType() != 4) // If the item is not a spell, lets put it in the vendor.
				vItems->AddItem(items.at(i).getItemID(), 0, 0, 59475);
		}
		if (!vItems)
		{
			WorldPacket data(SMSG_LIST_INVENTORY, 8 + 1 + 1);
			data << uint64(creature->GetGUID());
			data << uint8(0);                                   // count == 0, next will be error code
			data << uint8(0);                                   // "Vendor has no inventory"
			player->GetSession()->SendPacket(&data);
			return;
		}
		uint8 itemCount = vItems->GetItemCount();
		uint8 count = 0;
		WorldPacket data(SMSG_LIST_INVENTORY, 8 + 1 + itemCount * 8 * 4);
		data << uint64(creature->GetGUID());
		size_t countPos = data.wpos();
		data << uint8(count);

		float discountMod = player->GetReputationPriceDiscount(creature);
		for (uint8 slot = 0; slot < itemCount; ++slot)
		{
			if (VendorItem const* item = vItems->GetItem(slot))
			{
				if (ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(item->item))
				{
					if (!(itemTemplate->AllowableClass & player->getClassMask()) && itemTemplate->Bonding == BIND_WHEN_PICKED_UP && !player->IsGameMaster())
						continue;
					// Only display items in vendor lists for the team the
					// player is on. If GM on, display all items.
					if (!player->IsGameMaster() && ((itemTemplate->Flags2 & ITEM_FLAGS_EXTRA_HORDE_ONLY && player->GetTeam() == ALLIANCE) || (itemTemplate->Flags2 == ITEM_FLAGS_EXTRA_ALLIANCE_ONLY && player->GetTeam() == HORDE)))
						continue;


					// reputation discount
					int32 price = item->IsGoldRequired(itemTemplate) ? uint32(floor(itemTemplate->BuyPrice * discountMod)) : 0;

					data << uint32(slot + 1);       // client expects counting to start at 1
					data << uint32(item->item);
					data << uint32(itemTemplate->DisplayInfoID);
					data << int32(0);
					data << uint32(price);
					data << uint32(itemTemplate->MaxDurability);
					data << uint32(itemTemplate->BuyCount);
					data << uint32(item->ExtendedCost);

					if (++count >= MAX_VENDOR_ITEMS)
						break;
				}
			}
		}

		if (count == 0)
		{
			data << uint8(0);
			player->GetSession()->SendPacket(&data);
			return;
		}

		data.put<uint8>(countPos, count);
		player->GetSession()->SendPacket(&data);
		
	}
private:
	//Do not edit the Queries/constants below, they're set up 100%.
	std::string accountQuery = "UPDATE account SET dp_balance = %u WHERE id = %u;";
	std::string currentDP = "Current DP: ";
	std::string notEnoughDP = "You do not have enough DP, you can acquire more DP by donating on our website.";
	std::string dpQuery = "SELECT dp_balance FROM account WHERE id=";
};

//Do NOT modify any of the below functions unless you are sure you know what you're doing.
uint32 getDPFromID(uint32 item_id)
{
	//Not the most efficient loop. Let me know if you are using a large amount of items/spells
	//and I can implement a more efficient search.
	uint32 dpCost = 99999;
	for (unsigned int i = 0; i < items.size(); i++)
		if (item_id == items.at(i).getItemID())
			dpCost = items.at(i).getDPCost();
	return dpCost;
}
std::string getString(std::string string, uint32 number)
{
	std::ostringstream oss;
	oss << string << number;
	return oss.str();
}
std::string getDPString(std::string string, uint32 number, uint8 quality)
{

	//Colors currently disabled.
	/*std::string color;
	switch (quality)
	{
	case 0:
		color = "|cff9d9d9d";
		break;
	case 1:
		color = "|cffffffff";
		break;
	case 2:
		color = "|cff47b247";
		break;
	case 3:
		color = "|cff00003d";
		break;
	case 5:
		color = "|cff331f00";
		break;
	default:
		color = "|cff3d003d";
		break;
	}*/
	std::ostringstream oss;
	//oss << color << string << " - (" << number << "DP)"; //With Color
	oss << string << " - (" << number << "DP)"; //Without color
	return oss.str();
}


void AddSC_DPSystem()
{
	new load_dp();
	new gossip_dp();
}
