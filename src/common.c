#include "common.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> //inet_addr

size_t write_all(SOCKET fd, const char *buf, size_t buf_len)
{
    if (fd <= 0){
        return -1;
    }
    int write_len = 0;
    while (write_len < buf_len)
    {
        int len = 0;
        len = send(fd, buf + write_len, buf_len - write_len, 0);
        if (len < 0)
        {
            //LOGD("ERROR send[%d]:%s %d", len, strerror(len), buf_len);
            if (errno == EINTR)
            {
                continue;
            }
            else if (errno == EAGAIN) /* EAGAIN : Resource temporarily unavailable*/
            {
#ifdef WIN32
                Sleep(0);
#else
                sched_yield();
#endif
                continue;
            }
            else
            {
                return write_len;
            }
        }
        else
        {
            write_len = write_len + len;
        }
    }
    return write_len;
}

size_t read_all(SOCKET fd, char *buf, size_t buf_len)
{
    size_t len = 0, l = 0;
    do{
        l = recv(fd, buf + len, buf_len - len, 0);
        if(l == 0){
            return 0;
        }
        else if(l == (size_t)(-1)){
            int error = errno;
            //LOGE("read: %s", strerror(errno));
            if( error == EINTR || error == EWOULDBLOCK) {
                continue;
            }
            else if(error == EAGAIN){
#ifdef WIN32
                Sleep(0);
#else 
                sched_yield();
#endif
                continue;
                //break;
            }
            else {
                return 0;
            }
        }
        len += l;
    }while(buf_len != len);
    return len;
}
