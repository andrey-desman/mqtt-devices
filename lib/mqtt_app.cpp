#include "mqtt_app.h"
#include "log.h"

namespace
{

void sighandler(ev::sig& sig, int revents)
{
	LOG(info, "received signal '%s'", strsignal(sig.signum));
	sig.loop.break_loop();
}

}

mqtt_app::mqtt_app(ev::loop_ref& loop, const std::string& uri, const std::string& client_id)
	: loop_(loop)
	, caller_(loop_)
	, int_watcher_(loop_)
	, term_watcher_(loop_)
	, client_(uri, client_id, nullptr)
	, connection_(client_, loop_, caller_)
{
	int_watcher_.set<sighandler>();
	int_watcher_.start(SIGINT);

	term_watcher_.set<sighandler>();
	term_watcher_.start(SIGTERM);
}

void mqtt_app::run()
{
	LOG(debug, "running...");
	connection_.connect();
	loop_.run();
}

