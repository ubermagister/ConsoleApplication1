#include "kiihdyttaja.h"
#include <map>
#include <iostream>
#include <fstream>
#include <limits>


void kiihdyttaja::printtaaSpeksit() const {
	
    if (speksiMap.empty()) {
        cout << "tiedosto loytynyt mutta tyhja" << endl;
        return;
    }
    for (const auto& entry : speksiMap) {
        const Speksit& speksi = entry.second;
        cout << "Nimi:" << speksi.nimi << endl;
        cout << "Gear Ratio:" << speksi.GearRatio << endl;
        cout << "Step Angle:" << speksi.StepAngle << endl;
        cout << "Jaksonkulma:" << speksi.jaksonkulma << endl;
        cout << "Jaksonaika:" << speksi.jaksonaika << endl;
        cout << "----------:" << endl;

    }


};

bool kiihdyttaja::lataaSpeksit(const string& tiedosto) {
    ifstream file(tiedosto);
    if (!file.is_open()) {
        cout << "tiedoston avaamisessa ongelma" << endl;
        return false;
    }
    string nimi;
    Speksit speksi;
    while (getline(file, nimi)) {
        file >> speksi.GearRatio >> speksi.StepAngle >> speksi.jaksonkulma >> speksi.jaksonaika;
        file.ignore();
        speksi.nimi = nimi;
        speksiMap[nimi] = speksi;
    }
    file.close();
    return true;
}

bool kiihdyttaja::lataaSpeksitNimella(const string& nimi)
{
    auto it = speksiMap.find(nimi);
    if (it != speksiMap.end()) {
        loadedSpeksit = it->second;
        return true;
    }
    cout << "Speksit nimellä: " << nimi << " Ei Löydettavissa" << endl;
    return false;
}





void kiihdyttaja::tallennaSpeksit(const string& tiedosto) const {

    ofstream file(tiedosto, ios::app);
    if (!file.is_open()) {
        cout << "tiedoston avaamisessa ongelma" << endl;
        return;
    }
    for (const auto& entry : speksiMap) {
        const Speksit& speksi = entry.second;
        file << speksi.nimi << endl;
        file << speksi.GearRatio << " " << speksi.StepAngle << " "
            << speksi.jaksonkulma << " " << speksi.jaksonaika << endl;
    }

    file.close();
}


void kiihdyttaja::kyselySpeksit() {
    string nimi;
    cout << "nimeä uudet speksit";
    cin >> nimi;

    teecustomSpeksit(nimi);
}

void kiihdyttaja::teecustomSpeksit(const string& nimi) {
    Speksit uusispeksi;
    uusispeksi.nimi = nimi;

    while (true) {
        double driver, driven;
        cout << "Anna Gear ratio:n antamalla ensiksi ajuri rattaan hammasluku: ";
        if (cin >> driver) {
            cout << "Anna ajettavan rattaan hammasluku: ";
            if (cin >> driven && driven != 0) {
                uusispeksi.GearRatio = driver / driven;
                break;
            }
            else {
                cout << "Virheellinen syöte, ajettavan rattaan hammasluku ei voi olla nolla tai kirjain.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
        else {
            cout << "Virheellinen syöte, syötä numeroita.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    while (true) {
        cout << "Anna moottorin steppikulma <asteina>: ";
        if (cin >> uusispeksi.StepAngle) {
            break;
        }
        else {
            cout << "Virheellinen syöte, syötä numeroita.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    while (true) {
        cout << "Anna jaksonkulma <asteina>: ";
        if (cin >> uusispeksi.jaksonkulma) {
            break;
        }
        else {
            cout << "Virheellinen syöte, syötä numeroita.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }    while (true) {
        cout << "Anna jaksonaika <millisekunttia>: ";
        if (cin >> uusispeksi.jaksonaika) {
            break;
        }
        else {
            cout << "Virheellinen syöte, syötä numeroita.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }


    speksiMap[nimi] = uusispeksi;
    tallennaSpeksit("speksit.txt");
    cout << "Uudet speksit tallennettu nimellä: " << nimi << endl;

};
