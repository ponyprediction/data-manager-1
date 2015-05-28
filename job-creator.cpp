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
    if(ok)
    {
        for(QDate date = dateStart ; date < dateEnd ; date = date.addDays(1))
        {



            for(QDate date2 = dateStartHistory ; date2 < date ; date2 = date2.addDays(1))
            {
                //
            }
        }
    }
    // The end
    if(ok)
    {
        //Util::addMessage("Done");
    }
    if(!ok)
    {
        Util::addError(error);
    }
}
