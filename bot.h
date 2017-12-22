#include "client.h"

class Bot {
public:
	Bot(const std::string token, std::string host = "https://api.telegram.org", int64_t timeout = 0);

	virtual void handler(Message message);
	void start();
protected:
	TelegramClient client_;
};
