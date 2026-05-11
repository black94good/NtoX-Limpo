// by Godness. discord: godness.sh


#include "otpch.h"

#include "element.h"

#include <boost/algorithm/string.hpp>

#include "pugicast.h"
#include "tools.h"

bool Elements::loadFromXml()
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("data/xml/elements.xml");
	if (!result) {
		printXMLError("Error - Elements::loadFromXml", "data/xml/elements.xml", result);
		return false;
	}

	for (auto elementNode : doc.child("elements").children()) {
		pugi::xml_attribute attr;
		if (!(attr = elementNode.attribute("id"))) {
			std::cout << "[Warning - Elements::loadFromXml] Missing element id" << std::endl;
			continue;
		}

		uint16_t id = pugi::cast<uint16_t>(attr.value());

		auto it = elements.find(id);
		if (it != elements.end()) {
			std::cout << "[Warning - Elements::loadFromXml] Duplicate element id: " << id << std::endl;
			continue;
		}

		elements.emplace(id, id);
		Element& element = elements[id];

		if ((attr = elementNode.attribute("name"))) {
			element.name = attr.as_string();
		}

		if ((attr = elementNode.attribute("description"))) {
			element.description = attr.as_string();
		}

		if ((attr = elementNode.attribute("element"))) {
			std::string tmpStrValue = boost::algorithm::to_lower_copy<std::string>(attr.as_string());
			if (tmpStrValue == "katon") {
				element.elementType = ELEMENT_KATON;
			} else if (tmpStrValue == "raiton") {
				element.elementType = ELEMENT_RAITON;
			} else if (tmpStrValue == "doton") {
				element.elementType = ELEMENT_DOTON;
			} else if (tmpStrValue == "suiton") {
				element.elementType = ELEMENT_SUITON;
			} else if (tmpStrValue == "fuuton") {
				element.elementType = ELEMENT_FUUTON;
			} else {
				std::cout << "[Warning - Elements::loadFromXml] Unknown element type: " << attr.as_string() << std::endl;
			}
		}

		for (auto elementNode : elementNode.children()) {
			if (!caseInsensitiveEqual(elementNode.name(), "element")) {
				continue;
			}

			if (!(attr = elementNode.attribute("type"))) {
				std::cout << "[Warning - Elements::loadFromXml] Missing element type for element: " << element.name
				          << std::endl;
				continue;
			}

			std::string tmpStrValue = boost::algorithm::to_lower_copy<std::string>(attr.as_string());
			CombatType_t combatType = COMBAT_NONE;

			if (tmpStrValue == "physical") {
				combatType = COMBAT_PHYSICALDAMAGE;
			} else if (tmpStrValue == "energy") {
				combatType = COMBAT_ENERGYDAMAGE;
			} else if (tmpStrValue == "earth") {
				combatType = COMBAT_EARTHDAMAGE;
			} else if (tmpStrValue == "fire") {
				combatType = COMBAT_FIREDAMAGE;
			} else if (tmpStrValue == "ice") {
				combatType = COMBAT_ICEDAMAGE;
			} else if (tmpStrValue == "holy") {
				combatType = COMBAT_HOLYDAMAGE;
			} else if (tmpStrValue == "death") {
				combatType = COMBAT_DEATHDAMAGE;
			} else if (tmpStrValue == "drown") {
				combatType = COMBAT_DROWNDAMAGE;
			} else if (tmpStrValue == "katon") {
				combatType = COMBAT_KATONDAMAGE;
			} else if (tmpStrValue == "raiton") {
				combatType = COMBAT_RAITONDAMAGE;
			} else if (tmpStrValue == "doton") {
				combatType = COMBAT_DOTONDAMAGE;
			} else if (tmpStrValue == "suiton") {
				combatType = COMBAT_SUITONDAMAGE;
			} else if (tmpStrValue == "fuuton") {
				combatType = COMBAT_FUUTONDAMAGE;
			} else {
				std::cout << "[Warning - Elements::loadFromXml] Unknown element type: " << attr.as_string() << std::endl;
				continue;
			}

			float defenseFactor = 1.0f;
			if ((attr = elementNode.attribute("defense"))) {
				defenseFactor = attr.as_float();
			}

			float attackFactor = 1.0f;
			if ((attr = elementNode.attribute("attack"))) {
				attackFactor = attr.as_float();
			}

			element.elementModifiers.emplace(combatType, ElementModifier(combatType, defenseFactor, attackFactor));
		}
	}

	return true;
}

Element* Elements::getElement(uint16_t id)
{
	auto it = elements.find(id);
	if (it == elements.end()) {
		return nullptr;
	}
	return &it->second;
}

Element* Elements::getElementByType(ElementType_t elementType)
{
	for (auto& it : elements) {
		if (it.second.elementType == elementType) {
			return &it.second;
		}
	}
	return nullptr;
}

int32_t Elements::getElementId(const std::string& name) const
{
	for (const auto& it : elements) {
		if (caseInsensitiveEqual(it.second.name.c_str(), name.c_str()) == 0) {
			return it.first;
		}
	}
	return -1;
}

std::vector<std::pair<uint16_t, std::string>> Elements::getChoices(bool includeNone) const
{
	std::vector<std::pair<uint16_t, std::string>> choices;
	for (const auto& [id, element] : elements) {
		if (!includeNone && id == 0) {
			continue;
		}

		choices.emplace_back(id, element.name);
	}
	return choices;
}


float Element::getAttackFactor(CombatType_t combatType) const
{
	auto it = elementModifiers.find(combatType);
	if (it != elementModifiers.end()) {
		return it->second.attackFactor;
	}
	return 1.0f;
}

float Element::getDefenseFactor(CombatType_t combatType) const
{
	auto it = elementModifiers.find(combatType);
	if (it != elementModifiers.end()) {
		return it->second.defenseFactor;
	}
	return 1.0f;
}

CombatType_t Element::elementTypeToCombatType(ElementType_t element)
{
	switch (element) {
		case ELEMENT_KATON:
			return COMBAT_KATONDAMAGE;
		case ELEMENT_RAITON:
			return COMBAT_RAITONDAMAGE;
		case ELEMENT_DOTON:
			return COMBAT_DOTONDAMAGE;
		case ELEMENT_SUITON:
			return COMBAT_SUITONDAMAGE;
		case ELEMENT_FUUTON:
			return COMBAT_FUUTONDAMAGE;
		default:
			return COMBAT_NONE;
	}
}
