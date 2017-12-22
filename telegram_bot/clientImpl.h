#include <string>
#include <vector>
#include <json/json.h>
#include <memory>

#include <Poco/StreamCopier.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>
#include <Poco/SharedPtr.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/KeyConsoleHandler.h>
#include <Poco/Net/ConsoleCertificateHandler.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPSStreamFactory.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/SSLException.h>
#include <Poco/Net/AcceptCertificateHandler.h>

#include "message.h"


class Bot;

class TelegramClientImpl {
public:
	TelegramClientImpl(const std::string token, Bot* bot, const std::string host = "https://api.telegram.org", int64_t timeout = 0);

	~TelegramClientImpl();

	void start();
	void stop();

	void loop();
	bool getMe();

	std::tuple<bool, std::string> downloadFile(const std::string fileId);
	
	void sendMessage(Message message);
private:

	std::vector<Message> getUpdates();

	void saveUpdateId(int64_t update_id);

	std::istream& sendRequest(const std::string url, const std::string body = "", const std::string contentType = "application/json");

	std::tuple<bool, Json::Value> parse(std::istream& stream);

	bool uploadFile(const std::string path, const std::string url, std::vector<std::pair<std::string, std::string>> args);

	const std::string errorMessage(short code, const std::string description);

	int64_t update_id_;
	std::string token_;

	std::unique_ptr<Poco::Net::HTTPClientSession> pHttp_;

	Bot* bot_;

	bool running_;

	int64_t timeout_;
    bool https_;
};
