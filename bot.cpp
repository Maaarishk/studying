#include "bot.h"
#include "message.h"
#include "time.h"
#include <iostream>
#include <cstdio>
#include <string>
#include "dither.h"
#include <stdlib.h>

Bot::Bot(const std::string token, std::string host, int64_t timeout) : client_(token, this, host, timeout) {
	srand(time(NULL));

}

void Bot::start() {
	client_.start();
}

const std::vector<RGB> pixels {
        {215, 198, 124},
        {174, 159, 109},
        {27, 24, 23},
        {10, 33, 55},
        {8, 9, 24},
        {8, 8, 8},
        {47, 40, 37},
        {165, 176, 172},
        {107, 89, 58},
        {177, 139, 62},
        {50, 73, 88},
        {229, 223, 186},
        {72, 60, 48},
        {102, 120, 124},
        {130, 115, 84}
    };

const std::vector<std::string> JOKES{
	"А ВЫ КОГДА-НИБУДЬ ВИДЕЛИ НАЗВАНИЯ ПЕРЕМЕННЫХ НА ПОЛЬСКОМ?",
	"ОНО КАК БЫ РАБОТАЕТ, НО НАДО ТЕСТИТЬ",
	"ТАК ИСТОРИЧЕСКИ СЛОЖИЛОСЬ НА ПРОЕКТЕ",
	"ТАМ ТУПО 5000 СТРОК КОДА В ФАЙЛЕ",
	"НУ, ТАМ КАК БЫ НОРМАЛЬНО ВСЕ..."
};
const size_t JOKES_COUNT = JOKES.size();


const std::vector<std::string> STICKERS{
	"AAQCABOVGEsNAAS4yUm2OSWMRAiJAAIC",
	"CAADAgADyAADMiv4Ck3tasrT8wdCAg",
	"CAADAgADMQADR_sJDMVDESlNQGdkAg",
	"CAADAgADygADMiv4Ckq6o5i6i0pdAg",
	"CAADAgADSwMAAoVUjAAB1Zra7-Y3rz4C",
	"CAADAgADxAUAAvoLtgiCOopVZ7H8dwI"
};
const size_t STICKERS_COUNT = STICKERS.size();

const std::vector<std::string> DOCUMENTS{
	"CgADBAADTpwAAt4dZAc7YC18vQbONgI",
	"CgADBAADFYcAAh8ZZAfJKbw2eZ9rdgI",
	"CgADBAAD7J4AAoAXZAd1OWEaF1eb7QI",
	"CgADBAADqZ8AArgZZAektaTt-5d21gI",
	"CgADBAADDa8AAooYZAdu2rXFy5xvNQI",
	"CgADBAADEp8AAi8XZAfFB96LQz_wywI"
};
const size_t DOCUMENTS_COUNT = DOCUMENTS.size();

void Bot::handler(Message message) {
	switch(message.type) {
		case Message::Type::Text: {
			if (message.text == "/random") {
				message.text = std::to_string(rand());
			} else if (message.text == "/weather") {
				message.text = "Winter Is Coming";
			} else if (message.text == "/styleguide") {
				message.text = JOKES[rand() % JOKES_COUNT];
			} else if (message.text == "/stop") {
				message.text = "Идет бычок, качается,\nВздыхает на ходу:\n- Ох, доска кончается,\nСейчас я упаду!";

				client_.stop();
			} else if (message.text == "/start") {
				message.text = "Hi!";
			} else if (message.text == "/crash") {
				abort();
			} else if (message.text == "/sticker") {
				message.sticker = STICKERS[rand() % STICKERS_COUNT];
				message.type = Message::Type::Sticker;
			} else if (message.text == "/gif") {
				message.document = DOCUMENTS[rand() % DOCUMENTS_COUNT];
				message.type = Message::Type::Document;
			} else if (message.text == "") {
				return;
			}


			break;
		}
		case Message::Type::Photo: {

			auto [success, path] = client_.downloadFile(message.photo);

			std::string convert = "convert ";
			convert += path;
			convert += " ";
			convert += path + ".png";

			system(convert.c_str());

    		Dither(path + ".png", path + "_ready.png", pixels);

			message.type = Message::Type::NewPhoto;
			message.photo = path + "_ready.png";

			break;
		}
		case Message::Type::Sticker: {
			break;
		}
		case Message::Type::Document: {
			break;
		}

		case Message::Type::None:
		default: {
			break;
		}
	}

	client_.sendMessage(message);
}
