#ifndef UPDATEMOVIEHASHFORM_H
#define UPDATEMOVIEHASHFORM_H

#include <QWidget>
#include <QVariant>

class QValidator;

namespace Ui {
    class UpdateMovieHashForm;
}

class UpdateMovieHashForm : public QWidget {
    Q_OBJECT
public:
    UpdateMovieHashForm(QObject * obj, const char * sig, QWidget *parent = 0);
    ~UpdateMovieHashForm();

    void setSubtitle(const QMap<QString, QVariant> & subtitle);
    QVariant createUpdateHashArgs();

    void setLineNumberValidator(QValidator * v);

    bool isValid();
protected:
    void changeEvent(QEvent *e);

private:
    Ui::UpdateMovieHashForm *ui;
    QMap<QString, QVariant> m_subtitle;
};

#endif // UPDATEMOVIEHASHFORM_H
