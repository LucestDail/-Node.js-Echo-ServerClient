#include<stdio.h>
#include<WinSock2.h>
//TCP/IP 통신간 사용되는 대다수의 함수들은 <WinSock2.h> 헤더에 다 선언되어 있다. 요것만 있으면 만능!
#pragma comment(lib, "wsock32.lib")
//<WinSock2.h> 함수들은 일반적인 C언어 런타임 라이브러리에 포함되어 있지 않기 때문에, 해당 라이브러리 포함이 필요하다.
// <WinSock2.h> 헤더 포함시 같이 선언하여 주자!
/* 크게 돌아가는 구조는
	WSADATA 통한 소켓 데이터 소환
	WSAStartup 통한 소켓 점검
	Socket s; 소켓 개방
	closesocket(s) 소켓 폐쇄
	WSACleanup 소켓 자원 정리
	순으로 이어진다.
*/

void main(int argc, char *argv[])
{
	//소켓을 사용하기 위한 디스크럽터 정의, 소켓 번호라고 할 수 있으며, File* fd(file descriptor) 랑 비슷
	SOCKET s;
	//WSA 버전 확인하는 함수, 불러온 wsaData 는 struct 이다.
	WSADATA wsaData;
	//소켓 구조체 sin 명의로 선언
	struct sockaddr_in sin;
	char data[1000];
	char ID[100], PWD[100];

	//윈속 사용가능한지 운영체제 데이터와 비교하는 함수, WSAStartup(a,b) 함수로 a == b 이면 0을 반환한다
	if (WSAStartup(WINSOCK_VERSION, &wsaData) != 0)
	{
		printf("WSAStartup 실패, 에러코드 = %d\n", WSAGetLastError());
		return;
	}
	//TCP/IP 소켓 생성, 여러번 사용 가능bind 함수
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// 소켓 생성이 안되었을 경우 에러코드 출력, 대부분 WSAStartup 함수 안쓰거나, 잘못된 인수 사용할 경우 발생!
	if (s == INVALID_SOCKET)
	{
		printf("소켓 생성 실패, 에러코드 : %d\n", WSAGetLastError());
		WSACleanup();
		return;
	}
	puts("서버 연결을 시도합니다...");
	//접속 서버 정보 입력(INET 는 IPv4, 로컬호스트 접속 및 포트 10000 고정
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	sin.sin_port = htons(10000);
	if (connect(s, (struct sockaddr*)&sin, sizeof(sin)) != 0)
	{
		printf(" 접속 실패, 에러 코드 : %u\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return;
	}

	puts("서버 연결 완료");
	puts("인증 서버 시작");
	printf("[ID 입력] : ");
	gets(ID);
	printf("[PWD 입력] : ");
	gets(PWD);
	sprintf(data, "%-8.8s%-8.8s", ID, PWD);
	//전송 함수
	if (send(s, data, strlen(data), 0) < strlen(data))
	{
		printf("전송 실패, 에러 코드 = %u\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return;
	}
	//수신 함수
	if (recv(s, data, 1, 0) < 1)
	{
		printf("수신 실패, 에러 코드 = %u\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return;
	}

	if (data[0] == '1')
	{
		puts("로그인 성공");
	}
	else if (data[0] == '2')
	{
		puts("로그인 실패");
	}
	else
	{
		puts("ID 없음");
	}
	//소켓 제거
	if (closesocket(s) != 0)
	{
		printf("소켓 제거 실패, 에러코드 = %u\n", WSAGetLastError());
		WSACleanup();
		return;
	}
	//소켓 정리
	if (WSACleanup() != 0)
	{
		printf("WSACleanup 실패, 에러코드 = %u\n", WSAGetLastError());
		return;
	}
}