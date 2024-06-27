#ifndef STUDENT_CHECK_IN_H
#define STUDENT_CHECK_IN_H

#include <QWidget>
#include "protocol.h"
namespace Ui {
class Student_Check_In;
}

class Student_Check_In : public QWidget
{
    Q_OBJECT

public:
    explicit Student_Check_In(QWidget *parent = nullptr);
    ~Student_Check_In();
    static Student_Check_In& getInstance();
    void updateList(PDU *pdu);


private:
    Ui::Student_Check_In *ui;
};

#endif // STUDENT_CHECK_IN_H
