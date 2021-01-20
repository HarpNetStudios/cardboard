
#include "engine.h"

SVARFP(language, "en_US", {
	defformatstring(translationFile, "packages/lang/%s.cfg", language);
	execfile(translationFile);
});
VAR(translationCapture, 0, 0, 1);

void reloadlanguage(int ok) {
	defformatstring(translationFile, "packages/lang/%s.cfg", language);
	execfile(translationFile);
}

struct Translation
{
    char *key;
    char *value;
   
    Translation(const char *key, const char *value) : key(newstring(key)), value(newstring(value))
    {};
   
    ~Translation()
    {
        DELETEA(key);
        DELETEA(value);
    }
};

hashtable<const char *, Translation *> strings;
const char *noTranslation = "No translation available.";

VARP(dbgtranslation, 0, 0, 1);

const char *getTranslation(const char *name)
{
    Translation **c = strings.access(name);
    if(!c)
    {
        if(translationCapture)
        {
            Translation *trans = new Translation(name, name);
            strings[trans->key] = trans;
            logoutf("\"%s\" \"%s\"\n", name, name);
        }

        return dbgtranslation ? name : noTranslation;
    }
    return (*c)->value;
}

ICOMMAND(gettrans, "s", (const char *name), {
    const char *translation = getTranslation(name);
    if(translation == noTranslation)
    {
        defformatstring(_translation, "No translation available for: %s.", name);
        return result(_translation);
    }
    return result(translation);
});

ICOMMAND(translate, "sss", (const char* stringLanguage, const char* name, const char* string), {
    if(strcmp(language, stringLanguage) != 0)
    {
        return;
    }   
    Translation *trans = new Translation(name, string);
    strings[trans->key] = trans;
});

ICOMMAND(reloadlanguage, "", (), reloadlanguage(0));