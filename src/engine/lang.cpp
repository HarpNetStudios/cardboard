#include <engine.h>

SVARP(userlang, "en");

char* langtest(char* index, char* lang) {
	oldstring theboi;
	formatstring(theboi, "%s (%s)", index, lang);
	return theboi;
}

char* langtest(char* index) {
	return langtest(index, userlang);
}