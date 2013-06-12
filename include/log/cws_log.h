#ifndef CWS_LOG_H
#define CWS_LOG_H

typedef enum 
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
} LOG_LEVEL;

void cws_log(LOG_LEVEL level, const char* message);

#endif /* CWS_LOG_H */
