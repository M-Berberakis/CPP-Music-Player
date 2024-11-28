#include <iostream>
#include <vector>
#include <winsock2.h>
#include <dsound.h>
#include <Ws2tcpip.h>

// Link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

// Link with dsound.lib
#pragma comment(lib, "dsound.lib")

const int _PORT = 5050;
const int _BUFFER = 128;

// Function to receive audio data from the server
std::vector<char> receiveAudioData(SOCKET serverSocket) {
    std::vector<char> buffer(_BUFFER);
    std::vector<char> audioData;

    int byteData;
    do {
        byteData = recv(serverSocket, buffer.data(), buffer.size(), 0);
        if (byteData > 0) {
            audioData.insert(audioData.end(), buffer.begin(), buffer.begin() + byteData);
        }
    } while (byteData > 0);

    return audioData;
}

// Function to play the received audio file using DirectSound
void playAudioFile(const std::vector<char>& wavData) {
    LPDIRECTSOUND8 playDS;

    if (FAILED(DirectSoundCreate8(nullptr, &playDS, nullptr))) {
        std::cerr << "ERROR. DirectSound was not created" << std::endl;
        return;
    }

    if (FAILED(playDS->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY))) {
        std::cerr << "Cooperative level not set!" << std::endl;
        playDS->Release();
        return;
    }

    WAVEFORMATEX audioFormat;
    ZeroMemory(&audioFormat, sizeof(WAVEFORMATEX));
    audioFormat.wFormatTag = WAVE_FORMAT_PCM;
    audioFormat.nChannels = 2;
    audioFormat.nSamplesPerSec = 44100;
    audioFormat.wBitsPerSample = 16;
    audioFormat.nBlockAlign = (audioFormat.nChannels * audioFormat.wBitsPerSample) / 8;
    audioFormat.nAvgBytesPerSec = audioFormat.nSamplesPerSec * audioFormat.nBlockAlign;

    DSBUFFERDESC dsBuffer;
    ZeroMemory(&dsBuffer, sizeof(DSBUFFERDESC));
    dsBuffer.dwSize = sizeof(DSBUFFERDESC);
    dsBuffer.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME;
    dsBuffer.dwBufferBytes = static_cast<DWORD>(wavData.size());
    dsBuffer.lpwfxFormat = &audioFormat;

    LPDIRECTSOUNDBUFFER playDsBuffer;
    if (FAILED(playDS->CreateSoundBuffer(&dsBuffer, &playDsBuffer, nullptr))) {
        std::cerr << "Sound buffer was not created!" << std::endl;
        playDS->Release();
        return;
    }

    void* playAudio;
    DWORD sizeOfData;
    if (FAILED(playDsBuffer->Lock(0, wavData.size(), &playAudio, &sizeOfData, nullptr, nullptr, 0))) {
        std::cerr << "Could not lock sound buffer!" << std::endl;
        playDsBuffer->Release();
        playDS->Release();
        return;
    }

    memcpy(playAudio, wavData.data(), sizeOfData);

    if (FAILED(playDsBuffer->Unlock(playAudio, sizeOfData, nullptr, 0))) {
        std::cerr << "Could not unlock sound buffer!" << std::endl;
        playDsBuffer->Release();
        playDS->Release();
        return;
    }

    if (FAILED(playDsBuffer->Play(0, 0, 0))) {
        std::cerr << "Sound buffer could not be played" << std::endl;
        playDsBuffer->Release();
        playDS->Release();
        return;
    }

    // Wait for the sound to finish playing
    Sleep(static_cast<DWORD>(wavData.size()) / audioFormat.nAvgBytesPerSec * 1000);

    playDsBuffer->Release();
    playDS->Release();
}

int main() {
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    if (WSAStartup(ver, &wsData) != 0) {
        std::cerr << "Socket could not be initialized" << std::endl;
        return -1;
    }

    SOCKET serverSOCK = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSOCK == INVALID_SOCKET) {
        std::cerr << "Socket was not created" << std::endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in serverADDR;
    serverADDR.sin_family = AF_INET;
    serverADDR.sin_port = htons(_PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverADDR.sin_addr);

    if (connect(serverSOCK, (sockaddr*)&serverADDR, sizeof(serverADDR)) == SOCKET_ERROR) {
        std::cerr << "Connection to server failure" << std::endl;
        closesocket(serverSOCK);
        WSACleanup();
        return -1;
    }

    std::vector<char> wavData = receiveAudioData(serverSOCK);

    if (!wavData.empty()) {
        std::cout << "Audio file was received. Now playing " << std::endl;
        playAudioFile(wavData);
    }
    else {
        std::cerr << "WAV file could not be retrieved" << std::endl;
    }

    closesocket(serverSOCK);
    WSACleanup();

    return 0;
}
