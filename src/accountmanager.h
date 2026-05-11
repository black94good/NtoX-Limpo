// Copyright 2023 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_ACCOUNT_MANAGER_H
#define FS_ACCOUNT_MANAGER_H

#include "position.h"

#include <string>
#include <string_view>
#include <vector>

struct AccountManagerCharacterOption
{
	uint32_t id = 0;
	std::string name;
	uint16_t vocation = 0;
	uint16_t element = 0;
	bool chooseElement = true;
	bool sex = false;
	bool premium = false;
	std::vector<uint32_t> towns;
	Position spawnPosition;
	bool useSpawnPosition = false;
	uint16_t outfit[7] = {0, 0, 0, 0, 0, 0, 0};
	uint16_t skills[7] = {10, 10, 10, 10, 10, 10, 10};
	uint32_t magicLevel = 0;
};

class AccountManager
{
public:
	AccountManager() = delete;

	enum Button : uint8_t {
		BUTTON_NONE,
		BUTTON_PRIMARY,
		BUTTON_SECONDARY,
		BUTTON_TERTIARY
	};

	enum Choice : uint8_t {
		CHOICE_NONE,
		CHOICE_FIRST,
		CHOICE_SECOND,
		CHOICE_THIRD
	};

	enum TextWindow : uint32_t {
		TEXT_ACCOUNT_NAME = 0xA001,
		TEXT_PASSWORD = 0xA002,
		TEXT_PASSWORD_CONFIRM = 0xA003,
		TEXT_CHARACTER_NAME = 0xA004,
		TEXT_PASSWORD_RESET = 0xA005,
		TEXT_PASSWORD_RESET_CONFIRM = 0xA006,
		TEXT_EMAIL = 0xA007,
		TEXT_EMAIL_CODE = 0xA008,
		TEXT_RECOVERY_ACCOUNT = 0xA009,
		TEXT_RECOVERY_EMAIL = 0xA00A,
		TEXT_RECOVERY_CODE = 0xA00B
	};

	enum Window : uint32_t {
		WINDOW_COMMON_LOGIN = 0xB001,
		WINDOW_COMMON_ACCOUNT_RULES,
		WINDOW_COMMON_PASSWORD_RULES,
		WINDOW_COMMON_PASSWORD_CONFIRM,
		WINDOW_COMMON_SUCCESS,
		WINDOW_COMMON_ACCOUNT_FAILED,
		WINDOW_COMMON_PASSWORD_FAILED,
		WINDOW_COMMON_PASSWORD_MISMATCH,
		WINDOW_COMMON_EMAIL_RULES,
		WINDOW_COMMON_EMAIL_FAILED,
		WINDOW_COMMON_EMAIL_CODE,
		WINDOW_COMMON_EMAIL_CODE_FAILED,
		WINDOW_COMMON_EMAIL_SEND_FAILED,
		WINDOW_COMMON_RECOVERY_ACCOUNT,
		WINDOW_COMMON_RECOVERY_EMAIL,
		WINDOW_COMMON_RECOVERY_FAILED,
		WINDOW_COMMON_RECOVERY_CODE,
		WINDOW_COMMON_RECOVERY_CODE_FAILED,
		WINDOW_COMMON_RECOVERY_SEND_FAILED,
		WINDOW_COMMON_RECOVERY_PASSWORD,
		WINDOW_COMMON_RECOVERY_PASSWORD_CONFIRM,
		WINDOW_COMMON_RECOVERY_PASSWORD_FAILED,
		WINDOW_COMMON_RECOVERY_PASSWORD_MISMATCH,
		WINDOW_COMMON_RECOVERY_SUCCESS,

		WINDOW_PRIVATE_MENU = 0xB100,
		WINDOW_PRIVATE_CHARACTER_OPTION,
		WINDOW_PRIVATE_CHARACTER_TOWN,
		WINDOW_PRIVATE_CHARACTER_ELEMENT,
		WINDOW_PRIVATE_CHARACTER_NAME,
		WINDOW_PRIVATE_CHARACTER_FAILED,
		WINDOW_PRIVATE_CHARACTER_SUCCESS,
		WINDOW_PRIVATE_ACCOUNT_RULES,
		WINDOW_PRIVATE_ACCOUNT_FAILED,
		WINDOW_PRIVATE_PASSWORD_RULES,
		WINDOW_PRIVATE_PASSWORD_CONFIRM,
		WINDOW_PRIVATE_PASSWORD_SUCCESS,
		WINDOW_PRIVATE_PASSWORD_FAILED,
		WINDOW_PRIVATE_PASSWORD_MISMATCH,
		WINDOW_PRIVATE_PASSWORD_RESET,
		WINDOW_PRIVATE_PASSWORD_RESET_CONFIRM,
		WINDOW_PRIVATE_PASSWORD_RESET_SUCCESS,
		WINDOW_PRIVATE_PASSWORD_RESET_FAILED,
		WINDOW_PRIVATE_PASSWORD_RESET_MISMATCH
	};

	static bool loadFromXml();
	static bool isEnabled();
	static bool allowNoPasswordLogin();
	static const std::string& getAuthPassword();
	static const Position& getPosition();
	static bool isAccountManager(uint32_t guid, std::string_view name);
	static bool isValidRegistrationText(std::string_view text);
	static bool isValidCharacterName(std::string_view text);
	static bool isValidEmail(std::string_view text);
	static std::string generateEmailCode();
	static bool sendEmailCode(const std::string& recipient, const std::string& subject, const std::string& code);

	static const std::vector<AccountManagerCharacterOption>& getCharacterOptions();
	static const AccountManagerCharacterOption* getCharacterOption(uint32_t id);

	static constexpr uint32_t AccountId = 1;
	static constexpr uint32_t PlayerId = 1;
	static constexpr std::string_view Name = "Account Manager";
};

#endif
