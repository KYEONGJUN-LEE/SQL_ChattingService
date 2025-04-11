//#include <iostream>
//#include <string>
//#include <winsock2.h>
//#include <windows.h>
//#include <ws2tcpip.h>
//#include <thread>
//
//#pragma comment(lib, "ws2_32.lib")
//
//bool isLoggedIn = false;
//bool isRunning = true;
//
//// 서버에 메시지를 보내고 응답을 받는 함수
//std::string sendAndReceive(SOCKET sock, const std::string& msg) {
//    send(sock, msg.c_str(), msg.length(), 0);
//
//    char buffer[1024] = { 0 };
//    int recvLen = recv(sock, buffer, sizeof(buffer) - 1, 0);
//    if (recvLen > 0) {
//        buffer[recvLen] = '\0';
//        return std::string(buffer);
//    }
//    return "서버 응답 없음.";
//}
//
//// 실시간 수신 스레드
//void receiveThread(SOCKET sock) {
//    char buffer[1024];
//    while (isRunning) {
//        int recvLen = recv(sock, buffer, sizeof(buffer) - 1, 0);
//        if (recvLen > 0) {
//            buffer[recvLen] = '\0';
//            std::cout << "\n[받은 메시지] " << buffer << std::endl;
//            std::cout << "> ";
//            std::cout.flush();
//        }
//    }
//}
//
//void showInitialMenu() {
//    std::cout << "\n=== 채팅 클라이언트 ===\n";
//    std::cout << "1. 회원가입\n";
//    std::cout << "2. 로그인\n";
//    std::cout << "3. 종료\n";
//    std::cout << "메뉴 선택: ";
//}
//
//void showChatMenu() {
//    std::cout << "\n=== 채팅 메뉴 ===\n";
//    std::cout << "1. 사용자에게 메시지 보내기\n";
//    std::cout << "2. 로그아웃\n";
//    std::cout << "3. 종료\n";
//    std::cout << "메뉴 선택: ";
//}
//
//int main() {
//    SetConsoleOutputCP(CP_UTF8); // 콘솔 한글 출력 설정
//
//    WSADATA wsaData;
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//        std::cerr << "WSAStartup 실패\n";
//        return 1;
//    }
//
//    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
//    sockaddr_in serverAddr{};
//    serverAddr.sin_family = AF_INET;
//    serverAddr.sin_port = htons(12345);
//    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
//
//    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
//        std::cerr << "서버 연결 실패\n";
//        closesocket(sock);
//        WSACleanup();
//        return 1;
//    }
//
//    std::thread recvThread;
//
//    while (isRunning) {
//        if (!isLoggedIn) {
//            showInitialMenu();
//            int choice;
//            std::cin >> choice;
//
//            std::string id, pw;
//            switch (choice) {
//            case 1:
//                std::cout << "아이디: ";
//                std::cin >> id;
//                std::cout << "비밀번호: ";
//                std::cin >> pw;
//                std::cout << sendAndReceive(sock, "REGISTER:" + id + ":" + pw + ":") << std::endl;
//                break;
//            case 2: {
//                std::cout << "아이디: ";
//                std::cin >> id;
//                std::cout << "비밀번호: ";
//                std::cin >> pw;
//                std::string response = sendAndReceive(sock, "LOGIN:" + id + ":" + pw + ":");
//                std::cout << response << std::endl;
//                if (response.find("성공") != std::string::npos) {
//                    isLoggedIn = true;
//                    recvThread = std::thread(receiveThread, sock);
//                }
//                break;
//            }
//            case 3:
//                isRunning = false;
//                break;
//            default:
//                std::cout << "잘못된 입력입니다.\n";
//            }
//        }
//        else {
//            showChatMenu();
//            int choice;
//            std::cin >> choice;
//            std::cin.ignore();
//
//            switch (choice) {
//            case 1: {
//                std::cout << "상대방 ID 입력: ";
//                int targetId;
//                std::cin >> targetId;
//                std::cin.ignore();
//
//                std::cout << "메시지 (exit 입력 시 종료):\n";
//                while (true) {
//                    std::cout << "> ";
//                    std::string chat;
//                    std::getline(std::cin, chat);
//
//                    if (chat == "exit") {
//                        break;
//                    }
//
//                    std::string formattedMsg = "TO:" + std::to_string(targetId) + ":" + chat;
//                    std::cout << sendAndReceive(sock, formattedMsg) << std::endl;
//                }
//                break;
//            }
//            case 2:
//                std::cout << sendAndReceive(sock, "exit") << std::endl;
//                isLoggedIn = false;
//                if (recvThread.joinable()) recvThread.detach(); // 수신 스레드 종료
//                break;
//            case 3:
//                isRunning = false;
//                break;
//            default:
//                std::cout << "잘못된 입력입니다.\n";
//            }
//        }
//    }
//
//    if (recvThread.joinable()) recvThread.detach();
//    closesocket(sock);
//    WSACleanup();
//    std::cout << "프로그램을 종료합니다.\n";
//    return 0;
//}
