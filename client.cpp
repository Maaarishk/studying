#include "clientImpl.h"
#include "client.h"

TelegramClient::TelegramClient(const std::string token, Bot* bot, const std::string host, int64_t timeout) : 
	pImpl_(new TelegramClientImpl(token, bot, host, timeout))
{
    
}

TelegramClient::~TelegramClient() { }

void TelegramClient::start() {
	pImpl_->start();
}

void TelegramClient::stop() {
	pImpl_->stop();
}

void TelegramClient::loop() {
	pImpl_->loop();
}

void TelegramClient::sendMessage(Message message) {
	pImpl_->sendMessage(message);
}

std::tuple<bool, std::string> TelegramClient::downloadFile(const std::string fileId) {
	return pImpl_->downloadFile(fileId);
}

bool TelegramClient::getMe() {
	return pImpl_->getMe();
}


