#include <engine.h>

SVARP(userlang, "en");

oldstring theboi;

char* langtest(char* index, char* lang) {
	formatstring(theboi, "%s (%s)", index, lang);
	return theboi;
}

char* langtest(char* index) {
	return langtest(index, userlang);
}