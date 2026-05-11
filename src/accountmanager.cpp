// Copyright 2023 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "accountmanager.h"

#include "configmanager.h"
#include "pugicast.h"
#include "tools.h"

#include <algorithm>
#include <array>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <cctype>
#include <chrono>
#include <openssl/ssl.h>

namespace {
	bool enabled = true;
	bool noPasswordLogin = true;
	std::string authPassword = "1";
	Position managerPosition;
	std::vector<AccountManagerCharacterOption> characterOptions;

	bool parseBoolAttribute(const pugi::xml_node& node, const char* name, bool defaultValue)
	{
		const auto attr = node.attribute(name);
		return attr ? booleanString(attr.as_string()) : defaultValue;
	}

	void loadTownList(AccountManagerCharacterOption& option, const pugi::xml_node& node)
	{
		for (const auto townNode : node.children("town")) {
			if (const auto id = townNode.attribute("id")) {
				option.towns.push_back(pugi::cast<uint32_t>(id.value()));
			}
		}
	}

	std::string base64Encode(std::string_view input)
	{
		static constexpr char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		std::string output;
		output.reserve(((input.size() + 2) / 3) * 4);

		for (size_t i = 0; i < input.size(); i += 3) {
			const uint32_t octetA = static_cast<unsigned char>(input[i]);
			const uint32_t octetB = i + 1 < input.size() ? static_cast<unsigned char>(input[i + 1]) : 0;
			const uint32_t octetC = i + 2 < input.size() ? static_cast<unsigned char>(input[i + 2]) : 0;
			const uint32_t triple = (octetA << 16) | (octetB << 8) | octetC;

			output.push_back(table[(triple >> 18) & 0x3F]);
			output.push_back(table[(triple >> 12) & 0x3F]);
			output.push_back(i + 1 < input.size() ? table[(triple >> 6) & 0x3F] : '=');
			output.push_back(i + 2 < input.size() ? table[triple & 0x3F] : '=');
		}
		return output;
	}

	template <typename Stream>
	int32_t readSmtpResponse(Stream& stream, std::string& buffer)
	{
		int32_t code = 0;
		for (;;) {
			boost::asio::read_until(stream, boost::asio::dynamic_buffer(buffer), "\r\n");
			const auto end = buffer.find("\r\n");
			if (end == std::string::npos) {
				return 0;
			}

			const std::string line = buffer.substr(0, end);
			buffer.erase(0, end + 2);
			if (line.size() >= 3) {
				code = std::atoi(line.substr(0, 3).c_str());
				if (line.size() < 4 || line[3] == ' ') {
					return code;
				}
			}
		}
	}

	template <typename Stream>
	bool sendSmtpLine(Stream& stream, std::string_view line)
	{
		boost::asio::write(stream, boost::asio::buffer(line));
		boost::asio::write(stream, boost::asio::buffer("\r\n", 2));
		return true;
	}

	bool smtpCodeOk(int32_t code, std::initializer_list<int32_t> expected)
	{
		return std::find(expected.begin(), expected.end(), code) != expected.end();
	}

	template <typename Stream>
	bool finishSmtpSession(Stream& stream, std::string& buffer, const std::string& host, const std::string& from,
	                       const std::string& recipient, const std::string& subject, const std::string& body)
	{
		sendSmtpLine(stream, "EHLO " + host);
		if (!smtpCodeOk(readSmtpResponse(stream, buffer), {250})) {
			return false;
		}

		const std::string& user = ConfigManager::getString(ConfigManager::SMTP_USER);
		const std::string& password = ConfigManager::getString(ConfigManager::SMTP_PASSWORD);
		if (!user.empty()) {
			sendSmtpLine(stream, "AUTH LOGIN");
			if (!smtpCodeOk(readSmtpResponse(stream, buffer), {334})) {
				return false;
			}
			sendSmtpLine(stream, base64Encode(user));
			if (!smtpCodeOk(readSmtpResponse(stream, buffer), {334})) {
				return false;
			}
			sendSmtpLine(stream, base64Encode(password));
			if (!smtpCodeOk(readSmtpResponse(stream, buffer), {235})) {
				return false;
			}
		}

		sendSmtpLine(stream, "MAIL FROM:<" + from + ">");
		if (!smtpCodeOk(readSmtpResponse(stream, buffer), {250})) {
			return false;
		}
		sendSmtpLine(stream, "RCPT TO:<" + recipient + ">");
		if (!smtpCodeOk(readSmtpResponse(stream, buffer), {250, 251})) {
			return false;
		}
		sendSmtpLine(stream, "DATA");
		if (!smtpCodeOk(readSmtpResponse(stream, buffer), {354})) {
			return false;
		}

		std::string fromName = ConfigManager::getString(ConfigManager::SMTP_FROM_NAME);
		if (fromName.empty()) {
			fromName = "Account Manager";
		}

		std::string message;
		message.reserve(body.size() + 256);
		message += "From: " + fromName + " <" + from + ">\r\n";
		message += "To: <" + recipient + ">\r\n";
		message += "Subject: " + subject + "\r\n";
		message += "Content-Type: text/plain; charset=UTF-8\r\n";
		message += "\r\n";
		message += body;
		message += "\r\n.";
		sendSmtpLine(stream, message);
		if (!smtpCodeOk(readSmtpResponse(stream, buffer), {250})) {
			return false;
		}

		sendSmtpLine(stream, "QUIT");
		readSmtpResponse(stream, buffer);
		return true;
	}
}

bool AccountManager::loadFromXml()
{
	characterOptions.clear();

	pugi::xml_document doc;
	const pugi::xml_parse_result result = doc.load_file("data/XML/account_manager.xml");
	if (!result) {
		printXMLError("Error - AccountManager::loadFromXml", "data/XML/account_manager.xml", result);
		return false;
	}

	const auto root = doc.child("accountmanager");
	if (!root) {
		std::cout << "[Error - AccountManager::loadFromXml] Missing accountmanager root node." << std::endl;
		return false;
	}

	if (const auto configNode = root.child("config")) {
		enabled = parseBoolAttribute(configNode, "enabled", true);
		noPasswordLogin = parseBoolAttribute(configNode, "allowNoPasswordLogin", true);
		if (const auto auth = configNode.attribute("authPassword")) {
			authPassword = auth.as_string();
		}

		if (const auto positionNode = configNode.child("position")) {
			managerPosition.x = pugi::cast<uint16_t>(positionNode.attribute("x").value());
			managerPosition.y = pugi::cast<uint16_t>(positionNode.attribute("y").value());
			managerPosition.z = pugi::cast<uint8_t>(positionNode.attribute("z").value());
		}
	}

	uint32_t optionId = 0;
	for (const auto characterNode : root.children("character")) {
		AccountManagerCharacterOption option;
		option.id = optionId++;
		option.name = characterNode.attribute("name").as_string();
		option.vocation = pugi::cast<uint16_t>(characterNode.attribute("vocation").value());
		option.premium = parseBoolAttribute(characterNode, "premium", false);
		option.chooseElement = parseBoolAttribute(characterNode, "chooseElement", true);

		const std::string sex = characterNode.attribute("sex").as_string("female");
		option.sex = caseInsensitiveEqual(sex, "male");

		if (const auto element = characterNode.attribute("element")) {
			option.element = pugi::cast<uint16_t>(element.value());
			option.chooseElement = option.element == 0 && option.chooseElement;
		}

		if (const auto townsNode = characterNode.child("towns")) {
			loadTownList(option, townsNode);
		}

		if (const auto spawnNode = characterNode.child("spawn")) {
			option.spawnPosition.x = pugi::cast<uint16_t>(spawnNode.attribute("x").value());
			option.spawnPosition.y = pugi::cast<uint16_t>(spawnNode.attribute("y").value());
			option.spawnPosition.z = pugi::cast<uint8_t>(spawnNode.attribute("z").value());
			option.useSpawnPosition = true;
		}

		if (const auto outfitNode = characterNode.child("outfit")) {
			option.outfit[0] = pugi::cast<uint16_t>(outfitNode.attribute("id").value());
			option.outfit[1] = pugi::cast<uint16_t>(outfitNode.attribute("head").value());
			option.outfit[2] = pugi::cast<uint16_t>(outfitNode.attribute("body").value());
			option.outfit[3] = pugi::cast<uint16_t>(outfitNode.attribute("legs").value());
			option.outfit[4] = pugi::cast<uint16_t>(outfitNode.attribute("feet").value());
			option.outfit[5] = pugi::cast<uint16_t>(outfitNode.attribute("addons").value());
			option.outfit[6] = pugi::cast<uint16_t>(outfitNode.attribute("mount").value());
		}

		if (const auto skillsNode = characterNode.child("skills")) {
			option.skills[SKILL_FIST] = pugi::cast<uint16_t>(skillsNode.attribute("fist").as_string("10"));
			option.skills[SKILL_CLUB] = pugi::cast<uint16_t>(skillsNode.attribute("club").as_string("10"));
			option.skills[SKILL_SWORD] = pugi::cast<uint16_t>(skillsNode.attribute("sword").as_string("10"));
			option.skills[SKILL_AXE] = pugi::cast<uint16_t>(skillsNode.attribute("axe").as_string("10"));
			option.skills[SKILL_DISTANCE] = pugi::cast<uint16_t>(skillsNode.attribute("distance").as_string("10"));
			option.skills[SKILL_SHIELD] = pugi::cast<uint16_t>(skillsNode.attribute("shield").as_string("10"));
			option.skills[SKILL_FISHING] = pugi::cast<uint16_t>(skillsNode.attribute("fishing").as_string("10"));
			option.magicLevel = pugi::cast<uint32_t>(skillsNode.attribute("magiclevel").as_string("0"));
		}

		if (option.name.empty()) {
			option.name = "Character";
		}
		characterOptions.push_back(option);
	}

	return true;
}

bool AccountManager::isEnabled()
{
	return enabled;
}

bool AccountManager::allowNoPasswordLogin()
{
	return noPasswordLogin;
}

const std::string& AccountManager::getAuthPassword()
{
	return authPassword;
}

const Position& AccountManager::getPosition()
{
	return managerPosition;
}

bool AccountManager::isAccountManager(uint32_t guid, std::string_view name)
{
	return guid == PlayerId || name == Name;
}

bool AccountManager::isValidRegistrationText(std::string_view text)
{
	if (text.length() < 6 || text.length() > 29) {
		return false;
	}

	return std::ranges::all_of(text, [](unsigned char ch) {
		return std::isalnum(ch) != 0;
	});
}

bool AccountManager::isValidCharacterName(std::string_view text)
{
	if (text.length() < 6 || text.length() > 29 || text.front() == ' ' || text.back() == ' ') {
		return false;
	}

	bool lastWasSpace = false;
	for (unsigned char ch : text) {
		if (ch == ' ') {
			if (lastWasSpace) {
				return false;
			}
			lastWasSpace = true;
			continue;
		}

		if (std::isalnum(ch) == 0) {
			return false;
		}
		lastWasSpace = false;
	}
	return true;
}

bool AccountManager::isValidEmail(std::string_view text)
{
	if (text.length() < 6 || text.length() > 255 || text.front() == '.' || text.back() == '.') {
		return false;
	}

	const auto at = text.find('@');
	if (at == std::string_view::npos || at == 0 || at != text.rfind('@') || at + 1 >= text.length()) {
		return false;
	}

	const auto dot = text.find('.', at + 2);
	if (dot == std::string_view::npos || dot + 1 >= text.length()) {
		return false;
	}

	return std::ranges::all_of(text, [](unsigned char ch) {
		return std::isalnum(ch) != 0 || ch == '@' || ch == '.' || ch == '_' || ch == '-' || ch == '+';
	});
}

std::string AccountManager::generateEmailCode()
{
	return std::to_string(uniform_random(100000, 999999));
}

bool AccountManager::sendEmailCode(const std::string& recipient, const std::string& subject, const std::string& code)
{
	const std::string& host = ConfigManager::getString(ConfigManager::SMTP_HOST);
	std::string from = ConfigManager::getString(ConfigManager::SMTP_FROM);
	if (host.empty() || from.empty()) {
		std::cout << "[Warning - AccountManager::sendEmailCode] SMTP is not configured." << std::endl;
		return false;
	}

	const std::string body = "Seu codigo de confirmacao e: " + code + "\r\n\r\nEste codigo expira em 10 minutos.";

	try {
		boost::asio::io_context io;
		boost::asio::ip::tcp::resolver resolver(io);
		boost::asio::ip::tcp::socket socket(io);
		boost::asio::connect(socket, resolver.resolve(host, std::to_string(ConfigManager::getNumber(ConfigManager::SMTP_PORT))));

		std::string buffer;
		if (!smtpCodeOk(readSmtpResponse(socket, buffer), {220})) {
			return false;
		}

		if (ConfigManager::getBoolean(ConfigManager::SMTP_USE_STARTTLS)) {
			sendSmtpLine(socket, "EHLO " + host);
			if (!smtpCodeOk(readSmtpResponse(socket, buffer), {250})) {
				return false;
			}

			sendSmtpLine(socket, "STARTTLS");
			if (!smtpCodeOk(readSmtpResponse(socket, buffer), {220})) {
				return false;
			}

			boost::asio::ssl::context sslContext(boost::asio::ssl::context::tls_client);
			sslContext.set_verify_mode(boost::asio::ssl::verify_none);
			boost::asio::ssl::stream<boost::asio::ip::tcp::socket> sslStream(std::move(socket), sslContext);
			SSL_set_tlsext_host_name(sslStream.native_handle(), host.c_str());
			sslStream.handshake(boost::asio::ssl::stream_base::client);
			return finishSmtpSession(sslStream, buffer, host, from, recipient, subject, body);
		}

		return finishSmtpSession(socket, buffer, host, from, recipient, subject, body);
	} catch (const std::exception& e) {
		std::cout << "[Warning - AccountManager::sendEmailCode] " << e.what() << std::endl;
		return false;
	}
}

const std::vector<AccountManagerCharacterOption>& AccountManager::getCharacterOptions()
{
	return characterOptions;
}

const AccountManagerCharacterOption* AccountManager::getCharacterOption(uint32_t id)
{
	if (id >= characterOptions.size()) {
		return nullptr;
	}
	return &characterOptions[id];
}
