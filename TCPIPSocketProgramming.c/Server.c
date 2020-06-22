
#include<stdio.h>
#include<WinSock2.h>

// 사용자 ID 및 PW 저장 위치 선정
#define USERFILE "C:\\Users\\Admin\\userid.txt"
#define _CRT_SECURE_NO_WARNINGS
//ID 및 PWD 호출 함수 getid 선언
void getid(void);

//TCP/IP 통신간 사용되는 대다수의 함수들은 <WinSock2.h> 헤더에 다 선언되어 있다. 요것만 있으면 만능!
#pragma comment(lib, "wsock32.lib")

// 변수명 IDLIST 로 구조체 tagIDList 선언
typedef struct tagIDList
{
	char uid[9];
	char pwd[9];
}IDLIST;
//IDLIST 타입 구조체 100개 배열을 id 명으로 선언
IDLIST id[100];
//<WinSock2.h> 함수들은 일반적인 C언어 런타임 라이브러리에 포함되어 있지 않기 때문에, 해당 라이브러리 포함이 필요하다.
// <WinSock2.h> 헤더 포함시 같이 선언하여 주자!
/* 서버 소켓 프로그래밍 순환 구조는
	WSADATA 통한 소켓 데이터 소환 및 구조체 선언(sin, cli_addr)
	WSAStartup 통한 소켓 점검
	Socket s; 소켓 개방
	bind 접속 포트 및 ip 지정
	accept 접속 승인
	closesocket(s) 소켓 폐쇄
	WSACleanup 소켓 자원 정리
	순으로 이어진다.
*/
void main(void)
{
	//소켓을 사용하기 위한 디스크럽터 정의, 소켓 번호라고 할 수 있으며, File* fd(file descriptor) 랑 비슷
	SOCKET s, cs;
	//WSA 버전 확인하는 함수, 불러온 wsaData 는 struct 이다.
	WSADATA wsaData;

	//소켓 구조체 sin, cli_addr 선언
	struct sockaddr_in sin;
	struct sockaddr_in cli_addr;
	int size = sizeof(cli_addr);
	char data[1000];
	int ret,i;

	//구조체 login 명으로 선언
	IDLIST login;
	//해당 구조체 내용 getid 함수 시행
	getid();

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


	//주소 체계 설정 : IPv4 (내부 프로토콜단의 경우 AF_UNIX, IPv6 의 경우 AF_INET6)
	sin.sin_family = AF_INET;
	//접석 허용 : 모든 클라이언트 대상
	sin.sin_addr.s_addr = htonl(ADDR_ANY);
	//포트 번호 설정 : 10000(1 ~ 65535 까지 사용 가능, 단 1 ~ 2000번 사이는 운영체제 상 사용 할당됨. 21은 FTP, 80은 웹 서버 포트)
	sin.sin_port = htons(10000);

	//IP 및 포트 바인딩 간 실패 시 에러 설정
	if (bind(s, (struct sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf(" 바인드 실패, 에러 코드 : %u\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return;
	}

	// 리슨 모드 설정 실패 시 에러 설정
	if (listen(s, SOMAXCONN) != 0)
	{
		printf(" 리슨 모드 설정 실패, 에러 코드 : %u\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return;
	}
	puts("인증 서버 시작");





	while (1)
	{
		cs = accept(s, (struct sockaddr*)&cli_addr, &size);
		if (cs == INVALID_SOCKET)
		{
			printf(" 접속 승인 실패, 에러 코드 : %u\n", WSAGetLastError());
			closesocket(s);
			WSACleanup();
			return;
		}

		// data 변수 0으로 초기화
		memset(data, 0, sizeof data);

		ret = recv(cs, data, 1000, 0);
		if (ret == SOCKET_ERROR)
		{
			printf("수신 실패, 에러 코드 : %u\n", WSAGetLastError());
			closesocket(cs);
			closesocket(s);
			WSACleanup();
			return;
		}
		//login 변수값 0으로 초기화
		memset(&login, 0, sizeof(login));
		//data 값으로 login 복사
		strncpy(login.uid, &data[0], 8);
		strncpy(login.pwd, &data[8], 8);

		//공백 제거
		for (i = 7; i >= 0; i--)
		{
			if (isspace(login.uid[i]))
				login.uid[i] = 0;
			if (isspace(login.pwd[i]))
				login.pwd[i] = 0;
		}
		//파일 전체 확인하여 비교 실시
		for (i = 0; i < 100; i++)
		{
			if (strcmp(login.uid,id[i].uid) == 0)
			{
				if (strcmp(login.pwd, id[i].pwd) == 0)
				{
					printf("%s 사용자 접속\n", login.uid);
					send(cs, "1", 1, 0);
					break;
				}
				else
				{
					printf("%s 사용자 암호가 정확하지 않습니다.\n", login.uid);
					send(cs, "2", 1, 0);
					break;
				}
			}
		}
		// 파일 전체 출력 이후 해당사항 없는 경우 if 문 출력
		if (i == 100)
		{
			printf("등록되지 않은 사용자가 로그인을 시도하였습니다\n");
			printf("사용자 ID : %s\n", login.uid);
			send(cs, "3", 1, 0);
		}
		closesocket(cs);

	}

	//소켓 끊음. 버퍼 남아있어도 즉시 종료, 여러번 사용 가능
	if(closesocket(cs) != 0 || closesocket(s) != 0)
	{
		printf("소켓 제거 실패, 에러코드 = %u\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	// 소켓 사용시 사용한 자원 전체 정리 실시
	if (WSACleanup() != 0)
	{
		printf("WSACleanup 실패, 에러코드 = %u\n", WSAGetLastError());
		return;
	}
}
//struct tagIDlist 기준, fopen 통한 userid 파일 입출력 관련 함수
void getid(void)
{
	FILE *fp;
	int i, j;
	char buff[1000];
	fp = fopen(USERFILE, "r");

	for (i = 0; i < 100; i++)
	{
		fgets(buff, 20, fp);
		strncpy(id[i].uid, &buff[0], 8);
		strncpy(id[i].pwd, &buff[8], 8);

		for (j = 7; j >= 7; j--)
		{
			if (isspace(id[i].uid[j]))
				id[i].uid[j] = 0;
			if (isspace(id[i].pwd[j]))
				id[i].pwd[j] = 0;
		}
	}
	fclose(fp);
}