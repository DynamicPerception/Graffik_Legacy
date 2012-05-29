#ifndef FILMGLOBALOPTIONS_H
#define FILMGLOBALOPTIONS_H

class FilmGlobalOptions
{
public:
    FilmGlobalOptions();
    ~FilmGlobalOptions();

    void metric(bool p_metric);
    bool isMetric();


private:
    bool m_metric;
};

#endif // FILMGLOBALOPTIONS_H
