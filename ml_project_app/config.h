#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>

class Config
{
public:
    Config(QSettings& settings);
    ~Config();

    size_t getNeighborhoodSize() const;
    void setNeighborhoodSize(size_t neighborhoodSize);

    double getStoppingEpsilon() const;
    void setStoppingEpsilon(double stoppingEpsilon);

    void save();

private:
    QSettings& _settings;
    size_t _neighborhoodSize;
    double _stoppingEpsilon;
};

#endif // CONFIG_H
