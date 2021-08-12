#include "error.h"

void Err::add(const char *msg) {
    if (QString::fromUtf8(msg).isEmpty()) return;

    list.append(QString::fromUtf8(msg));
}
void Err::add(const QString msg) {
    if(msg.isEmpty()) return;
    list.append(msg);
}
void Err::add(int err) {
   //     list.append(Vr().err[err]);
}
void Err::show(const char *msg) {
    if (!QString::fromUtf8(msg).isEmpty())
        disp.append(QString::fromUtf8(msg));
    while(!list.isEmpty()) {
        disp.append((disp.isEmpty() ? NULL : QString("\n"))+list.takeLast());
    }
    display(1);
}
void Err::show(const QString msg) {
    if(!msg.isEmpty())
        disp.append(msg);
    while(!list.isEmpty()) {
        disp.append((disp.isEmpty() ? NULL : QString("\n"))+list.takeLast());
    }
    display(1);
}
void Err::show(int err) {
    while(!list.isEmpty()) {
        disp.append((disp.isEmpty() ? NULL : QString("\n"))+list.takeLast());
    }
    display(1);
}
void Err::show() {
    while(!list.isEmpty()) {
        disp.append((disp.isEmpty() ? NULL : QString("\n"))+list.takeLast());
    }
    display(1);
}
QString Err::getMsg() {
    while(!list.isEmpty()) {
        disp.append((disp.isEmpty() ? NULL : QString("\n"))+list.takeLast());
    }
    return disp;
}
void Err::display(int type) {
    qDebug() << "9 ERROR:"<<disp;
    if(type==1)
        qDebug() << "9 ERROR:"<<disp;
    if(type==2)
        qDebug() << "9 INFO:"<<disp;
    disp="";
}
void Err::showmsg() {
    while(!list.isEmpty()) {
        disp.append((disp.isEmpty() ? NULL : QString("\n"))+list.takeLast());
    }
    display(2);
}
