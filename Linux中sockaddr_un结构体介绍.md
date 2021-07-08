首先说一下Unix域协议

Unix域协议并不是一个实际的协议族，它只是在同一台主机上进行客户-服务器通信时，使用与在不同主机上的客户和服务器间通信时相同的API(套接口或XTI)的一种方法。

当客户和服务器在同一台主机上时，Unix域协议是IPC通信方式的一种替代品。

Unix域提供了两种类型的套接口：字节流套接口(与TCP类似)和数据报套接口(与UDP类似)。

1. Unix域套接口地址结构(在《sys/un.h>头文件中定义的Unix域套接口地址结构)
```C++
struct sockaddr_un {
	unsigned char   sun_len;  
	unsigned short 	sun_family; 	/* AF_LOCAL */  
			 char   sun_path[104]; 	/* null-terminated pathname */
};
```

由上可得sockaddr是用在同一台主机上的通信的。

```C++
struct sockaddr {
	unsigned short sa_family;     /* address family, AF_xxx */
			 char  sa_data[14];   /* 14 bytes of protocol address */
};
```
sa_family是地址家族，一般都是“AF_xxx”的形式。好像通常大多用的是都是AF_INET。
sa_data是14字节协议地址。此数据结构用做bind、connect、recvfrom、sendto等函数的参数，指明地址信息。但一般编程中并不直接针对此数据结构操作，而是使用另一个与sockaddr等价的数据结构`struct sockaddr_in`。

```C++
typedef struct in_addr {
	union {
		struct {
			unsigned char 	s_b1,
							s_b2,
							s_b3,
							s_b4;
		} S_un_b;
		struct {
			unsigned short 	s_w1,
							s_w2;
		} S_un_w;
		unsigned long 	S_addr;
	} S_un;
} IN_ADDR;

struct sockaddr_in {
			 short int sin_family;     /* Address family */
	unsigned short int sin_port;       /* Port number */
	struct in_addr     sin_addr;       /* Internet address */
	unsigned char      sin_zero[8];    /* Same size as struct sockaddr */
};
```
sin_family指代协议族，在socket编程中只能是AF_INET
sin_port存储端口号（使用网络字节顺序）
sin_addr存储IP地址，使用in_addr这个数据结构
sin_zero是为了让sockaddr与sockaddr_in两个数据结构保持大小相同而保留的空字节。
s_addr按照网络字节顺序存储IP地址

sockaddr_in和sockaddr是并列的结构，指向sockaddr_in的结构体的指针也可以指向
sockadd的结构体，并代替它。
