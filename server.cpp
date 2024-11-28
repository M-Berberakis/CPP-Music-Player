#include <iostream>
#include <fstream>
#include <vector>
#include <WS2tcpip.h>
#include <thread>
#include <filesystem>
#include <queue>

// Link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

const int _PORT = 5050;
const int _BUFFER = 128;

// Function to read audio data from a file
std::vector<char> readAudioFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Could not open audio file: " << filename << std::endl;
        return std::vector<char>();
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    return buffer;
}

// Function to get a list of .wav files in a directory
std::queue<std::string> getAudioFile(const std::filesystem::path& directory) {
    std::queue<std::string> wavAudioFile;
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file() and entry.path().extension() == ".wav") {
            wavAudioFile.push(entry.path().string());
        }
    }

    return wavAudioFile;
}

// Function to send audio data to the client socket
void sendAudioData(SOCKET clientSocket, const std::vector<char>& wavData) {
    send(clientSocket, wavData.data(), wavData.size(), 0);
}

// Function to handle client connection and send audio files
void clientConnect(SOCKET clientSocket, std::queue<std::string>& wavFiles) {
    while (!wavFiles.empty()) {
        std::string currentFile = wavFiles.front();
        wavFiles.pop();

        std::vector<char> wavData = readAudioFile(currentFile);
        if (!wavData.empty()) {
            std::cout << "Playing: " << currentFile << std::endl;
            sendAudioData(clientSocket, wavData);

            // Wait for the sound to finish playing
            Sleep(static_cast<DWORD>(wavData.size()) / (44100 * 2 * 16 / 8));

            std::cout << "Song finished: " << currentFile << std::endl;
        }
    }

    std::cout << "The client has disconnected. The client socket will now close." << std::endl;
    closesocket(clientSocket);
}

int main(int argc, char* argv[]) {
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    if (WSAStartup(ver, &wsData) != 0) {
        std::cerr << "Could not initialize socket" << std::endl;
        return -1;
    }

    // Get the directory of the executable
    std::filesystem::path filePath = std::filesystem::absolute(std::filesystem::path(argv[0]).parent_path());
    std::queue<std::string> wavFiles = getAudioFile(filePath);

    if (wavFiles.empty()) {
        std::cerr << "There were no .wav files found" << std::endl;
        WSACleanup();
        return -1;
    }

    SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket == INVALID_SOCKET) {
        std::cerr << "ERROR. Socket was not created!" << std::endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(_PORT);
    hint.sin_addr.s_addr = INADDR_ANY;

    bind(listeningSocket, (sockaddr*)&hint, sizeof(hint));
    listen(listeningSocket, SOMAXCONN);

    std::cout << "Listening on port... " << _PORT << "..." << std::endl;

    while (true) {
        SOCKET clientSocket = accept(listeningSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Client not accepted" << std::endl;
            break;
        }

        std::thread clientThread(clientConnect, clientSocket, std::ref(wavFiles));
        clientThread.detach();
    }

    std::cout << "The server is closing" << std::endl;
    closesocket(listeningSocket);
    WSACleanup();

    return 0;
}
