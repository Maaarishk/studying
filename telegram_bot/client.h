#include <string>
#include <vector>
#include <json/json.h>
#include <memory>

class Bot;
class TelegramClientImpl;
class Message;

class TelegramClient {
public:
	TelegramClient(const std::string token, Bot* bot, const std::string host = "https://api.telegram.org", int64_t timeout = 0);

	~TelegramClient();

	void start();
	void stop();

	void loop();
	bool getMe();

	std::tuple<bool, std::string> downloadFile(const std::string fileId);
	
	void sendMessage(Message message);
private:

	std::unique_ptr<TelegramClientImpl> pImpl_;
};
