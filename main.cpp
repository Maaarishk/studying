#include "bot.h"

const std::string TOKEN = "HERE MUST BE TOKEN";

int main() {
    Bot bot(TOKEN);
    bot.start();
    
    return 0;
}

// download
// ./configure --static
// xcode-select --install
// sudo xcode-select --switch /Library/Developer/CommandLineTools/

