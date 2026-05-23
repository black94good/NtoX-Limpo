// Copyright 2023 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "items.h"

#include "configmanager.h"
#include "movement.h"
#include "pugicast.h"
#include "weapons.h"

#include <fstream>

extern MoveEvents* g_moveEvents;
extern Weapons* g_weapons;

namespace {

	const std::unordered_map<std::string, ItemParseAttributes_t> ItemParseAttributesMap = {
		{"type", ITEM_PARSE_TYPE},
		{"description", ITEM_PARSE_DESCRIPTION},
		{"runespellname", ITEM_PARSE_RUNESPELLNAME},
		{"weight", ITEM_PARSE_WEIGHT},
		{"showcount", ITEM_PARSE_SHOWCOUNT},
		{"armor", ITEM_PARSE_ARMOR},
		{"defense", ITEM_PARSE_DEFENSE},
		{"extradef", ITEM_PARSE_EXTRADEF},
		{"attack", ITEM_PARSE_ATTACK},
		{"attackspeed", ITEM_PARSE_ATTACK_SPEED},
		{"rotateto", ITEM_PARSE_ROTATETO},
		{"moveable", ITEM_PARSE_MOVEABLE},
		{"movable", ITEM_PARSE_MOVEABLE},
		{"blockprojectile", ITEM_PARSE_BLOCKPROJECTILE},
		{"allowpickupable", ITEM_PARSE_PICKUPABLE},
		{"pickupable", ITEM_PARSE_PICKUPABLE},
		{"forceserialize", ITEM_PARSE_FORCESERIALIZE},
		{"forcesave", ITEM_PARSE_FORCESERIALIZE},
		{"floorchange", ITEM_PARSE_FLOORCHANGE},
		{"corpsetype", ITEM_PARSE_CORPSETYPE},
		{"containersize", ITEM_PARSE_CONTAINERSIZE},
		{"fluidsource", ITEM_PARSE_FLUIDSOURCE},
		{"readable", ITEM_PARSE_READABLE},
		{"writeable", ITEM_PARSE_WRITEABLE},
		{"maxtextlen", ITEM_PARSE_MAXTEXTLEN},
		{"writeonceitemid", ITEM_PARSE_WRITEONCEITEMID},
		{"weapontype", ITEM_PARSE_WEAPONTYPE},
		{"slottype", ITEM_PARSE_SLOTTYPE},
		{"ammotype", ITEM_PARSE_AMMOTYPE},
		{"shoottype", ITEM_PARSE_SHOOTTYPE},
		{"effect", ITEM_PARSE_EFFECT},
		{"range", ITEM_PARSE_RANGE},
		{"stopduration", ITEM_PARSE_STOPDURATION},
		{"decayto", ITEM_PARSE_DECAYTO},
		{"transformequipto", ITEM_PARSE_TRANSFORMEQUIPTO},
		{"transformdeequipto", ITEM_PARSE_TRANSFORMDEEQUIPTO},
		{"duration", ITEM_PARSE_DURATION},
		{"showduration", ITEM_PARSE_SHOWDURATION},
		{"charges", ITEM_PARSE_CHARGES},
		{"showcharges", ITEM_PARSE_SHOWCHARGES},
		{"showattributes", ITEM_PARSE_SHOWATTRIBUTES},
		{"hitchance", ITEM_PARSE_HITCHANCE},
		{"maxhitchance", ITEM_PARSE_MAXHITCHANCE},
		{"invisible", ITEM_PARSE_INVISIBLE},
		{"speed", ITEM_PARSE_SPEED},
		{"healthgain", ITEM_PARSE_HEALTHGAIN},
		{"healthticks", ITEM_PARSE_HEALTHTICKS},
		{"managain", ITEM_PARSE_MANAGAIN},
		{"manaticks", ITEM_PARSE_MANATICKS},
		{"manashield", ITEM_PARSE_MANASHIELD},
		{"skillsword", ITEM_PARSE_SKILLSWORD},
		{"skillaxe", ITEM_PARSE_SKILLAXE},
		{"skillclub", ITEM_PARSE_SKILLCLUB},
		{"skilldist", ITEM_PARSE_SKILLDIST},
		{"skillfish", ITEM_PARSE_SKILLFISH},
		{"skillshield", ITEM_PARSE_SKILLSHIELD},
		{"skillfist", ITEM_PARSE_SKILLFIST},
		{"maxhitpoints", ITEM_PARSE_MAXHITPOINTS},
		{"maxhitpointspercent", ITEM_PARSE_MAXHITPOINTSPERCENT},
		{"maxmanapoints", ITEM_PARSE_MAXMANAPOINTS},
		{"maxmanapointspercent", ITEM_PARSE_MAXMANAPOINTSPERCENT},
		{"magicpoints", ITEM_PARSE_MAGICPOINTS},
		{"magiclevelpoints", ITEM_PARSE_MAGICPOINTS},
		{"magicpointspercent", ITEM_PARSE_MAGICPOINTSPERCENT},
		{"criticalhitchance", ITEM_PARSE_CRITICALHITCHANCE},
		{"criticalhitamount", ITEM_PARSE_CRITICALHITAMOUNT},
		{"lifeleechchance", ITEM_PARSE_LIFELEECHCHANCE},
		{"lifeleechamount", ITEM_PARSE_LIFELEECHAMOUNT},
		{"manaleechchance", ITEM_PARSE_MANALEECHCHANCE},
		{"manaleechamount", ITEM_PARSE_MANALEECHAMOUNT},
		{"fieldabsorbpercentenergy", ITEM_PARSE_FIELDABSORBPERCENTENERGY},
		{"fieldabsorbpercentfire", ITEM_PARSE_FIELDABSORBPERCENTFIRE},
		{"fieldabsorbpercentpoison", ITEM_PARSE_FIELDABSORBPERCENTPOISON},
		{"fieldabsorbpercentearth", ITEM_PARSE_FIELDABSORBPERCENTPOISON},
		{"absorbpercentall", ITEM_PARSE_ABSORBPERCENTALL},
		{"absorbpercentallelements", ITEM_PARSE_ABSORBPERCENTALL},
		{"absorbpercentelements", ITEM_PARSE_ABSORBPERCENTELEMENTS},
		{"absorbpercentmagic", ITEM_PARSE_ABSORBPERCENTMAGIC},
		{"absorbpercentenergy", ITEM_PARSE_ABSORBPERCENTENERGY},
		{"absorbpercentfire", ITEM_PARSE_ABSORBPERCENTFIRE},
		{"absorbpercentpoison", ITEM_PARSE_ABSORBPERCENTPOISON},
		{"absorbpercentearth", ITEM_PARSE_ABSORBPERCENTPOISON},
		{"absorbpercentice", ITEM_PARSE_ABSORBPERCENTICE},
		{"absorbpercentholy", ITEM_PARSE_ABSORBPERCENTHOLY},
		{"absorbpercentdeath", ITEM_PARSE_ABSORBPERCENTDEATH},
		{"absorbpercentlifedrain", ITEM_PARSE_ABSORBPERCENTLIFEDRAIN},
		{"absorbpercentmanadrain", ITEM_PARSE_ABSORBPERCENTMANADRAIN},
		{"absorbpercentdrown", ITEM_PARSE_ABSORBPERCENTDROWN},
		{"absorbpercentphysical", ITEM_PARSE_ABSORBPERCENTPHYSICAL},
		{"absorbpercenthealing", ITEM_PARSE_ABSORBPERCENTHEALING},
		{"absorbpercentundefined", ITEM_PARSE_ABSORBPERCENTUNDEFINED},
		
		//LONNE ELEMENTO
		{"absorbpercentkaton", ITEM_PARSE_ABSORBPERCENTKATON},
		{"absorbpercentraiton", ITEM_PARSE_ABSORBPERCENTRAITON},
		{"absorbpercentdoton", ITEM_PARSE_ABSORBPERCENTDOTON},
		{"absorbpercentsuiton", ITEM_PARSE_ABSORBPERCENTSUITON},
		{"absorbpercentfuuton", ITEM_PARSE_ABSORBPERCENTFUUTON},
		
		{"suppressdrunk", ITEM_PARSE_SUPPRESSDRUNK},
		{"suppressenergy", ITEM_PARSE_SUPPRESSENERGY},
		{"suppressfire", ITEM_PARSE_SUPPRESSFIRE},
		{"suppresspoison", ITEM_PARSE_SUPPRESSPOISON},
		{"suppressdrown", ITEM_PARSE_SUPPRESSDROWN},
		{"suppressphysical", ITEM_PARSE_SUPPRESSPHYSICAL},
		{"suppressfreeze", ITEM_PARSE_SUPPRESSFREEZE},
		{"suppressdazzle", ITEM_PARSE_SUPPRESSDAZZLE},
		{"suppresscurse", ITEM_PARSE_SUPPRESSCURSE},
		{"field", ITEM_PARSE_FIELD},
		{"replaceable", ITEM_PARSE_REPLACEABLE},
		{"partnerdirection", ITEM_PARSE_PARTNERDIRECTION},
		{"leveldoor", ITEM_PARSE_LEVELDOOR},
		{"maletransformto", ITEM_PARSE_MALETRANSFORMTO},
		{"malesleeper", ITEM_PARSE_MALETRANSFORMTO},
		{"femaletransformto", ITEM_PARSE_FEMALETRANSFORMTO},
		{"femalesleeper", ITEM_PARSE_FEMALETRANSFORMTO},
		{"transformto", ITEM_PARSE_TRANSFORMTO},
		{"destroyto", ITEM_PARSE_DESTROYTO},
		{"elementice", ITEM_PARSE_ELEMENTICE},
		{"elementearth", ITEM_PARSE_ELEMENTEARTH},
		{"elementfire", ITEM_PARSE_ELEMENTFIRE},
		{"elementenergy", ITEM_PARSE_ELEMENTENERGY},
		{"elementdeath", ITEM_PARSE_ELEMENTDEATH},
		{"elementholy", ITEM_PARSE_ELEMENTHOLY},
		
		//LONNE ELEMENTO
		{"elementkaton", ITEM_PARSE_ELEMENTKATON},
		{"elementraiton", ITEM_PARSE_ELEMENTRAITON},
		{"elementdoton", ITEM_PARSE_ELEMENTDOTON},
		{"elementsuiton", ITEM_PARSE_ELEMENTSUITON},
		{"elementfuuton", ITEM_PARSE_ELEMENTFUUTON},


		{"walkstack", ITEM_PARSE_WALKSTACK},
		{"blocking", ITEM_PARSE_BLOCKING},
		{"allowdistread", ITEM_PARSE_ALLOWDISTREAD},
		{"storeitem", ITEM_PARSE_STOREITEM},
		{"worth", ITEM_PARSE_WORTH},
	};

	const std::unordered_map<std::string, ItemTypes_t> ItemTypesMap = {
		{"key", ITEM_TYPE_KEY},
		{"magicfield", ITEM_TYPE_MAGICFIELD},
		{"container", ITEM_TYPE_CONTAINER},
		{"depot", ITEM_TYPE_DEPOT},
		{"mailbox", ITEM_TYPE_MAILBOX},
		{"trashholder", ITEM_TYPE_TRASHHOLDER},
		{"teleport", ITEM_TYPE_TELEPORT},
		{"door", ITEM_TYPE_DOOR},
		{"bed", ITEM_TYPE_BED},
		{"rune", ITEM_TYPE_RUNE},
	};

	const std::unordered_map<std::string, tileflags_t> TileStatesMap = {
		{"down", TILESTATE_FLOORCHANGE_DOWN},
		{"north", TILESTATE_FLOORCHANGE_NORTH},
		{"south", TILESTATE_FLOORCHANGE_SOUTH},
		{"southalt", TILESTATE_FLOORCHANGE_SOUTH_ALT},
		{"west", TILESTATE_FLOORCHANGE_WEST},
		{"east", TILESTATE_FLOORCHANGE_EAST},
		{"eastalt", TILESTATE_FLOORCHANGE_EAST_ALT},
	};

	const std::unordered_map<std::string, RaceType_t> RaceTypesMap = {
		{"venom", RACE_VENOM},
		{"blood", RACE_BLOOD},
		{"undead", RACE_UNDEAD},
		{"fire", RACE_FIRE},
		{"energy", RACE_ENERGY},
	};
	
	//LONNE ELEMENT
	const std::unordered_map<std::string, ElementType_t> ElementTypesMap = {
		{"katon", ELEMENT_KATON},
		{"raiton", ELEMENT_RAITON},
		{"doton", ELEMENT_DOTON},
		{"suiton", ELEMENT_SUITON},
		{"fuuton", ELEMENT_FUUTON},
	};

	const std::unordered_map<std::string, WeaponType_t> WeaponTypesMap = {
		{"sword", WEAPON_SWORD},
		{"club", WEAPON_CLUB},
		{"axe", WEAPON_AXE},
		{"shield", WEAPON_SHIELD},
		{"distance", WEAPON_DISTANCE},
		{"wand", WEAPON_WAND},
		{"ammunition", WEAPON_AMMO},
	};

	const std::unordered_map<std::string, FluidTypes_t> FluidTypesMap = {
		{"water", FLUID_WATER},
		{"blood", FLUID_BLOOD},
		{"beer", FLUID_BEER},
		{"slime", FLUID_SLIME},
		{"lemonade", FLUID_LEMONADE},
		{"milk", FLUID_MILK},
		{"mana", FLUID_MANA},
		{"life", FLUID_LIFE},
		{"oil", FLUID_OIL},
		{"urine", FLUID_URINE},
		{"coconut", FLUID_COCONUTMILK},
		{"wine", FLUID_WINE},
		{"mud", FLUID_MUD},
		{"fruitjuice", FLUID_FRUITJUICE},
		{"lava", FLUID_LAVA},
		{"rum", FLUID_RUM},
		{"swamp", FLUID_SWAMP},
		{"tea", FLUID_TEA},
		{"mead", FLUID_MEAD},
	};

	const std::unordered_map<std::string_view, Direction> DirectionsMap = {
		{"north", DIRECTION_NORTH},
		{"n", DIRECTION_NORTH},
		{"0", DIRECTION_NORTH},
		{"east", DIRECTION_EAST},
		{"e", DIRECTION_EAST},
		{"1", DIRECTION_EAST},
		{"south", DIRECTION_SOUTH},
		{"s", DIRECTION_SOUTH},
		{"2", DIRECTION_SOUTH},
		{"west", DIRECTION_WEST},
		{"w", DIRECTION_WEST},
		{"3", DIRECTION_WEST},
		{"southwest", DIRECTION_SOUTHWEST},
		{"south west", DIRECTION_SOUTHWEST},
		{"south-west", DIRECTION_SOUTHWEST},
		{"sw", DIRECTION_SOUTHWEST},
		{"4", DIRECTION_SOUTHWEST},
		{"southeast", DIRECTION_SOUTHEAST},
		{"south east", DIRECTION_SOUTHEAST},
		{"south-east", DIRECTION_SOUTHEAST},
		{"se", DIRECTION_SOUTHEAST},
		{"5", DIRECTION_SOUTHEAST},
		{"northwest", DIRECTION_NORTHWEST},
		{"north west", DIRECTION_NORTHWEST},
		{"north-west", DIRECTION_NORTHWEST},
		{"nw", DIRECTION_NORTHWEST},
		{"6", DIRECTION_NORTHWEST},
		{"northeast", DIRECTION_NORTHEAST},
		{"north east", DIRECTION_NORTHEAST},
		{"north-east", DIRECTION_NORTHEAST},
		{"ne", DIRECTION_NORTHEAST},
		{"7", DIRECTION_NORTHEAST},
	};

	Direction getDirection(std::string_view string) {
		if (auto it = DirectionsMap.find(string); it != DirectionsMap.end()) {
			return it->second;
		}
		fmt::print("[Warning - getDirection] Invalid direction: {}\n", string);
		return DIRECTION_NORTH;
	}

} // namespace

Items::Items() {
	items.reserve(30000);
	nameToItems.reserve(30000);
}

void Items::clear() {
	items.clear();
	nameToItems.clear();
	currencyItems.clear();
	inventory.clear();
}

bool Items::reload() {
	clear();
	if (!loadFromDat(getString(ConfigManager::ASSETS_DAT_PATH))) {
		return false;
	}

	if (!loadFromXml()) {
		return false;
	}

	g_moveEvents->reload();
	g_weapons->reload();
	g_weapons->loadDefaults();
	return true;
}

bool Items::loadFromDat(std::string_view file)
{
	majorVersion = 3;
	minorVersion = CLIENT_VERSION_1098;
	buildNumber = 0;

	std::ifstream fin(std::string{file}, std::ios::binary | std::ios::ate);
	if (!fin.is_open()) {
		std::cout << "[Error - Items::loadFromDat] Unable to load assets.dat from path: " << file << std::endl;
		std::cout << "[Error - Items::loadFromDat] Copy 'Tibia.dat' from your client folder, rename it to 'assets.dat' and place it in 'data/items/'." << std::endl;
		return false;
	}

	auto fileSize = static_cast<size_t>(fin.tellg());
	fin.seekg(0, std::ios::beg);

	std::vector<uint8_t> buf(fileSize);
	fin.read(reinterpret_cast<char*>(buf.data()), fileSize);
	fin.close();

	size_t pos = 4;

	uint16_t itemCount = 0;
	std::memcpy(&itemCount, &buf[pos], sizeof(itemCount));
	pos += sizeof(itemCount);

	pos += 6;

	items.resize(itemCount + 1);

	const bool extendedSprites = true;
	for (uint16_t id = 100; id < items.size(); ++id) {
		ItemType& iType = items[id];
		iType.clientId = id;
		iType.id = id;
		if (!unserializeDatItem(iType, buf.data(), pos, fileSize, extendedSprites)) {
			return false;
		}
	}

	items.shrink_to_fit();
	return true;
}

bool Items::loadFromXml() {
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("data/items/items.xml");
	if (!result) {
		printXMLError("Error - Items::loadFromXml", "data/items/items.xml", result);
		return false;
	}

	for (auto itemNode : doc.child("items").children()) {
		pugi::xml_attribute idAttribute = itemNode.attribute("id");
		if (idAttribute) {
			parseItemNode(itemNode, pugi::cast<uint16_t>(idAttribute.value()));
			continue;
		}

		pugi::xml_attribute fromIdAttribute = itemNode.attribute("fromid");
		if (!fromIdAttribute) {
			std::cout << "[Warning - Items::loadFromXml] No item id found" << std::endl;
			continue;
		}

		pugi::xml_attribute toIdAttribute = itemNode.attribute("toid");
		if (!toIdAttribute) {
			std::cout << "[Warning - Items::loadFromXml] fromid (" << fromIdAttribute.value() << ") without toid" << std::endl;
			continue;
		}

		uint16_t id = pugi::cast<uint16_t>(fromIdAttribute.value());
		uint16_t toId = pugi::cast<uint16_t>(toIdAttribute.value());
		while (id <= toId) {
			parseItemNode(itemNode, id++);
		}
	}

	buildInventoryList();
	return true;
}

void Items::buildInventoryList() {
	inventory.reserve(items.size());
	for (const auto& type: items) {
		if (type.weaponType != WEAPON_NONE || type.ammoType != AMMO_NONE ||
			type.attack != 0 || type.defense != 0 ||
			type.extraDefense != 0 || type.armor != 0 ||
			type.slotPosition & SLOTP_NECKLACE ||
			type.slotPosition & SLOTP_RING ||
			type.slotPosition & SLOTP_AMMO ||
			type.slotPosition & SLOTP_FEET ||
			type.slotPosition & SLOTP_HEAD ||
			type.slotPosition & SLOTP_ARMOR ||
			type.slotPosition & SLOTP_LEGS)
		{
			inventory.push_back(type.clientId);
		}
	}
	inventory.shrink_to_fit();
	std::sort(inventory.begin(), inventory.end());
}

void Items::parseItemNode(const pugi::xml_node& itemNode, uint16_t id) {
	if (id > 0 && id < 100) {
		ItemType& iType = items[id];
		iType.id = id;
	}

	ItemType& it = getItemType(id);
	if (it.id == 0) {
		return;
	}

	if (!it.name.empty()) {
		std::cout << "[Warning - Items::parseItemNode] Duplicate item with id: " << id << std::endl;
		return;
	}

	it.name = itemNode.attribute("name").as_string();

	if (!it.name.empty()) {
		std::string lowerCaseName = boost::algorithm::to_lower_copy(it.name);
		if (nameToItems.find(lowerCaseName) == nameToItems.end()) {
			nameToItems.emplace(std::move(lowerCaseName), id);
		}
	}

	pugi::xml_attribute articleAttribute = itemNode.attribute("article");
	if (articleAttribute) {
		it.article = articleAttribute.as_string();
	}

	pugi::xml_attribute pluralAttribute = itemNode.attribute("plural");
	if (pluralAttribute) {
		it.pluralName = pluralAttribute.as_string();
	}

	Abilities& abilities = it.getAbilities();

	for (auto attributeNode : itemNode.children()) {
		pugi::xml_attribute keyAttribute = attributeNode.attribute("key");
		if (!keyAttribute) {
			continue;
		}

		pugi::xml_attribute valueAttribute = attributeNode.attribute("value");
		if (!valueAttribute) {
			continue;
		}

		std::string tmpStrValue = boost::algorithm::to_lower_copy<std::string>(keyAttribute.as_string());
		auto parseAttribute = ItemParseAttributesMap.find(tmpStrValue);
		if (parseAttribute != ItemParseAttributesMap.end()) {
			ItemParseAttributes_t parseType = parseAttribute->second;
			switch (parseType) {
				case ITEM_PARSE_TYPE: {
					tmpStrValue = boost::algorithm::to_lower_copy<std::string>(valueAttribute.as_string());
					auto it2 = ItemTypesMap.find(tmpStrValue);
					if (it2 != ItemTypesMap.end()) {
						it.type = it2->second;
						if (it.type == ITEM_TYPE_CONTAINER) {
							it.group = ITEM_GROUP_CONTAINER;
						}
					} else {
						std::cout << "[Warning - Items::parseItemNode] Unknown type: " << valueAttribute.as_string() << std::endl;
					}
					break;
				}

				case ITEM_PARSE_DESCRIPTION: {
					it.description = valueAttribute.as_string();
					break;
				}

				case ITEM_PARSE_RUNESPELLNAME: {
					it.runeSpellName = valueAttribute.as_string();
					break;
				}

				case ITEM_PARSE_WEIGHT: {
					it.weight = pugi::cast<uint32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_SHOWCOUNT: {
					it.showCount = valueAttribute.as_bool();
					break;
				}

				case ITEM_PARSE_ARMOR: {
					it.armor = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_DEFENSE: {
					it.defense = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_EXTRADEF: {
					it.extraDefense = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_ATTACK: {
					it.attack = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_ATTACK_SPEED: {
					it.attackSpeed = pugi::cast<uint32_t>(valueAttribute.value());
					if (it.attackSpeed > 0 && it.attackSpeed < 100) {
						std::cout << "[Warning - Items::parseItemNode] AttackSpeed lower than 100 for item: " << it.id << std::endl;
						it.attackSpeed = 100;
					}
					break;
				}

				case ITEM_PARSE_ROTATETO: {
					it.rotateTo = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_MOVEABLE: {
					it.moveable = valueAttribute.as_bool();
					break;
				}

				case ITEM_PARSE_BLOCKPROJECTILE: {
					it.blockProjectile = valueAttribute.as_bool();
					break;
				}

				case ITEM_PARSE_PICKUPABLE: {
					it.allowPickupable = valueAttribute.as_bool();
					break;
				}

				case ITEM_PARSE_FORCESERIALIZE: {
					it.forceSerialize = valueAttribute.as_bool();
					break;
				}

				case ITEM_PARSE_FLOORCHANGE: {
					tmpStrValue = boost::algorithm::to_lower_copy<std::string>(valueAttribute.as_string());
					auto it2 = TileStatesMap.find(tmpStrValue);
					if (it2 != TileStatesMap.end()) {
						it.floorChange |= it2->second;
					} else {
						std::cout << "[Warning - Items::parseItemNode] Unknown floorChange: " << valueAttribute.as_string() << std::endl;
					}
					break;
				}

				case ITEM_PARSE_CORPSETYPE: {
					tmpStrValue = boost::algorithm::to_lower_copy<std::string>(valueAttribute.as_string());
					auto it2 = RaceTypesMap.find(tmpStrValue);
					if (it2 != RaceTypesMap.end()) {
						it.corpseType = it2->second;
					} else {
						std::cout << "[Warning - Items::parseItemNode] Unknown corpseType: " << valueAttribute.as_string() << std::endl;
					}
					break;
				}

				case ITEM_PARSE_CONTAINERSIZE: {
					it.maxItems = pugi::cast<uint16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_FLUIDSOURCE: {
					tmpStrValue = boost::algorithm::to_lower_copy<std::string>(valueAttribute.as_string());
					auto it2 = FluidTypesMap.find(tmpStrValue);
					if (it2 != FluidTypesMap.end()) {
						it.fluidSource = it2->second;
					} else {
						std::cout << "[Warning - Items::parseItemNode] Unknown fluidSource: " << valueAttribute.as_string() << std::endl;
					}
					break;
				}

				case ITEM_PARSE_READABLE: {
					it.canReadText = valueAttribute.as_bool();
					break;
				}

				case ITEM_PARSE_WRITEABLE: {
					it.canWriteText = valueAttribute.as_bool();
					it.canReadText = it.canWriteText;
					break;
				}

				case ITEM_PARSE_MAXTEXTLEN: {
					it.maxTextLen = pugi::cast<uint16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_WRITEONCEITEMID: {
					it.writeOnceItemId = pugi::cast<uint16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_WEAPONTYPE: {
					tmpStrValue = boost::algorithm::to_lower_copy<std::string>(valueAttribute.as_string());
					auto it2 = WeaponTypesMap.find(tmpStrValue);
					if (it2 != WeaponTypesMap.end()) {
						it.weaponType = it2->second;
					} else {
						std::cout << "[Warning - Items::parseItemNode] Unknown weaponType: " << valueAttribute.as_string() << std::endl;
					}
					break;
				}

				case ITEM_PARSE_SLOTTYPE: {
					tmpStrValue = boost::algorithm::to_lower_copy<std::string>(valueAttribute.as_string());
					if (tmpStrValue == "head") {
						it.slotPosition |= SLOTP_HEAD;
					} else if (tmpStrValue == "body") {
						it.slotPosition |= SLOTP_ARMOR;
					} else if (tmpStrValue == "legs") {
						it.slotPosition |= SLOTP_LEGS;
					} else if (tmpStrValue == "feet") {
						it.slotPosition |= SLOTP_FEET;
					} else if (tmpStrValue == "backpack") {
						it.slotPosition |= SLOTP_BACKPACK;
					} else if (tmpStrValue == "two-handed") {
						it.slotPosition |= SLOTP_TWO_HAND;
					} else if (tmpStrValue == "right-hand") {
						it.slotPosition &= ~SLOTP_LEFT;
					} else if (tmpStrValue == "left-hand") {
						it.slotPosition &= ~SLOTP_RIGHT;
					} else if (tmpStrValue == "necklace") {
						it.slotPosition |= SLOTP_NECKLACE;
					} else if (tmpStrValue == "ring") {
						it.slotPosition |= SLOTP_RING;
					} else if (tmpStrValue == "ammo") {
						it.slotPosition |= SLOTP_AMMO;
					} else if (tmpStrValue == "hand") {
						it.slotPosition |= SLOTP_HAND;
					} else {
						std::cout << "[Warning - Items::parseItemNode] Unknown slotType: " << valueAttribute.as_string() << std::endl;
					}
					break;
				}

				case ITEM_PARSE_AMMOTYPE: {
					it.ammoType = getAmmoType(boost::algorithm::to_lower_copy<std::string>(valueAttribute.as_string()));
					if (it.ammoType == AMMO_NONE) {
						std::cout << "[Warning - Items::parseItemNode] Unknown ammoType: " << valueAttribute.as_string() << std::endl;
					}
					break;
				}

				case ITEM_PARSE_SHOOTTYPE: {
					ShootType_t shoot = getShootType(boost::algorithm::to_lower_copy<std::string>(valueAttribute.as_string()));
					if (shoot != CONST_ANI_NONE) {
						it.shootType = shoot;
					} else {
						std::cout << "[Warning - Items::parseItemNode] Unknown shootType: " << valueAttribute.as_string() << std::endl;
					}
					break;
				}

				case ITEM_PARSE_EFFECT: {
					MagicEffectClasses effect = getMagicEffect(boost::algorithm::to_lower_copy<std::string>(valueAttribute.as_string()));
					if (effect != CONST_ME_NONE) {
						it.magicEffect = effect;
					} else {
						std::cout << "[Warning - Items::parseItemNode] Unknown effect: " << valueAttribute.as_string() << std::endl;
					}
					break;
				}

				case ITEM_PARSE_RANGE: {
					it.shootRange = pugi::cast<uint16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_STOPDURATION: {
					it.stopTime = valueAttribute.as_bool();
					break;
				}

				case ITEM_PARSE_DECAYTO: {
					it.decayTo = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_TRANSFORMEQUIPTO: {
					it.transformEquipTo = pugi::cast<uint16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_TRANSFORMDEEQUIPTO: {
					it.transformDeEquipTo = pugi::cast<uint16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_DURATION: {
					it.decayTime = pugi::cast<uint32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_SHOWDURATION: {
					it.showDuration = valueAttribute.as_bool();
					break;
				}

				case ITEM_PARSE_CHARGES: {
					it.charges = pugi::cast<uint32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_SHOWCHARGES: {
					it.showCharges = valueAttribute.as_bool();
					break;
				}

				case ITEM_PARSE_SHOWATTRIBUTES: {
					it.showAttributes = valueAttribute.as_bool();
					break;
				}

				case ITEM_PARSE_HITCHANCE: {
					it.hitChance = std::min<int8_t>(100, std::max<int8_t>(-100, pugi::cast<int16_t>(valueAttribute.value())));
					break;
				}

				case ITEM_PARSE_MAXHITCHANCE: {
					it.maxHitChance = std::min<uint32_t>(100, pugi::cast<uint32_t>(valueAttribute.value()));
					break;
				}

				case ITEM_PARSE_INVISIBLE: {
					abilities.invisible = valueAttribute.as_bool();
					break;
				}

				case ITEM_PARSE_SPEED: {
					abilities.speed = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_HEALTHGAIN: {
					abilities.regeneration = true;
					abilities.healthGain = pugi::cast<uint32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_HEALTHTICKS: {
					abilities.regeneration = true;
					abilities.healthTicks = pugi::cast<uint32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_MANAGAIN: {
					abilities.regeneration = true;
					abilities.manaGain = pugi::cast<uint32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_MANATICKS: {
					abilities.regeneration = true;
					abilities.manaTicks = pugi::cast<uint32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_MANASHIELD: {
					abilities.manaShield = valueAttribute.as_bool();
					break;
				}

				case ITEM_PARSE_SKILLSWORD: {
					abilities.skills[SKILL_SWORD] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_SKILLAXE: {
					abilities.skills[SKILL_AXE] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_SKILLCLUB: {
					abilities.skills[SKILL_CLUB] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_SKILLDIST: {
					abilities.skills[SKILL_DISTANCE] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_SKILLFISH: {
					abilities.skills[SKILL_FISHING] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_SKILLSHIELD: {
					abilities.skills[SKILL_SHIELD] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_SKILLFIST: {
					abilities.skills[SKILL_FIST] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_CRITICALHITAMOUNT: {
					abilities.specialSkills[SPECIALSKILL_CRITICALHITAMOUNT] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_CRITICALHITCHANCE: {
					abilities.specialSkills[SPECIALSKILL_CRITICALHITCHANCE] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_MANALEECHAMOUNT: {
					abilities.specialSkills[SPECIALSKILL_MANALEECHAMOUNT] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_MANALEECHCHANCE: {
					abilities.specialSkills[SPECIALSKILL_MANALEECHCHANCE] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_LIFELEECHAMOUNT: {
					abilities.specialSkills[SPECIALSKILL_LIFELEECHAMOUNT] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_LIFELEECHCHANCE: {
					abilities.specialSkills[SPECIALSKILL_LIFELEECHCHANCE] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_MAXHITPOINTS: {
					abilities.stats[STAT_MAXHITPOINTS] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_MAXHITPOINTSPERCENT: {
					abilities.statsPercent[STAT_MAXHITPOINTS] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_MAXMANAPOINTS: {
					abilities.stats[STAT_MAXMANAPOINTS] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_MAXMANAPOINTSPERCENT: {
					abilities.statsPercent[STAT_MAXMANAPOINTS] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_MAGICPOINTS: {
					abilities.stats[STAT_MAGICPOINTS] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_MAGICPOINTSPERCENT: {
					abilities.statsPercent[STAT_MAGICPOINTS] = pugi::cast<int32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_FIELDABSORBPERCENTENERGY: {
					abilities.fieldAbsorbPercent[combatTypeToIndex(COMBAT_ENERGYDAMAGE)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_FIELDABSORBPERCENTFIRE: {
					abilities.fieldAbsorbPercent[combatTypeToIndex(COMBAT_FIREDAMAGE)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_FIELDABSORBPERCENTPOISON: {
					abilities.fieldAbsorbPercent[combatTypeToIndex(COMBAT_EARTHDAMAGE)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_ABSORBPERCENTALL: {
					int16_t value = pugi::cast<int16_t>(valueAttribute.value());
					for (auto& i : abilities.absorbPercent) {
						i += value;
					}
					break;
				}

				case ITEM_PARSE_ABSORBPERCENTELEMENTS: {
					int16_t value = pugi::cast<int16_t>(valueAttribute.value());
					abilities.absorbPercent[combatTypeToIndex(COMBAT_ENERGYDAMAGE)] += value;
					abilities.absorbPercent[combatTypeToIndex(COMBAT_FIREDAMAGE)] += value;
					abilities.absorbPercent[combatTypeToIndex(COMBAT_EARTHDAMAGE)] += value;
					abilities.absorbPercent[combatTypeToIndex(COMBAT_ICEDAMAGE)] += value;
					
					//LONNE ELEMENTO 
					abilities.absorbPercent[combatTypeToIndex(COMBAT_KATONDAMAGE)] += value;
					abilities.absorbPercent[combatTypeToIndex(COMBAT_RAITONDAMAGE)] += value;
					abilities.absorbPercent[combatTypeToIndex(COMBAT_DOTONDAMAGE)] += value;
					abilities.absorbPercent[combatTypeToIndex(COMBAT_SUITONDAMAGE)] += value;
					abilities.absorbPercent[combatTypeToIndex(COMBAT_FUUTONDAMAGE)] += value;
					
					break;
				}

				case ITEM_PARSE_ABSORBPERCENTMAGIC: {
					int16_t value = pugi::cast<int16_t>(valueAttribute.value());
					abilities.absorbPercent[combatTypeToIndex(COMBAT_ENERGYDAMAGE)] += value;
					abilities.absorbPercent[combatTypeToIndex(COMBAT_FIREDAMAGE)] += value;
					abilities.absorbPercent[combatTypeToIndex(COMBAT_EARTHDAMAGE)] += value;
					abilities.absorbPercent[combatTypeToIndex(COMBAT_ICEDAMAGE)] += value;
					abilities.absorbPercent[combatTypeToIndex(COMBAT_HOLYDAMAGE)] += value;
					abilities.absorbPercent[combatTypeToIndex(COMBAT_DEATHDAMAGE)] += value;
					
					//LONNE ELEMENTO 
					abilities.absorbPercent[combatTypeToIndex(COMBAT_KATONDAMAGE)] += value;
					abilities.absorbPercent[combatTypeToIndex(COMBAT_RAITONDAMAGE)] += value;
					abilities.absorbPercent[combatTypeToIndex(COMBAT_DOTONDAMAGE)] += value;
					abilities.absorbPercent[combatTypeToIndex(COMBAT_SUITONDAMAGE)] += value;
					abilities.absorbPercent[combatTypeToIndex(COMBAT_FUUTONDAMAGE)] += value;
					break;
				}

				case ITEM_PARSE_ABSORBPERCENTENERGY: {
					abilities.absorbPercent[combatTypeToIndex(COMBAT_ENERGYDAMAGE)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_ABSORBPERCENTFIRE: {
					abilities.absorbPercent[combatTypeToIndex(COMBAT_FIREDAMAGE)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_ABSORBPERCENTPOISON: {
					abilities.absorbPercent[combatTypeToIndex(COMBAT_EARTHDAMAGE)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_ABSORBPERCENTICE: {
					abilities.absorbPercent[combatTypeToIndex(COMBAT_ICEDAMAGE)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_ABSORBPERCENTHOLY: {
					abilities.absorbPercent[combatTypeToIndex(COMBAT_HOLYDAMAGE)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_ABSORBPERCENTDEATH: {
					abilities.absorbPercent[combatTypeToIndex(COMBAT_DEATHDAMAGE)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_ABSORBPERCENTLIFEDRAIN: {
					abilities.absorbPercent[combatTypeToIndex(COMBAT_LIFEDRAIN)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_ABSORBPERCENTMANADRAIN: {
					abilities.absorbPercent[combatTypeToIndex(COMBAT_MANADRAIN)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_ABSORBPERCENTDROWN: {
					abilities.absorbPercent[combatTypeToIndex(COMBAT_DROWNDAMAGE)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_ABSORBPERCENTPHYSICAL: {
					abilities.absorbPercent[combatTypeToIndex(COMBAT_PHYSICALDAMAGE)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_ABSORBPERCENTHEALING: {
					abilities.absorbPercent[combatTypeToIndex(COMBAT_HEALING)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_ABSORBPERCENTUNDEFINED: {
					abilities.absorbPercent[combatTypeToIndex(COMBAT_UNDEFINEDDAMAGE)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}
				
				//LONNE ELEMENTO

				case ITEM_PARSE_ABSORBPERCENTKATON: {
					abilities.absorbPercent[combatTypeToIndex(COMBAT_KATONDAMAGE)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}
				
				case ITEM_PARSE_ABSORBPERCENTRAITON: {
					abilities.absorbPercent[combatTypeToIndex(COMBAT_RAITONDAMAGE)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}
				
				case ITEM_PARSE_ABSORBPERCENTDOTON: {
					abilities.absorbPercent[combatTypeToIndex(COMBAT_DOTONDAMAGE)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_ABSORBPERCENTSUITON: {
					abilities.absorbPercent[combatTypeToIndex(COMBAT_SUITONDAMAGE)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_ABSORBPERCENTFUUTON: {
					abilities.absorbPercent[combatTypeToIndex(COMBAT_FUUTONDAMAGE)] += pugi::cast<int16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_SUPPRESSDRUNK: {
					if (valueAttribute.as_bool()) {
						abilities.conditionSuppressions |= CONDITION_DRUNK;
					}
					break;
				}

				case ITEM_PARSE_SUPPRESSENERGY: {
					if (valueAttribute.as_bool()) {
						abilities.conditionSuppressions |= CONDITION_ENERGY;
					}
					break;
				}

				case ITEM_PARSE_SUPPRESSFIRE: {
					if (valueAttribute.as_bool()) {
						abilities.conditionSuppressions |= CONDITION_FIRE;
					}
					break;
				}

				case ITEM_PARSE_SUPPRESSPOISON: {
					if (valueAttribute.as_bool()) {
						abilities.conditionSuppressions |= CONDITION_POISON;
					}
					break;
				}

				case ITEM_PARSE_SUPPRESSDROWN: {
					if (valueAttribute.as_bool()) {
						abilities.conditionSuppressions |= CONDITION_DROWN;
					}
					break;
				}

				case ITEM_PARSE_SUPPRESSPHYSICAL: {
					if (valueAttribute.as_bool()) {
						abilities.conditionSuppressions |= CONDITION_BLEEDING;
					}
					break;
				}

				case ITEM_PARSE_SUPPRESSFREEZE: {
					if (valueAttribute.as_bool()) {
						abilities.conditionSuppressions |= CONDITION_FREEZING;
					}
					break;
				}

				case ITEM_PARSE_SUPPRESSDAZZLE: {
					if (valueAttribute.as_bool()) {
						abilities.conditionSuppressions |= CONDITION_DAZZLED;
					}
					break;
				}

				case ITEM_PARSE_SUPPRESSCURSE: {
					if (valueAttribute.as_bool()) {
						abilities.conditionSuppressions |= CONDITION_CURSED;
					}
					break;
				}

				case ITEM_PARSE_FIELD: {
					it.group = ITEM_GROUP_MAGICFIELD;
					it.type = ITEM_TYPE_MAGICFIELD;

					CombatType_t combatType = COMBAT_NONE;
					ConditionDamage* conditionDamage = nullptr;

					tmpStrValue = boost::algorithm::to_lower_copy<std::string>(valueAttribute.as_string());
					if (tmpStrValue == "fire") {
						conditionDamage = new ConditionDamage(CONDITIONID_COMBAT, CONDITION_FIRE);
						combatType = COMBAT_FIREDAMAGE;
					} else if (tmpStrValue == "energy") {
						conditionDamage = new ConditionDamage(CONDITIONID_COMBAT, CONDITION_ENERGY);
						combatType = COMBAT_ENERGYDAMAGE;
					} else if (tmpStrValue == "poison") {
						conditionDamage = new ConditionDamage(CONDITIONID_COMBAT, CONDITION_POISON);
						combatType = COMBAT_EARTHDAMAGE;
					} else if (tmpStrValue == "drown") {
						conditionDamage = new ConditionDamage(CONDITIONID_COMBAT, CONDITION_DROWN);
						combatType = COMBAT_DROWNDAMAGE;
					} else if (tmpStrValue == "physical") {
						conditionDamage = new ConditionDamage(CONDITIONID_COMBAT, CONDITION_BLEEDING);
						combatType = COMBAT_PHYSICALDAMAGE;
					} else {
						std::cout << "[Warning - Items::parseItemNode] Unknown field value: " << valueAttribute.as_string() << std::endl;
					}

					if (combatType != COMBAT_NONE) {
						it.combatType = combatType;
						it.conditionDamage.reset(conditionDamage);

						uint32_t ticks = 0;
						int32_t start = 0;
						int32_t count = 1;
						int32_t initDamage = -1;
						int32_t damage = 0;
						for (auto subAttributeNode : attributeNode.children()) {
							pugi::xml_attribute subKeyAttribute = subAttributeNode.attribute("key");
							if (!subKeyAttribute) {
								continue;
							}

							pugi::xml_attribute subValueAttribute = subAttributeNode.attribute("value");
							if (!subValueAttribute) {
								continue;
							}

							tmpStrValue = boost::algorithm::to_lower_copy<std::string>(subKeyAttribute.as_string());
							if (tmpStrValue == "initdamage") {
								initDamage = pugi::cast<int32_t>(subValueAttribute.value());
							} else if (tmpStrValue == "ticks") {
								ticks = pugi::cast<uint32_t>(subValueAttribute.value());
							} else if (tmpStrValue == "count") {
								count = std::max<int32_t>(1, pugi::cast<int32_t>(subValueAttribute.value()));
							} else if (tmpStrValue == "start") {
								start = std::max<int32_t>(0, pugi::cast<int32_t>(subValueAttribute.value()));
							} else if (tmpStrValue == "damage") {
								damage = -pugi::cast<int32_t>(subValueAttribute.value());
								if (start > 0) {
									std::list<int32_t> damageList;
									ConditionDamage::generateDamageList(damage, start, damageList);
									for (int32_t damageValue : damageList) {
										conditionDamage->addDamage(1, ticks, -damageValue);
									}

									start = 0;
								} else {
									conditionDamage->addDamage(count, ticks, damage);
								}
							}
						}

						// datapack compatibility, presume damage to be initialdamage if initialdamage is not declared.
						// initDamage = 0 (don't override initDamage with damage, don't set any initDamage)
						// initDamage = -1 (undefined, override initDamage with damage)
						if (initDamage > 0 || initDamage < -1) {
							conditionDamage->setInitDamage(-initDamage);
						} else if (initDamage == -1 && start != 0) {
							conditionDamage->setInitDamage(start);
						}

						conditionDamage->setParam(CONDITION_PARAM_FIELD, 1);

						if (conditionDamage->getTotalDamage() > 0) {
							conditionDamage->setParam(CONDITION_PARAM_FORCEUPDATE, 1);
						}
					}
					break;
				}

				case ITEM_PARSE_REPLACEABLE: {
					it.replaceable = valueAttribute.as_bool();
					break;
				}

				case ITEM_PARSE_PARTNERDIRECTION: {
					it.bedPartnerDir = getDirection(valueAttribute.as_string());
					break;
				}

				case ITEM_PARSE_LEVELDOOR: {
					it.levelDoor = pugi::cast<uint32_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_MALETRANSFORMTO: {
					uint16_t value = pugi::cast<uint16_t>(valueAttribute.value());
					it.transformToOnUse[PLAYERSEX_MALE] = value;
					ItemType& other = getItemType(value);
					if (other.transformToFree == 0) {
						other.transformToFree = it.id;
					}

					if (it.transformToOnUse[PLAYERSEX_FEMALE] == 0) {
						it.transformToOnUse[PLAYERSEX_FEMALE] = value;
					}
					break;
				}

				case ITEM_PARSE_FEMALETRANSFORMTO: {
					uint16_t value = pugi::cast<uint16_t>(valueAttribute.value());
					it.transformToOnUse[PLAYERSEX_FEMALE] = value;

					ItemType& other = getItemType(value);
					if (other.transformToFree == 0) {
						other.transformToFree = it.id;
					}

					if (it.transformToOnUse[PLAYERSEX_MALE] == 0) {
						it.transformToOnUse[PLAYERSEX_MALE] = value;
					}
					break;
				}

				case ITEM_PARSE_TRANSFORMTO: {
					it.transformToFree = pugi::cast<uint16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_DESTROYTO: {
					it.destroyTo = pugi::cast<uint16_t>(valueAttribute.value());
					break;
				}

				case ITEM_PARSE_ELEMENTICE: {
					abilities.elementDamage = pugi::cast<uint16_t>(valueAttribute.value());
					abilities.elementType = COMBAT_ICEDAMAGE;
					break;
				}

				case ITEM_PARSE_ELEMENTEARTH: {
					abilities.elementDamage = pugi::cast<uint16_t>(valueAttribute.value());
					abilities.elementType = COMBAT_EARTHDAMAGE;
					break;
				}

				case ITEM_PARSE_ELEMENTFIRE: {
					abilities.elementDamage = pugi::cast<uint16_t>(valueAttribute.value());
					abilities.elementType = COMBAT_FIREDAMAGE;
					break;
				}

				case ITEM_PARSE_ELEMENTENERGY: {
					abilities.elementDamage = pugi::cast<uint16_t>(valueAttribute.value());
					abilities.elementType = COMBAT_ENERGYDAMAGE;
					break;
				}

				case ITEM_PARSE_ELEMENTDEATH: {
					abilities.elementDamage = pugi::cast<uint16_t>(valueAttribute.value());
					abilities.elementType = COMBAT_DEATHDAMAGE;
					break;
				}

				case ITEM_PARSE_ELEMENTHOLY: {
					abilities.elementDamage = pugi::cast<uint16_t>(valueAttribute.value());
					abilities.elementType = COMBAT_HOLYDAMAGE;
					break;
				}
				
				//LONNE ELEMENTO
				case ITEM_PARSE_ELEMENTKATON: {
					abilities.elementDamage = pugi::cast<uint16_t>(valueAttribute.value());
					abilities.elementType = COMBAT_KATONDAMAGE;
					break;
				}
				
				case ITEM_PARSE_ELEMENTRAITON: {
					abilities.elementDamage = pugi::cast<uint16_t>(valueAttribute.value());
					abilities.elementType = COMBAT_RAITONDAMAGE;
					break;
				}

				case ITEM_PARSE_ELEMENTDOTON: {
					abilities.elementDamage = pugi::cast<uint16_t>(valueAttribute.value());
					abilities.elementType = COMBAT_DOTONDAMAGE;
					break;
				}
				
				case ITEM_PARSE_ELEMENTSUITON: {
					abilities.elementDamage = pugi::cast<uint16_t>(valueAttribute.value());
					abilities.elementType = COMBAT_SUITONDAMAGE;
					break;
				}

				case ITEM_PARSE_ELEMENTFUUTON: {
					abilities.elementDamage = pugi::cast<uint16_t>(valueAttribute.value());
					abilities.elementType = COMBAT_FUUTONDAMAGE;
					break;
				}

				case ITEM_PARSE_WALKSTACK: {
					it.walkStack = valueAttribute.as_bool();
					break;
				}

				case ITEM_PARSE_BLOCKING: {
					it.blockSolid = valueAttribute.as_bool();
					break;
				}

				case ITEM_PARSE_ALLOWDISTREAD: {
					it.allowDistRead = booleanString(valueAttribute.as_string());
					break;
				}

				case ITEM_PARSE_STOREITEM: {
					it.storeItem = booleanString(valueAttribute.as_string());
					break;
				}

				case ITEM_PARSE_WORTH: {
					uint64_t worth = pugi::cast<uint64_t>(valueAttribute.value());
					if (currencyItems.find(worth) != currencyItems.end()) {
						std::cout << "[Warning - Items::parseItemNode] Duplicated currency worth. Item " << id
								<< " redefines worth " << worth << std::endl;
					} else {
						currencyItems.insert(CurrencyMap::value_type(worth, id));
						it.worth = worth;
					}
					break;
				}

				default: {
					// It should not ever get to here, only if you add a new key to the map and don't configure a case for it.
					std::cout << "[Warning - Items::parseItemNode] Not configured key value: " << keyAttribute.as_string() << std::endl;
					break;
				}
			}
		} else {
			std::cout << "[Warning - Items::parseItemNode] Unknown key value: " << keyAttribute.as_string() << std::endl;
		}
	}

	//check bed items
	if ((it.transformToFree != 0 || it.transformToOnUse[PLAYERSEX_FEMALE] != 0 || it.transformToOnUse[PLAYERSEX_MALE] != 0) && it.type != ITEM_TYPE_BED) {
		std::cout << "[Warning - Items::parseItemNode] Item " << it.id << " is not set as a bed-type" << std::endl;
	}
}

ItemType& Items::getItemType(size_t id) {
	if (id < items.size()) {
		return items[id];
	}
	return items.front();
}

bool Items::hasItemType(size_t id) const {
	return id < items.size() && items[id].id != 0 && items[id].clientId != 0;
}

const ItemType& Items::getNetworkItemType(size_t id) const {
	if (hasItemType(id)) {
		return items[id];
	}

	if (hasItemType(ITEM_BAG)) {
		return items[ITEM_BAG];
	}

	return items.front();
}

const ItemType& Items::getItemType(size_t id) const {
	if (id < items.size()) {
		return items[id];
	}
	return items.front();
}

const ItemType& Items::getItemIdByClientId(uint16_t spriteId) const {
	if (spriteId >= 100 && hasItemType(spriteId)) {
		return getItemType(spriteId);
	}
	return items.front();
}

bool Items::unserializeDatItem(ItemType& iType, const uint8_t* buf, size_t& pos, size_t bufSize, bool extendedSprites)
{
	auto readUint16 = [buf, bufSize](size_t& cursor, uint16_t& value) -> bool {
		if (cursor + sizeof(uint16_t) > bufSize) {
			return false;
		}

		std::memcpy(&value, &buf[cursor], sizeof(uint16_t));
		cursor += sizeof(uint16_t);
		return true;
	};

	auto skipBytes = [bufSize](size_t& cursor, size_t amount) -> bool {
		if (cursor + amount > bufSize) {
			return false;
		}

		cursor += amount;
		return true;
	};

	auto remapAttr = [](uint8_t rawAttr) -> uint16_t {
		if (rawAttr == 16) {
			return 253;
		}
		if (rawAttr == 254) {
			return 34;
		}
		if (rawAttr == 35) {
			return 251;
		}
		if (rawAttr > 16) {
			return rawAttr - 1;
		}
		return rawAttr;
	};

	uint8_t rawFlag;
	do {
		if (pos >= bufSize) {
			return false;
		}

		rawFlag = buf[pos++];
		if (rawFlag == 0xFF) {
			break;
		}

		switch (remapAttr(rawFlag)) {
			case 0: {
				iType.group = ITEM_GROUP_GROUND;
				uint16_t groundSpeed = 0;
				if (!readUint16(pos, groundSpeed)) {
					return false;
				}
				iType.speed = groundSpeed;
				break;
			}

			case 1:
				iType.alwaysOnTopOrder = 1;
				break;

			case 2:
				iType.alwaysOnTopOrder = 2;
				break;

			case 3:
				iType.alwaysOnTopOrder = 3;
				break;

			case 4:
				iType.group = ITEM_GROUP_CONTAINER;
				iType.type = ITEM_TYPE_CONTAINER;
				break;

			case 5:
				iType.stackable = true;
				break;

			case 6:
				iType.forceUse = true;
				break;

			case 7:
				iType.useable = true;
				break;

			case 8: {
				iType.canWriteText = true;
				iType.canReadText = true;
				uint16_t maxTextLen = 0;
				if (!readUint16(pos, maxTextLen)) {
					return false;
				}
				iType.maxTextLen = maxTextLen;
				break;
			}

			case 9: {
				iType.canReadText = true;
				uint16_t maxTextLen = 0;
				if (!readUint16(pos, maxTextLen)) {
					return false;
				}
				iType.maxTextLen = maxTextLen;
				break;
			}

			case 10:
				iType.group = ITEM_GROUP_FLUID;
				break;

			case 11:
				iType.group = ITEM_GROUP_SPLASH;
				break;

			case 12:
				iType.blockSolid = true;
				break;

			case 13:
				iType.moveable = false;
				break;

			case 14:
				iType.blockProjectile = true;
				break;

			case 15:
				iType.blockPathFind = true;
				break;

			case 16:
				iType.pickupable = true;
				break;

			case 17:
				iType.isHangable = true;
				break;

			case 18:
				iType.isHangable = true;
				iType.isHorizontal = true;
				break;

			case 19:
				iType.isHangable = true;
				iType.isVertical = true;
				break;

			case 20:
				iType.rotatable = true;
				break;

			case 21: {
				uint16_t lightLevel = 0;
				uint16_t lightColor = 0;
				if (!readUint16(pos, lightLevel) || !readUint16(pos, lightColor)) {
					return false;
				}
				iType.lightLevel = lightLevel;
				iType.lightColor = lightColor;
				break;
			}

			case 22:
			case 23:
			case 26:
			case 27:
			case 30:
				break;

			case 24:
				if (!skipBytes(pos, 4)) {
					return false;
				}
				break;

			case 25:
				iType.hasHeight = true;
				if (!skipBytes(pos, 2)) {
					return false;
				}
				break;

			case 28:
				if (!skipBytes(pos, 2)) {
					return false;
				}
				break;

			case 29: {
				uint16_t lensHelp = 0;
				if (!readUint16(pos, lensHelp)) {
					return false;
				}
				if (lensHelp == 1112) {
					iType.canReadText = true;
				}
				break;
			}

			case 31:
				iType.lookThrough = true;
				break;

			case 32:
				if (!skipBytes(pos, 2)) {
					return false;
				}
				break;

			case 33:
				iType.wareId = iType.id;
				if (!skipBytes(pos, 6)) {
					return false;
				}
				{
					uint16_t nameLen = 0;
					if (!readUint16(pos, nameLen) || !skipBytes(pos, static_cast<size_t>(nameLen) + 4)) {
						return false;
					}
				}
				break;

			case 34: {
				uint16_t usable = 0;
				if (!readUint16(pos, usable)) {
					return false;
				}
				iType.useable = (usable != 0);
				break;
			}

			case 35:
			case 36:
			case 37:
			case 38:
			case 39:
			case 40:
			case 41:
			case 42:
			case 43:
			case 44:
			case 253:
				break;

			case 251:
				if (!skipBytes(pos, 2)) {
					return false;
				}
				break;

			default: {
				std::cout << "[Error - Items::unserializeDatItem] Unknown flag " << static_cast<uint32_t>(rawFlag)
					<< " (mapped to " << remapAttr(rawFlag) << ") at id " << iType.id << std::endl;
				return false;
			}
		}
	} while (rawFlag != 0xFF);

	iType.alwaysOnTop = (iType.alwaysOnTopOrder != 0);

	if (pos + 7 > bufSize) {
		return false;
	}

	uint8_t width = buf[pos++];
	uint8_t height = buf[pos++];
	if (width > 1 || height > 1) {
		if (!skipBytes(pos, 1)) {
			return false;
		}
	}

	uint8_t layers = buf[pos++];
	uint8_t patternX = buf[pos++];
	uint8_t patternY = buf[pos++];
	uint8_t patternZ = buf[pos++];
	uint8_t frames = buf[pos++];
	iType.isAnimation = (frames > 1);

	if (frames > 1) {
		if (!skipBytes(pos, 1 + 4 + 1 + (static_cast<size_t>(frames) * 8))) {
			return false;
		}
	}

	uint32_t numSprites = static_cast<uint32_t>(width) * height * layers * patternX * patternY * patternZ * frames;
	const uint32_t spriteIdBytes = extendedSprites ? 4 : 2;
	if (!skipBytes(pos, static_cast<size_t>(spriteIdBytes) * numSprites)) {
		return false;
	}

	return true;
}

uint16_t Items::getItemIdByName(const std::string& name) {
	if (name.empty()) {
		return 0;
	}

	auto result = nameToItems.find(boost::algorithm::to_lower_copy(name));

	if (result == nameToItems.end())
		return 0;

	return result->second;
}
