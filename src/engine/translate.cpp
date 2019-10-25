
#include "engine.h"

SVARP(language, "en_US");
VAR(translationCapture, 0, 0, 1);

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

        return noTranslation;
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

ICOMMAND(reloadlanguage, "", (), {
    defformatstring(translationFile, "packages/lang/%s.cfg", language);
    execfile(translationFile);
});