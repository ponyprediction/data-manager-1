#ifndef JOBCREATOR_HPP
#define JOBCREATOR_HPP

#include <QDate>

class JobCreator
{
    public:
        JobCreator();
        ~JobCreator();
        static void createJob(const QDate & dateStart,
                              const QDate & dateEnd,
                              const QDate & dateStartHistory);
};

#endif // JOBCREATOR_HPP
