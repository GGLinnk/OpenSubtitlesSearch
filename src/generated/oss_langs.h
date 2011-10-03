#include <QMap>
#include <QString>

class OssLangs : public QMap<QString, QString> {
protected:
    explicit OssLangs();
    static OssLangs * _instance;
public:
    static OssLangs * instance();
};


