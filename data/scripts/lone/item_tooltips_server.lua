local SendItem = 251

local ExtendedEvent = CreatureEvent("ItemDataExtended")

function ExtendedEvent.onExtendedOpcode(player, opcode, buffer)
    if opcode == SendItem then
   
	local itemClientId = tonumber(buffer)
        if not itemClientId then
            return
        end
    local item = Game.getItemByClientId(itemClientId)
    local itemInfo = {}
	
	
if item then
    itemInfo.id = item:getId()
    itemInfo.clientId = itemClientId
    itemInfo.name = item:getName()
    itemInfo.attack = item:getAttack()
    itemInfo.defense = item:getDefense()
    itemInfo.extraDefense = item:getExtraDefense()
    itemInfo.speedAttack = item:getAttackSpeed() 
    itemInfo.description = item:getDescription()
    itemInfo.armor = item:getArmor()
    itemInfo.weight = item:getWeight() / 100 
    itemInfo.duration = item:getDuration()
    itemInfo.charge = item:getCharges()
    itemInfo.range = item:getShootRange()
    itemInfo.hitchance = item:getHitChance()

	local level = item:getRequiredLevel() or 0 
	if level == 0 then
		level = 1
	end
	itemInfo.level = level
	
	local vocation = item:getVocationString()
	if vocation == nil or vocation == "" then
		vocation = "Todas"
	end
	itemInfo.vocation = vocation

    if item:isContainer() then
        itemInfo.containerSize = item:getCapacity()
    end
end


        player:sendExtendedOpcode(SendItem, json.encode(itemInfo))
    end
end

local LoginEvent = CreatureEvent("ItemDataExtendedLogin")
function LoginEvent.onLogin(player)
  player:registerEvent("ItemDataExtended")
  return true
end

ExtendedEvent:type("extendedopcode")
ExtendedEvent:register()
LoginEvent:type("login")
LoginEvent:register()