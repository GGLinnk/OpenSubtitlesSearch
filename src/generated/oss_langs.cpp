#include "generated/oss_langs.h"
#include <QObject>

OssLangs * OssLangs::_instance = NULL;

OssLangs * OssLangs::instance() {
    if (!_instance) _instance = new OssLangs();
    return _instance;
}

OssLangs::OssLangs() : QMap<QString, QString>() {
        insert("alb", QObject::trUtf8("Albanian"));
        insert("ara", QObject::trUtf8("Arabic"));
        insert("baq", QObject::trUtf8("Basque"));
        insert("bul", QObject::trUtf8("Bulgarian"));
        insert("cat", QObject::trUtf8("Catalan"));
        insert("chi", QObject::trUtf8("Chinese"));
        insert("cze", QObject::trUtf8("Czech"));
        insert("dan", QObject::trUtf8("Danish"));
        insert("dut", QObject::trUtf8("Dutch"));
        insert("eng", QObject::trUtf8("English"));
        insert("est", QObject::trUtf8("Estonian"));
        insert("fin", QObject::trUtf8("Finnish"));
        insert("fre", QObject::trUtf8("French"));
        insert("geo", QObject::trUtf8("Georgian"));
        insert("ger", QObject::trUtf8("German"));
        insert("glg", QObject::trUtf8("Galician"));
        insert("ell", QObject::trUtf8("Greek"));
        insert("heb", QObject::trUtf8("Hebrew"));
        insert("hrv", QObject::trUtf8("Croatian"));
        insert("hun", QObject::trUtf8("Hungarian"));
        insert("ice", QObject::trUtf8("Icelandic"));
        insert("ind", QObject::trUtf8("Indonesian"));
        insert("ita", QObject::trUtf8("Italian"));
        insert("jpn", QObject::trUtf8("Japanese"));
        insert("kor", QObject::trUtf8("Korean"));
        insert("mac", QObject::trUtf8("Macedonian"));
        insert("may", QObject::trUtf8("Malay"));
        insert("nor", QObject::trUtf8("Norwegian"));
        insert("oci", QObject::trUtf8("Occitan"));
        insert("per", QObject::trUtf8("Persian"));
        insert("pol", QObject::trUtf8("Polish"));
        insert("por", QObject::trUtf8("Portuguese"));
        insert("rus", QObject::trUtf8("Russian"));
        insert("scc", QObject::trUtf8("Serbian"));
        insert("sin", QObject::trUtf8("Sinhalese"));
        insert("slo", QObject::trUtf8("Slovak"));
        insert("slv", QObject::trUtf8("Slovenian"));
        insert("spa", QObject::trUtf8("Spanish"));
        insert("swe", QObject::trUtf8("Swedish"));
        insert("tgl", QObject::trUtf8("Tagalog"));
        insert("tha", QObject::trUtf8("Thai"));
        insert("tur", QObject::trUtf8("Turkish"));
        insert("ukr", QObject::trUtf8("Ukrainian"));
        insert("vie", QObject::trUtf8("Vietnamese"));
        insert("rum", QObject::trUtf8("Romanian"));
        insert("pob", QObject::trUtf8("Brazilian"));
}
