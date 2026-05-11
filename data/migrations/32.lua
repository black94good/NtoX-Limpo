function onUpdateDatabase()
	print("> Updating database to version 33 (in-game account manager)")

	local column = db.storeQuery("SHOW COLUMNS FROM `players` LIKE 'element'")
	if column then
		result.free(column)
	else
		db.query([[
			ALTER TABLE `players`
			ADD COLUMN `element` int NOT NULL DEFAULT 0 AFTER `vocation`;
		]])
	end

	db.query([[
		INSERT IGNORE INTO `accounts` (`id`, `name`, `password`, `secret`, `type`, `premium_ends_at`, `email`, `creation`)
		VALUES (1, '1', '356a192b7913b04c54574d18c28d46e6395428ab', NULL, 1, 0, '', 0);
	]])

	db.query([[
		INSERT IGNORE INTO `players` (`id`, `name`, `group_id`, `account_id`, `level`, `vocation`, `element`, `health`, `healthmax`, `experience`, `lookbody`, `lookfeet`, `lookhead`, `looklegs`, `looktype`, `lookaddons`, `currentmount`, `direction`, `maglevel`, `mana`, `manamax`, `manaspent`, `soul`, `town_id`, `posx`, `posy`, `posz`, `conditions`, `cap`, `sex`, `lastlogin`, `lastip`, `save`, `skull`, `skulltime`, `lastlogout`, `blessings`, `onlinetime`, `deletion`, `balance`, `offlinetraining_time`, `offlinetraining_skill`, `stamina`, `skill_fist`, `skill_fist_tries`, `skill_club`, `skill_club_tries`, `skill_sword`, `skill_sword_tries`, `skill_axe`, `skill_axe_tries`, `skill_dist`, `skill_dist_tries`, `skill_shielding`, `skill_shielding_tries`, `skill_fishing`, `skill_fishing_tries`)
		VALUES (1, 'Account Manager', 1, 1, 1, 0, 0, 150, 150, 0, 0, 0, 0, 0, 110, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, '', 400, 1, 0, 0x00, 1, 0, 0, 0, 0, 0, 0, 0, 43200, -1, 2520, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0);
	]])

	return true
end
