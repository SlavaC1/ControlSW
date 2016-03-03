
#define IDS_APPLICATION_NAME      1
#define IDS_APPLICATION_VERSION   2
#define IDS_REQUEST_MESSAGE       3
#define IDS_RESPONSE_MESSAGE      4
#define IDS_PIPE_NAME             5

#define LOAD_STRING(resid)       (LoadStr(resid).c_str())
#define LOAD_INT(resid)          (LoadStr(resid).ToInt())

