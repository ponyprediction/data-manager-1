#ifndef JOB_HPP
#define JOB_HPP

#include <QObject>

class Manager : public QObject {
    Q_OBJECT
  public:
    Manager();
    ~Manager();
    void execute(const QString & command);
  public slots:
    void addFinishedDownolad();
  private:
    int downloadCount;
    int finishedDonwloadCount;
};

#endif // JOB_HPP
