#ifndef JOB_HPP
#define JOB_HPP

#include <QObject>

class Manager : public QObject
{
    Q_OBJECT
  public:
    Manager();
    ~Manager();
    static void execute(const QString & command);
};

#endif // JOB_HPP
