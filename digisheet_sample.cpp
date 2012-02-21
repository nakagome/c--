#include <stdio.h>
#include <winsock2.h>
#include <string>

//	�ݒ�l

//	Proxy
const std::string strProxyAddress = "XXX.XXX.XXX.XXX";
const int nProxyPort = 8080;

//	�ڑ����[�U�[���
//	�h���� CD
const std::string strBaseCD = "XXXX";
//	�X�^�b�tID
const std::string strStaffID = "XXXXXXX";
//	�p�X���[�h
const std::string strPassword = "XXXXXXXXX";

//	POST���āArecv�����擾
std::string getPostData( SOCKET sock, const std::string& strPostData )
{
	std::string strRecvData;

	// HTTP���N�G�X�g���M
	int n = send(sock, strPostData.c_str(), strPostData.size(), 0);
	if (n < 0) {
		printf("send : %d\n", WSAGetLastError());
		return strRecvData;
	}
	
	// �T�[�o�����HTTP���b�Z�[�W��M
	char buf[1024];
	while (n > 0) {
		memset(buf, 0, sizeof(buf));
		n = recv(sock, buf, sizeof(buf), 0);
		if (n < 0) {
			printf("recv : %d\n", WSAGetLastError());
			return strRecvData;
		}
		// ��M���ʂ�\��
		//fwrite(buf, n, 1, stdout);

		//	��M�f�[�^�ɒǉ�
		strRecvData.append(buf,n);
	}

	//	�f�[�^��Ԃ�
	return strRecvData;
}

//	�\�P�b�g���쐬����
//		����:nError==0
int makeSocket(
		SOCKET*				pSock,				//	�\�P�b�g
		const std::string&	strServerAddress,	//	�ڑ���A�h���X
		int					nServerPort			//	�ڑ���|�[�g
)
{

	struct sockaddr_in server;
	unsigned int **addrptr;

	*pSock = socket(AF_INET, SOCK_STREAM, 0);
	if (*pSock == INVALID_SOCKET) {
		printf("socket : %d\n", WSAGetLastError());
		return 1;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(nServerPort); // �ڑ��|�[�g

	server.sin_addr.S_un.S_addr = inet_addr(strServerAddress.c_str());
	if (server.sin_addr.S_un.S_addr == 0xffffffff) {
		struct hostent *host;

		host = gethostbyname(strServerAddress.c_str());	//	�ڑ���A�h���X

		if (host == NULL) {
			if (WSAGetLastError() == WSAHOST_NOT_FOUND) {
				printf("host not found : %s\n", strServerAddress.c_str());
			}
			return 1;
		}

		addrptr = (unsigned int **)host->h_addr_list;

		while (*addrptr != NULL) {
			server.sin_addr.S_un.S_addr = *(*addrptr);

			// connect()������������loop�𔲂��܂�
			if (connect(*pSock,
					(struct sockaddr *)&server,
					sizeof(server)) == 0) {
				break;
			}

			addrptr++;
			// connect�����s�����玟�̃A�h���X�Ŏ����܂�
		}

		// connect���S�Ď��s�����ꍇ
		if (*addrptr == NULL) {
			printf("connect : %d\n", WSAGetLastError());
			return 1;
		}
	}else{
		if(connect(*pSock,(struct sockaddr *)&server,sizeof(server)) != 0){
			printf("connect : %d\n", WSAGetLastError());
			return 1;
		}
	}

	//	����
	return 0;

}

int main(int argc, char *argv[])
{
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
		printf("WSAStartup failed\n");
		return 1;
	}

	std::string strCookie;	//	�N�b�L�[

	//	���O�C���A�N�b�L�[�擾
	{
		SOCKET sock;

		//	�\�P�b�g�쐬
		if( makeSocket( &sock, strProxyAddress,	nProxyPort ) ){
			printf("�ڑ��ł��܂���ł���\n");
			return 1;	
		}

		//	���N�G�X�g�𓊂���
		std::string strPostData;
		strPostData += "POST http://www.digisheet.info/servlet/d HTTP/1.1\r\n";
		strPostData += "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/msword, */*\r\n";
		strPostData += "Referer: http://www.digisheet.info/staffLogin.html\r\n";
		strPostData += "Accept-Language: ja\r\n";
		strPostData += "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; .NET CLR 2.0.50727; Lunascape 3.0.4)\r\n";
		strPostData += "Content-Type: application/x-www-form-urlencoded\r\n";
		strPostData += "Accept-Encoding: gzip, deflate\r\n";
		strPostData += "Host: www.digisheet.info\r\n";
		strPostData += "Content-Length: 63\r\n";
		strPostData += "Proxy-Connection: Keep-Alive\r\n";
		strPostData += "Pragma: no-cache\r\n";
		//strPostData += "Cookie: JServSessionIdroot=sozb41beo1\r\n";
		strPostData += "\r\n";
		strPostData += "HC=XXXX&UI=XXXXXXX&Pw=XXXXXXX&loginButton=login&SI=&CI=0&Typ=1\r\n";
		
		//	�f�[�^���M�A��M
		std::string strRecvData = getPostData( sock, strPostData );

		printf("******\n");
		printf("%s",strRecvData.c_str());

		//�N�b�L�[���擾
		unsigned int nPos = strRecvData.rfind("Set-Cookie: ");
		strCookie.append( strRecvData,nPos+31,10);
		printf("strBuff=%s\n",strCookie.c_str());

		shutdown(sock,0);
		closesocket(sock);
	}

	//	��񑗐M
	{
		SOCKET sock;

		//	�\�P�b�g�쐬
		if( makeSocket( &sock, strProxyAddress,	nProxyPort ) ){
			printf("�ڑ��ł��܂���ł���\n");
			return 1;	
		}

		//	���N�G�X�g�𓊂���
		std::string strPostData;

		//	�f�[�^���M
		strPostData += "POST http://www.digisheet.info/servlet/d HTTP/1.1\r\n";
		strPostData += "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/msword, */*\r\n";
		strPostData += "Referer: http://www.digisheet.info/servlet/d\r\n";
		strPostData += "Accept-Language: ja\r\n";
		strPostData += "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; .NET CLR 2.0.50727; Lunascape 3.0.4)\r\n";
		strPostData += "Content-Type: application/x-www-form-urlencoded\r\n";
		strPostData += "Accept-Encoding: gzip, deflate\r\n";
		strPostData += "Host: www.digisheet.info\r\n";
		strPostData += "Content-Length: 543\r\n";
		strPostData += "Proxy-Connection: Keep-Alive\r\n";
		strPostData += "Pragma: no-cache\r\n";
		//Cookie: JServSessionIdroot=bpiyfvbra1
		strPostData += "\r\n";
		strPostData += "HC=7008&Cl=null&SI=";
		strPostData += strCookie;				//�@�N�b�L�[
		strPostData += "&UI=XXXXXXX";			//	�Ј��ԍ�
		strPostData += "&CI=101";
		strPostData += "&SubCommandID=2";
		strPostData += "&Typ=1";
		strPostData += "&Cr=0000XXXXXXX001";	//	�Ј��ԍ��{�H
		strPostData += "&Y=2006";				//	�N
		strPostData += "&M=11";					//	��
		strPostData += "&Up=2";
		strPostData += "&PrevCommandID=101";
		strPostData += "&D=6";					//	��
		strPostData += "&Years=2006";
		strPostData += "&PrevSubCommandID=";
		strPostData += "&YearStart=2006";
		strPostData += "&MonthStart=10";
		strPostData += "&DayStart=1";
		strPostData += "&YearEnd=2006";
		strPostData += "&MonthEnd=10";
		strPostData += "&DayEnd=30";
		strPostData += "&DayEndSelect=2760";
		strPostData += "&MoneyInputFlag=0";
		strPostData += "&ProcInputFlag=0";
		strPostData += "&CalculateFlag=80";
		strPostData += "&AttendSelect=A0";
		strPostData += "&SaveAttendSelect=A0";
		strPostData += "&HourStart=9";		//�n�Ǝ���
		strPostData += "&MinuteStart=0";	//�n�Ǝ���
		strPostData += "&HourRest=1";		//�x�e����
		strPostData += "&MinuteRest=0";		//�x�e����
		strPostData += "&HourEnd=21";		//�I������
		strPostData += "&MinuteEnd=0";		//�I������
		strPostData += "&OverHourRest=0";
		strPostData += "&OverMinuteRest=0";
		strPostData += "&NightHourRest=0";
		strPostData += "&NightMinuteRest=0";
		strPostData += "&ContentSelect=0000000000";
		strPostData += "&ProcTimeHour=0";
		strPostData += "&ProcTimeMinute=0";
		strPostData += "&CommentInput=\r\n";

		//	�f�[�^���M�A��M
		std::string strRecvData = getPostData( sock, strPostData );

		printf("******\n");
		printf("%s",strRecvData.c_str());

		shutdown(sock,0);
		closesocket(sock);
	}
	
	WSACleanup();
	
	return 0;
}
