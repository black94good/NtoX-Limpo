// Copyright 2023 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_ELEMENT_H
#define FS_ELEMENT_H

#include "enums.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

struct ElementModifier
{
	CombatType_t combatType;
	float defenseFactor;
	float attackFactor;

	ElementModifier(CombatType_t type, float defFactor, float atkFactor) :
	    combatType(type), defenseFactor(defFactor), attackFactor(atkFactor)
	{}
};

class Element
{
public:
	Element() = default;
	explicit Element(uint16_t id) : id(id) {}

	const std::string& getName() const { return name; }
	uint16_t getId() const { return id; }
	ElementType_t getElementType() const { return elementType; }

	float getAttackFactor(CombatType_t combatType) const;
	float getDefenseFactor(CombatType_t combatType) const;

private:
	friend class Elements;

	std::string name;
	std::string description;

	std::map<CombatType_t, ElementModifier> elementModifiers;

	uint16_t id = 0;
	ElementType_t elementType = ELEMENT_NONE;

	static CombatType_t elementTypeToCombatType(ElementType_t element);
};

class Elements
{
public:
	bool loadFromXml();

	Element* getElement(uint16_t id);
	Element* getElementByType(ElementType_t elementType);
	int32_t getElementId(const std::string& name) const;
	std::vector<std::pair<uint16_t, std::string>> getChoices(bool includeNone = false) const;

private:
	std::map<uint16_t, Element> elements;
};

#endif // FS_ELEMENT_H
