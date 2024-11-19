#pragma once
#include <map>
#include <string>
using namespace std;

class kiihdyttaja
{
public:
	struct Speksit {
		string nimi;
		double GearRatio;
		double StepAngle;
		double jaksonkulma;
		double jaksonaika;


	};
	void printtaaSpeksit() const;

	bool lataaSpeksit(const string& tiedosto);

	bool lataaSpeksitNimella(const string& nimi);

	void tallennaSpeksit(const string& tiedosto) const;

	void kyselySpeksit();

	const Speksit& haeSpeksit() const {
		return loadedSpeksit;
	}

private:

	std::map<string, Speksit> speksiMap;
	Speksit loadedSpeksit;

	void teecustomSpeksit(const string& nimi);

};


