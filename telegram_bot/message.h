#include <json/json.h>

struct Message {
	Message(const Json::Value update);

	std::string stringify();

	int64_t update_id;
	int64_t message_id;
	int64_t chat_id;

	std::string text;
	std::string photo;
	std::string sticker;
	std::string document;

	enum Type {
		None, Text, Photo, Sticker, Document, NewPhoto
	};

	Type type;
};

