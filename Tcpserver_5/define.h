#pragma pack(1)   //按照单字节对齐 
struct Header {
	int num;    //包id 
	int index;  //学生编号 
};
struct PkgContent {
	char sex;  //性别 
	int score;  //分数 
	char address[100];  //地址 
	int age;  //年龄 
};
struct Pkg {
	Header head;
	PkgContent content;
};
//发送 指定长度的数据包 
int Mysend(int sock, char * pchBuf, size_t tLen) {
	int iThisSend;
	unsigned int iSended = 0;
	if (tLen == 0)
		return 0;
	while (iSended < tLen) {
		do {
			iThisSend = send(sock, pchBuf, tLen-iSended, 0);
		}while ((iThisSend < 0) && (errno == EINTR));
		if (iThisSend < 0) {
			return (iSended);
		}
		iSended += iThisSend;
		pchBuf += iThisSend;
	}
	return (tLen);
}
//接收指定长度的数据包 
int MyRecv(int isock, char * pchBuf, size_t tCount) {
    size_t tByteRead = 0;
    int iThisRead;
    while (tByteRead < tCount) {
        do {
            iThisRead = read(isock, pchBuf, tCount - iThisRead);
        }while ((iThisRead < 0) && (errno == EINTR));
        if (iThisRead < 0) {
            return (iThisRead);
        }
        else if (iThisRead == 0)
            return (tByteRead);
        tByteRead += iThisRead;
        pchBuf += iThisRead;
    }
}
