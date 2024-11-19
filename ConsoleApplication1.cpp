#include <windows.h>
#include <iostream>
#include <string>
using namespace std;

#include <windows.h>
#include <iostream>
#include <string>
#include "kiihdyttaja.h"
using namespace std;

wstring stringToWString(const string& str) {
    return wstring(str.begin(), str.end());
}
// Function to open the serial port (COMx), configure it and return the handle
HANDLE openSerialPort(const string& portName) {
    wstring widePortName = stringToWString(portName);
    HANDLE hSerial = CreateFile(
        widePortName.c_str(),  // COM port name, e.g., "COM3"
        GENERIC_READ | GENERIC_WRITE,  // Open for both read and write
        0,  // No sharing
        NULL,  // Default security attributes
        OPEN_EXISTING,  // Open the existing port
        0,  // No special attributes
        NULL);  // No template file

    if (hSerial == INVALID_HANDLE_VALUE) {
        cerr << "Error opening COM port" << endl;
        return INVALID_HANDLE_VALUE;
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        cerr << "Error getting current serial settings" << endl;
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    dcbSerialParams.BaudRate = CBR_115200;  // 115200 baud rate
    dcbSerialParams.ByteSize = 8;  // 8 data bits
    dcbSerialParams.StopBits = ONESTOPBIT;  // 1 stop bit
    dcbSerialParams.Parity = NOPARITY;  // No parity

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        cerr << "Error setting serial port parameters" << endl;
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    // Set timeouts
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        cerr << "Error setting timeouts" << endl;
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    return hSerial;
}


void sendCommandToArduino(HANDLE hSerial, const string& command) {
    DWORD bytesWritten;
    if (!WriteFile(hSerial, command.c_str(), command.length(), &bytesWritten, NULL)) {
        cerr << "Error writing to serial port" << endl;
        return;
    }

    Sleep(100);
    char buffer[256];
    DWORD bytesRead;
    memset(buffer, 0, sizeof(buffer));

    COMMTIMEOUTS timeouts;
    GetCommTimeouts(hSerial, &timeouts);
    timeouts.ReadTotalTimeoutConstant = 5000;
    SetCommTimeouts(hSerial, &timeouts);


    if (!ReadFile(hSerial, buffer, sizeof(buffer), &bytesRead, NULL)) {
        cerr << "Error reading from serial port" << endl;
        return;
    }


    buffer[bytesRead] = '\0'; 
    cout << "Arduino Response: " << buffer << endl;
}
bool testCommunication(HANDLE hSerial) {
    cout << "Sending test command to Arduino..." << endl;
    string testCommand = "test";
    sendCommandToArduino(hSerial, testCommand);


    cout << "Communication test complete." << endl;
    return true;
}
int main() {
    kiihdyttaja kiihdyta;
    string portName = "COM6";  // Change to the correct COM port for your system
    HANDLE hSerial = openSerialPort(portName);
    kiihdyta.lataaSpeksit("speksit.txt");

    bool edetaan=0;

    string commandv;

    while (true) {
        
        sendCommandToArduino(hSerial, "test");
        Sleep(100);
        if (edetaan)
        {
            cout << "päästiin tänne" << endl;
            return 0;
        }



        cout << "Anna komento (speksit, load <nimi>, customspeksi, lopeta): ";
        cin >> commandv;

        if (commandv == "speksit") {
            kiihdyta.printtaaSpeksit();
        }
        else if (commandv == "load") {
            string nimi;
            cin >> nimi;
            if (kiihdyta.lataaSpeksitNimella(nimi)) {
                const auto& loaded = kiihdyta.haeSpeksit();
                cout << "Loaded Speksit: " << loaded.nimi << endl;
                cout << "Gear Ratio: " << loaded.GearRatio << endl;
                cout << "Step Angle: " << loaded.StepAngle << endl;
                cout << "Angle per Step: " << loaded.jaksonkulma << endl;
                cout << "Step Time: " << loaded.jaksonaika << " ms" << endl;

                cout << "Edetaanko nailla parametreilla? (0/1): ";
                cin >> edetaan;
                while (edetaan) {
                    sendCommandToArduino(hSerial, "test");
                    double speed, accell;
                	int matka;
                    speed = ((loaded.jaksonkulma / loaded.StepAngle) / (loaded.jaksonaika / 1000))*1.5;
                    accell = (speed / 1)*1.5;
                    cout << "Anna haluttu matka askeleina(resolution nyt1/8)" << endl;
                    cin >> matka;
                    int intSpeed = static_cast<int>(round(speed));
                    int intAccell = static_cast<int>(round(accell));

                    string komento = "run " + to_string(intSpeed) + " " + to_string(intAccell) + " " + to_string(matka);
                    sendCommandToArduino(hSerial, komento);

                    cout<<"Jatketaanko? (0/1):" << endl;
                    cin >>edetaan;



                }
                
                }
            else {
                cout << "Loadaus epäonnistui" << endl;
            }
        }else if (commandv == "customspeksi") {
            kiihdyta.kyselySpeksit();
        }
        else if (commandv == "lopeta") {
            cout << "lopetetaan" << endl;
            return 0;
        }
        else {
            cout << "Tuntematon komento!" << endl;
        }

        }
    CloseHandle(hSerial);
    return 0;
}

