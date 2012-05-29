#include "filmglobaloptions.h"

FilmGlobalOptions::FilmGlobalOptions()
{
}

FilmGlobalOptions::~FilmGlobalOptions() {

}

void FilmGlobalOptions::metric(bool p_metric) {
    m_metric = p_metric;
}

bool FilmGlobalOptions::isMetric() {
    return(m_metric);
}
