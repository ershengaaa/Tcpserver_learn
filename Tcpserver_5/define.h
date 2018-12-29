#pragma pack(1)   //���յ��ֽڶ��� 
struct Header {
	int num;    //��id 
	int index;  //ѧ����� 
};
struct PkgContent {
	char sex;  //�Ա� 
	int score;  //���� 
	char address[100];  //��ַ 
	int age;  //���� 
};
struct Pkg {
	Header head;
	PkgContent content;
};
//���� ָ�����ȵ����ݰ� 
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
//����ָ�����ȵ����ݰ� 
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
