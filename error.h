#ifndef ERROR_H
#define ERROR_H

#include <QtCore>
#include <QList>
#include <QString>

class Err
{
private:
  /// Stack of error messages
        QList<QString> list;
        QString disp;
        void display(int type);

protected:
  /** Use rhis constructor if you need to compute something before creating
  first error message and can't call constructor of this base class
  on the constructor initialization list */
//      Err() { }

public:
        void add(const char *msg);
        void add(const QString msg);
        void add(int err);
        void show(const char *msg);
        void show(const QString msg);
        void show(int err);
        void show();
        QString getMsg();
        void showmsg();

        /// General error creation
        Err(const char *msg) {
                disp="";
                add(msg);
        }
        Err(const QString msg) {
                disp="";
                add(msg);
        }
        Err(int err) {
                disp="";
                add(err);
        }
};

#endif // ERROR_H
