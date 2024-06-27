#ifndef SHARE_H
#define SHARE_H

#include <QWidget>

namespace Ui {
class Share;
}

class Share : public QWidget
{
    Q_OBJECT

public:
    explicit Share(QWidget *parent = nullptr);
    ~Share();
    static Share& getInstance();

private slots:
    void on_cancel_PB_clicked();

    void on_shareOK_PB_clicked();

    void on_flushFriend_PB_clicked();

private:
    Ui::Share *ui;
};

#endif // SHARE_H
