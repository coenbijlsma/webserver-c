#include <syslog.h>
#include "log/cws_log.h"

void cws_log(LOG_LEVEL level, const char* message) {

    openlog("testapp", LOG_PID | LOG_CONS, LOG_USER);

    switch(level) {
        case DEBUG:
            syslog(LOG_DEBUG, "%s", message); break;
        case INFO:
            syslog(LOG_INFO, "%s", message); break;
        case WARNING:
            syslog(LOG_WARNING, "%s", message); break;
        case ERROR:
            syslog(LOG_ERR, "%s", message); break;
        case FATAL:
            syslog(LOG_CRIT, "%s", message); break;
    }

    closelog();
}
