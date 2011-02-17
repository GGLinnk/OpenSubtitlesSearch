#include "updatemoviehashform.h"
#include "ui_updatemoviehashform.h"
#include "mainwindow.h"

UpdateMovieHashForm::UpdateMovieHashForm(QObject * obj, const char * sig, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdateMovieHashForm)
{
    ui->setupUi(this);
    connect(ui->widgetMovie, SIGNAL(validStateChanged(bool)), obj, sig);
}

UpdateMovieHashForm::~UpdateMovieHashForm()
{
    delete ui;
}

void UpdateMovieHashForm::setLineNumberValidator(QValidator *v) {
    ui->frames->setValidator(v);
    ui->ms->setValidator(v);
}

void UpdateMovieHashForm::setSubtitle(const QMap<QString, QVariant> &subtitle) {
    m_subtitle = subtitle;
    ui->label_sstitre->setText(subtitle.value("SubFileName").toString());
}

QVariant UpdateMovieHashForm::createUpdateHashArgs() {
    OpenSubtitlesRPCClient * client = MainWindow::instance()->client();
    QFile f(ui->widgetMovie->text());
    if (!f.open(QIODevice::ReadOnly)) return QVariant();
    double fps = ui->fps->value();
    QVariant res = client->createTryUploadSubtitlesCd(m_subtitle.value("SubFileName").toString(),
                                       m_subtitle.value("SubHash").toString(),
                                       &f,
                                       ui->ms->text(),
                                       ui->frames->text(),
                                       fps == 0.0 ? "" : QString::number(fps));
    f.close();
    return res;
}

bool UpdateMovieHashForm::isValid() {
    return ui->widgetMovie->isValid();
}

void UpdateMovieHashForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
