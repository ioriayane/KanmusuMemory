#ifndef TWEETDIALOG_H
#define TWEETDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPixmap>
#include <QtTwitterAPI>

#include "twitterinfo.h"

namespace Ui {
class TweetDialog;
}

class TweetDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TweetDialog(QWidget *parent = 0);
    ~TweetDialog();
    
    QString imagePath() const;
    void setImagePath(const QString &imagePath);

    const QString &token() const;
    void setToken(const QString &token);

    const QString &tokenSecret() const;
    void setTokenSecret(const QString &tokenSecret);

    const QString &user_id() const;
    void user_id(const QString &user_id);

    const QString &screen_name() const;
    void screen_name(const QString &screen_name);

private slots:
    void on_closeButton_clicked();
    void on_tweetButton_clicked();
    void on_closeButton_2_clicked();
    void on_tweetTextEdit_textChanged();

public slots:
    virtual int exec();
    void stateChanged(OAuth::State state);

private:
    Ui::TweetDialog *ui;

    QLabel m_imageLabel;
    QPixmap m_imagePixmap;

    QString m_imagePath;

    //ツイッター関連
    OAuth m_oauth;
    Status m_status;

    void authorize(bool force = false);

};

#endif // TWEETDIALOG_H
