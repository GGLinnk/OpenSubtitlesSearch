#include "searchdialog.h"
#include "ui_searchdialog.h"
#include "client/OpenSubtitlesRPCClient.h"
#include "settings.h"

SearchDialog::SearchDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchDialog)
{
    ui->setupUi(this);
    ui->comboBox->addItem(trUtf8("Hash"));
    ui->comboBox->addItem(trUtf8("Text"));
    ui->comboBox->addItem(trUtf8("Mixed"));


    SearchByHash * byHash = new SearchByHash(this);
    SearchByQuery * byQuery = new SearchByQuery(this);
    SearchByMixed * byMixed = new SearchByMixed(this);

    ui->stackWidget->addWidget(byHash);
    ui->stackWidget->addWidget(byQuery);
    ui->stackWidget->addWidget(byMixed);

    QPushButton * ok = ui->buttonBox->button(QDialogButtonBox::Ok);
    ok->setEnabled(false);
    connect(byHash, SIGNAL(stateChanged(bool)), ok, SLOT(setEnabled(bool)));
    connect(byQuery, SIGNAL(stateChanged(bool)), ok, SLOT(setEnabled(bool)));
    connect(byMixed, SIGNAL(stateChanged(bool)), ok, SLOT(setEnabled(bool)));

    connect(ui->stackWidget, SIGNAL(currentChanged(int)), this, SLOT(modeChanged(int)));

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), ui->stackWidget, SLOT(setCurrentIndex(int)));

    ((SearchWidget*) ui->stackWidget->currentWidget())->setFocus();
}

SearchDialog::~SearchDialog()
{
    delete ui;
}

void SearchDialog::modeChanged(int index) {
    SearchWidget * w = (SearchWidget*) ui->stackWidget->widget(index);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(w->isValid());
    w->setFocus();
}

void SearchDialog::setText(const QString &txt) {
    SearchWidget * w = (SearchWidget*) ui->stackWidget->currentWidget();
    w->setText(txt);
}

SearchDialog::MODE SearchDialog::mode() {
    return (MODE) ui->comboBox->currentIndex();
}

void SearchDialog::setMode(SearchDialog::MODE m) {
    ui->comboBox->setCurrentIndex((int)m);
}

QString SearchDialog::selectedLangs() {
    QStringList res;
    QList<QCheckBox *> checks = ui->widgetLangs->findChildren<QCheckBox *>();
    foreach (QCheckBox * c, checks) {
        if (c->isChecked()) res << c->objectName();
    }
    return res.join(",");
}

void SearchDialog::setSelectedLangs(const QString & langs) {
    QStringList lst = langs.split(",");
    QList<QCheckBox *> checks = ui->widgetLangs->findChildren<QCheckBox *>();
    foreach (QCheckBox * c, checks) {
        bool checked = lst.contains(c->objectName());
        c->setChecked(checked);
    }
}

SearchWidget * SearchDialog::searchWidget() {
    return ((SearchWidget*) ui->stackWidget->currentWidget());
}

void SearchDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/***********************************************/

SearchByQuery::SearchByQuery(QWidget *parent) : SearchWidget(parent), Ui::SearchByQuery() {
    setupUi(this);
    connect(queryLineEdit, SIGNAL(textChanged(QString)), this, SLOT(verifyInput(QString)));
    verifyInput(queryLineEdit->text());
}

void SearchByQuery::verifyInput(const QString &text) {
    bool ok = !text.isEmpty();
    queryLineEdit->setStyleSheet(ok ? "" : "QLineEdit { color: red; }");
    emit stateChanged(ok);
}

QList<QVariant> SearchByQuery::createSearchArgs(OpenSubtitlesRPCClient * client, const QString & langs) {
    QList<QVariant> result;
    result << client->createSearchSubtitlesByFileName(queryLineEdit->text(), langs);
    return result;
}

/***********************************************/

SearchByHash::SearchByHash(QWidget * parent) : SearchWidget(parent), Ui::SearchByHash() {
    setupUi(this);
    connect(videoWidget, SIGNAL(validStateChanged(bool)), this, SIGNAL(stateChanged(bool)));
}

QList<QVariant> SearchByHash::createSearchArgs(OpenSubtitlesRPCClient * client, const QString & langs) {
    QList<QVariant> result;
    QFile f(videoWidget->text());
    if (f.open(QIODevice::ReadOnly)) {
        result << client->createSearchSubtitlesByHash(&f, langs);
        f.close();
    }
    return result;
}

/************************************************/

SearchByMixed::SearchByMixed(QWidget * parent) : SearchWidget(parent), Ui::SearchByMixed() {
    setupUi(this);
    connect(videoWidget, SIGNAL(validStateChanged(bool)), this, SIGNAL(stateChanged(bool)));
}

QList<QVariant> SearchByMixed::createSearchArgs(OpenSubtitlesRPCClient * client, const QString & langs) {
    QList<QVariant> result;
    QFile f(videoWidget->text());
    if (f.open(QIODevice::ReadOnly)) {
        result << client->createSearchSubtitlesByHash(&f, langs);
        f.close();
    }
    return result;
}
