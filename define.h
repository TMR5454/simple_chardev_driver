#define DRIVERNAME "simple_char"
#define BUF_SIZE 256

struct simple_char_private_data {
    char *buffer;
    struct mutex buffer_lock;
};
