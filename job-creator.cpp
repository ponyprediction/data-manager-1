#include "job-creator.hpp"
#include "util.hpp"

JobCreator::JobCreator()
{

}

JobCreator::~JobCreator()
{

}

void JobCreator::createJob(const QDate & dateStart,
                           const QDate & dateEnd,
                           const QDate & dateStartHistory)
{
    Util::addMessage("Creating job");
    // Init
    bool ok = true;
    QString error = "";
    //

    //

}
