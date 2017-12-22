#include "bot.h"
#include "clientImpl.h"

#include <time.h>
#include <string>
#include <vector>
#include <fstream>
#include <stdio.h>

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
#include <memory>
#include <iostream>
#include <sstream>

std::vector<std::string> split(std::string str, std::string sep){
    char* cstr = const_cast<char*>(str.c_str());
    char* current;
    std::vector<std::string> arr;
    current=strtok(cstr,sep.c_str());
    while(current!=NULL){
        arr.push_back(current);
        current=strtok(NULL,sep.c_str());
    }
    return arr;
}

using namespace Poco;
using namespace Poco::Net;

TelegramClientImpl::TelegramClientImpl(const std::string token, Bot* bot, const std::string host, int64_t timeout) : token_(token), update_id_(0), pHttp_(nullptr), bot_(bot), running_(true), timeout_(timeout), https_(false) {
    
    std::ifstream fin("settings.txt");
    fin >> update_id_;
    fin.close();
	
	const Poco::URI uri( host );
    
    if (uri.getScheme() == "http") {
		Poco::Net::Context::Ptr context (new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", ""));
		pHttp_ = std::unique_ptr<HTTPClientSession>(new HTTPClientSession(uri.getHost(), uri.getPort() ));
    } else {
        https_ = true;
    	Poco::Net::SSLManager::InvalidCertificateHandlerPtr ptrHandler(new Poco::Net::AcceptCertificateHandler(false));
		Poco::Net::Context::Ptr context (new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", ""));
		Poco::Net::SSLManager::instance().initializeClient(0, ptrHandler, context);

    	pHttp_ = std::unique_ptr<HTTPClientSession>((HTTPClientSession*)(new HTTPSClientSession(uri.getHost(), uri.getPort(), context )));
    }
}

TelegramClientImpl::~TelegramClientImpl() {
    
}

void TelegramClientImpl::start() {
	std::clog << "Checking connection...\n";
	while(!getMe()){
		sleep(1);
	}

	std::cout << "Starting loop...\n";
	loop();
}

void TelegramClientImpl::stop() {
	running_ = false;
}

void TelegramClientImpl::loop() {
	while (running_) {
		std::vector<Message> updates = getUpdates();

		for (const auto& update: updates) {
			std::cout << update.text << "\n"; 
			saveUpdateId(update.update_id + 1);
			
			bot_->handler(update);
		}

		sleep(1);
	}
}

void TelegramClientImpl::saveUpdateId(int64_t update_id) {
	update_id_ = update_id;
    
    std::ifstream fin("settings.txt");
    fin >> update_id_;
    fin.close();
}

void TelegramClientImpl::sendMessage(Message message) {
	std::string url = "/bot";
	url += token_;

	switch (message.type) {
		case Message::Type::Photo: {
			url += "/sendPhoto";
			break;
		}
		case Message::Type::Sticker: {
			url += "/sendSticker";
			break;
		}
		case Message::Type::Document: {
			url += "/sendDocument";
			break;
		}
		case Message::Type::Text: {
			url += "/sendMessage";
			break;
		}
		case Message::Type::NewPhoto: {
			url += "/sendPhoto";

			std::vector<std::pair<std::string, std::string>> args;

			args.push_back({"chat_id", std::to_string(message.chat_id)});

			uploadFile(message.photo, url, args);

			return;
		}
		case Message::Type::None:
		default: {
			return;
		}
	}

	std::istream& rs = sendRequest(url, message.stringify());
	parse(rs);
}


std::vector<Message> TelegramClientImpl::getUpdates() {
	std::string url = "/bot";
	url += token_ + "/getUpdates";

	if (timeout_ > 0 || update_id_ > 0) {
		url += "?";
	}

	if (update_id_ > 0) {
		url += "offset=" + std::to_string(update_id_);
	}

	if (timeout_ > 0 && update_id_ > 0) {
		url += "&";
	}

	if (timeout_ > 0) {
		url += "timeout=" + std::to_string(timeout_);
	}

	std::cout << "Getting updates for offset: " << update_id_ << "\n";

    std::istream& rs = sendRequest(url);
	auto [success, response] = parse(rs);

	std::vector<Message> updates;

	if (success) {
		for (const auto& update: response) {
			updates.push_back(Message(update));
		}

		if (updates.size() == 0) {
			std::cout  << "No new messages\n";
		}
	}

	return updates;
}

const std::string TelegramClientImpl::errorMessage(short code, const std::string description) {
	return "{\"ok\":false, \"error_code\": " + std::to_string(code) + ", \"description\":\"" + description + "\"}";
}

std::istream& TelegramClientImpl::sendRequest(const std::string url, const std::string body, const std::string contentType) {
	static std::stringstream empty_stream;
    
	try {
		const std::string method = body.size() == 0 ? Poco::Net::HTTPRequest::HTTP_GET : Poco::Net::HTTPRequest::HTTP_POST;
		
		Poco::Net::HTTPRequest req(method, url);

	    req.setKeepAlive(true);
	    req.setContentLength( body.length() );
	    req.setContentType(contentType);
        
        pHttp_->sendRequest(req) << body;
	    
	    Poco::Net::HTTPResponse res;
        std::istream* responseStream = &pHttp_->receiveResponse(res);
        
        if (res.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
            return *responseStream;
        } else {
            empty_stream.str(errorMessage(res.getStatus(), res.getReason()));
            return empty_stream;
        }
        
	} catch(Poco::Net::ConnectionRefusedException& e) {
		std::cout << "Error\n" << e.what() << "\n";
		empty_stream.str(errorMessage(e.code(), e.what()));
        pHttp_->reset();
	} catch(Poco::TimeoutException& e) {
		std::cout << "Error\n" << e.what() << "\n";
		empty_stream.str(errorMessage(e.code(), e.what()));
        pHttp_->reset();
	} catch(Poco::Exception& e) {
        std::cout << "Error\n" << e.what() << "\n";
		empty_stream.str(errorMessage(e.code(), e.what()));
        pHttp_->reset();
	}
    
    std::cout << "Exception\n";

	return empty_stream;
	
}

std::tuple<bool, Json::Value> TelegramClientImpl::parse(std::istream& stream) {
	std::string errs;
	Json::Value root;
	Json::CharReaderBuilder rbuilder;

	if (!Json::parseFromStream(rbuilder, stream, &root, &errs)) {
	    std::cout  << "Failed to parse response\n" << errs;
	    return {false, root};
	}

	bool success = root["ok"].asBool();
	
	if (success) {
		std::cout  << "Request success\n";
	} else {
		std::cout  << "Request failed\n" << "Code: " << root["error_code"] << "\n" << "Description: " << root["description"] << "\n";
	}

	return {success, root["result"]};
}

std::tuple<bool, std::string> TelegramClientImpl::downloadFile(const std::string fileId) {
	std::string url = "/bot";
	url += token_ + "/getFile?file_id=" + fileId;

	std::istream& file_id_stream = sendRequest(url);
	auto [file_id_success, file_id_response] = parse(file_id_stream);

	if (!file_id_success) {
		return {false, ""};
	}

	std::string file_path = file_id_response["file_path"].asString();

	url = "/file/bot";
	url += token_ + "/" + file_path;

	std::istream& file = sendRequest(url);

	std::fstream fs;

	auto p = split(file_path, "/");

	file_path = "/tmp/" + fileId + p[p.size()-1];

    fs.open (file_path, std::fstream::out | std::fstream::trunc | std::fstream::binary);

    Poco::StreamCopier::copyStream(file, fs);
    fs.close();

    return {true, file_path};
}

bool TelegramClientImpl::uploadFile(const std::string path, const std::string url, std::vector<std::pair<std::string, std::string>> args) {
	std::string contentType = "multipart/form-data; charset=utf-8; boundary=";
	std::string boundary = "__X_PAW_BOUNDARY__";

	std::string data1("--" + boundary + "\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"");
	
	auto p = split(path, "/");

	std::string data2(p[p.size()-1]);

	std::string data3("\";\r\nContent-Type: application/octet-stream\r\n\r\n");

	std::string data4("\r\n--" + boundary + "--\r\n");

	// Read File Data in binary
	std::ifstream file (path,std::fstream::binary);
	std::ostringstream ostrm;
	ostrm << file.rdbuf();

	std::string reqBody;

	for (const auto& a: args) {
		reqBody += "--" + boundary + "\r\n";
		reqBody += "Content-Disposition: form-data; name=\"";
		reqBody += a.first;
		reqBody += "\"\r\n\r\n";
		reqBody += a.second;
		reqBody += "\r\n";
	}

	reqBody.append(data1);
	reqBody.append(data2);
	reqBody.append(data3);
	reqBody.append(ostrm.str());
	reqBody.append(data4);

    std::istream& rs = sendRequest(url, reqBody, contentType + boundary);
	
	std::cout << "request sended\n";

	auto [success, _] = parse(rs);

	return success;
}

bool TelegramClientImpl::getMe() {
	std::string url = "/bot";
	url += token_ + "/getMe";

	std::istream& file_id_stream = sendRequest(url);
	
	auto [success, response] = parse(file_id_stream);

	if (success) {
		std::cout << "Bot info:" << "\n";
		std::cout << response << "\n";
	} else {
		std::cout << "Can't get bot info" << "\n";
	}

	return success;
}


