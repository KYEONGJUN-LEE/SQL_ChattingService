//#include <iostream>
//#include <string>
//#include <vector>
//#include <map>
//#include <mutex>
//#include <thread>
//#include <winsock2.h>
//#include <mysql/jdbc.h>
//#include <memory>
//#include <windows.h>  // 한글 출력 설정용
//
//#pragma comment(lib, "ws2_32.lib")
//
//const std::string DB_HOST = "tcp://127.0.0.1:3306";
//const std::string DB_USER = "root";
//const std::string DB_PASS = "1234";
//const std::string DB_NAME = "chat_system";
//
//std::map<int, SOCKET> userSocketMap;
//std::mutex mapMutex;
//
//void handleClient(SOCKET clientSocket) {
//    std::string currentUser = "";
//    int currentUserId = -1;
//
//    try {
//        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
//        std::unique_ptr<sql::Connection> conn(driver->connect(DB_HOST, DB_USER, DB_PASS));
//        conn->setSchema(DB_NAME);
//
//        char buffer[1024];
//        while (true) {
//            int len = recv(clientSocket, buffer, sizeof(buffer), 0);
//            if (len <= 0) break;
//
//            std::string msg(buffer, len);
//            std::cout << "클라이언트로부터 수신: " << msg << std::endl;
//
//            if (msg.rfind("REGISTER:", 0) == 0) {
//                size_t pos1 = msg.find(":", 9);
//                size_t pos2 = msg.find(":", pos1 + 1);
//                std::string username = msg.substr(9, pos1 - 9);
//                std::string password = msg.substr(pos1 + 1, pos2 - pos1 - 1);
//
//                std::unique_ptr<sql::PreparedStatement> checkStmt(
//                    conn->prepareStatement("SELECT * FROM users WHERE username = ?")
//                );
//                checkStmt->setString(1, username);
//                std::unique_ptr<sql::ResultSet> res(checkStmt->executeQuery());
//
//                if (res->next()) {
//                    std::string fail = "이미 존재하는 아이디입니다.";
//                    send(clientSocket, fail.c_str(), fail.length(), 0);
//                }
//                else {
//                    std::unique_ptr<sql::PreparedStatement> regStmt(
//                        conn->prepareStatement("INSERT INTO users(username, password) VALUES (?, ?)")
//                    );
//                    regStmt->setString(1, username);
//                    regStmt->setString(2, password);
//                    regStmt->execute();
//
//                    std::string success = "회원가입 성공!";
//                    send(clientSocket, success.c_str(), success.length(), 0);
//                }
//            }
//            else if (msg.rfind("LOGIN:", 0) == 0) {
//                size_t pos1 = msg.find(":", 6);
//                size_t pos2 = msg.find(":", pos1 + 1);
//                std::string username = msg.substr(6, pos1 - 6);
//                std::string password = msg.substr(pos1 + 1, pos2 - pos1 - 1);
//
//                std::unique_ptr<sql::PreparedStatement> loginStmt(
//                    conn->prepareStatement("SELECT user_id FROM users WHERE username = ? AND password = ?")
//                );
//                loginStmt->setString(1, username);
//                loginStmt->setString(2, password);
//                std::unique_ptr<sql::ResultSet> res(loginStmt->executeQuery());
//
//                if (res->next()) {
//                    currentUser = username;
//                    currentUserId = res->getInt("user_id");
//
//                    std::unique_ptr<sql::PreparedStatement> sessionStmt(
//                        conn->prepareStatement("INSERT INTO user_sessions(user_id, ip_address) VALUES (?, '127.0.0.1')")
//                    );
//                    sessionStmt->setInt(1, currentUserId);
//                    sessionStmt->execute();
//
//                    {
//                        std::lock_guard<std::mutex> lock(mapMutex);
//                        userSocketMap[currentUserId] = clientSocket;
//                    }
//
//                    std::string success = "로그인 성공!";
//                    send(clientSocket, success.c_str(), success.length(), 0);
//                }
//                else {
//                    std::string fail = "로그인 실패: 아이디 또는 비밀번호가 틀렸습니다.";
//                    send(clientSocket, fail.c_str(), fail.length(), 0);
//                }
//            }
//            else if (msg.rfind("CHAT:", 0) == 0 && currentUserId != -1) {
//                std::string chatMsg = msg.substr(5);
//
//                std::unique_ptr<sql::PreparedStatement> msgStmt(
//                    conn->prepareStatement("INSERT INTO message_log(sender_id, content) VALUES (?, ?)")
//                );
//                msgStmt->setInt(1, currentUserId);
//                msgStmt->setString(2, chatMsg);
//                msgStmt->execute();
//
//                std::string echo = "서버 응답: " + chatMsg;
//                send(clientSocket, echo.c_str(), echo.length(), 0);
//            }
//            else if (msg.rfind("TO:", 0) == 0 && currentUserId != -1) {
//                size_t pos1 = msg.find(":", 3);
//                if (pos1 == std::string::npos) {
//                    std::string err = "메시지 형식 오류 (예: TO:상대ID:내용)";
//                    send(clientSocket, err.c_str(), err.length(), 0);
//                    continue;
//                }
//
//                int targetId = std::stoi(msg.substr(3, pos1 - 3));
//                std::string chatMsg = msg.substr(pos1 + 1);
//
//                SOCKET targetSocket = INVALID_SOCKET;
//                {
//                    std::lock_guard<std::mutex> lock(mapMutex);
//                    if (userSocketMap.find(targetId) != userSocketMap.end()) {
//                        targetSocket = userSocketMap[targetId];
//                    }
//                }
//
//                if (targetSocket != INVALID_SOCKET) {
//                    std::string toSend = "[From " + currentUser + "] " + chatMsg;
//                    send(targetSocket, toSend.c_str(), toSend.length(), 0);
//                    std::string confirm = "[To " + std::to_string(targetId) + "] 전송 완료.";
//                    send(clientSocket, confirm.c_str(), confirm.length(), 0);
//                }
//                else {
//                    std::string fail = "해당 ID는 접속 중이 아닙니다.";
//                    send(clientSocket, fail.c_str(), fail.length(), 0);
//                }
//            }
//            else if (msg == "exit" && currentUserId != -1) {
//                std::unique_ptr<sql::PreparedStatement> updateStmt(
//                    conn->prepareStatement(
//                        "UPDATE user_sessions SET logout_time = NOW() WHERE user_id = ? AND logout_time IS NULL"
//                    )
//                );
//                updateStmt->setInt(1, currentUserId);
//                updateStmt->execute();
//
//                {
//                    std::lock_guard<std::mutex> lock(mapMutex);
//                    userSocketMap.erase(currentUserId);
//                }
//
//                std::string bye = "로그아웃 완료! 안녕히 가세요";
//                send(clientSocket, bye.c_str(), bye.length(), 0);
//                break;
//            }
//            else {
//                std::string err = "알 수 없는 명령입니다.";
//                send(clientSocket, err.c_str(), err.length(), 0);
//            }
//        }
//    }
//    catch (sql::SQLException& e) {
//        std::cerr << "DB 오류: " << e.what() << std::endl;
//        std::string err = "데이터베이스 오류가 발생했습니다.";
//        send(clientSocket, err.c_str(), err.length(), 0);
//    }
//
//    {
//        std::lock_guard<std::mutex> lock(mapMutex);
//        if (currentUserId != -1) {
//            userSocketMap.erase(currentUserId);
//        }
//    }
//
//    closesocket(clientSocket);
//}
//
//int main() {
//    SetConsoleOutputCP(CP_UTF8); // 한글 출력 설정
//
//    WSADATA wsaData;
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//        std::cerr << "WSAStartup 실패" << std::endl;
//        return 1;
//    }
//
//    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//    if (serverSocket == INVALID_SOCKET) {
//        std::cerr << "소켓 생성 실패" << std::endl;
//        WSACleanup();
//        return 1;
//    }
//
//    sockaddr_in serverAddr{};
//    serverAddr.sin_family = AF_INET;
//    serverAddr.sin_port = htons(12345);
//    serverAddr.sin_addr.s_addr = INADDR_ANY;
//
//    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
//    listen(serverSocket, SOMAXCONN);
//    std::cout << "서버가 포트 12345에서 실행 중입니다.\n";
//
//    while (true) {
//        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
//        if (clientSocket != INVALID_SOCKET) {
//            std::cout << "클라이언트가 접속했습니다.\n";
//            std::thread clientThread(handleClient, clientSocket);
//            clientThread.detach();  // 멀티스레드 처리
//        }
//    }
//
//    closesocket(serverSocket);
//    WSACleanup();
//    return 0;
//}
