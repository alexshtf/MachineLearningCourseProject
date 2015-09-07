#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>

// Configuration class. Specified the properties that the user can configure in the 'Configure'
// dialog. The properties are saved to the specified QSettings object and loaded from this object
// on construction.
class Config
{
public:
    Config(QSettings& settings);
    ~Config();

    size_t getNeighborhoodSize() const;
    void setNeighborhoodSize(size_t neighborhoodSize);

    double getStoppingEpsilon() const;
    void setStoppingEpsilon(double stoppingEpsilon);

    double getLambda() const;
    void setLambda(double getLambda);

    void save();
private:
    QSettings& _settings;
    size_t _neighborhoodSize;
    double _stoppingEpsilon;
    double _lambda;
};

#endif // CONFIG_H
