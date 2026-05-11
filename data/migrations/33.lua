function onUpdateDatabase()
	print("> Updating database to version 34 (account manager email verification)")

	local column = db.storeQuery("SHOW COLUMNS FROM `accounts` LIKE 'email_verified'")
	if column then
		result.free(column)
	else
		db.query([[
			ALTER TABLE `accounts`
			ADD COLUMN `email_verified` tinyint(1) NOT NULL DEFAULT 0 AFTER `email`;
		]])
	end

	return true
end
