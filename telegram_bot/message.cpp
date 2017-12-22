#include "message.h"
#include <iostream>
/*
	{
		"update_id":254869990,
		"message": {
			"message_id":3,
			"from":{
				"id":256011953,
				"is_bot":false,
				"first_name":"Marina",
				"last_name":"Bakhmetyeva",
				"username":"Maaarishk",
				"language_code":"en"
			},
			"chat":{
				"id":256011953,
				"first_name":"Marina",
				"last_name":"Bakhmetyeva",
				"username":"Maaarishk",
				"type":"private"
			},
			"date":1512336315,
			"text":"how r u"
		}
	}
*/

Message::Message(const Json::Value update) :
	update_id(update["update_id"].asInt64()),
	chat_id(update["message"]["chat"]["id"].asInt64()),
	text(update["message"]["text"].asString()),
	message_id(update["message"]["message_id"].asInt64()),
	type(Type::Text)
{
	if (update["message"].isMember("photo")) {
		int max_size = 0;

		for (const auto& pic: update["message"]["photo"]) {
			if (pic["file_size"].asInt64() > max_size) {
				max_size = pic["file_size"].asInt64();
				photo = pic["file_id"].asString();
			}
		}

		type = Type::Photo;
	}

	if (update["message"].isMember("sticker")) {
		sticker = update["message"]["sticker"]["file_id"].asString();
		type = Type::Sticker;
		text = "";
	}

	if (update["message"].isMember("document")) {
		document = update["message"]["document"]["file_id"].asString();
		type = Type::Document;
		text = "";
	}
}

std::string Message::stringify(){
		std::string str = "{";

		str += "\"chat_id\":" + std::to_string(chat_id) + ",";
		str += "\"text\":\"" + text + "\",";
		str += "\"photo\":\"" + photo + "\",";
		str += "\"caption\":\"" + text + "\",";
		str += "\"sticker\":\"" + sticker + "\",";
		str += "\"document\":\"" + document + "\",";
		str += "\"reply_to_message_id\":" + std::to_string(message_id);

		str += "}";

		return str;
	}