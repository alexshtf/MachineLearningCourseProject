#include "config.h"
#include <QString>

namespace {
    const QString NEIGHBORHOOD_SIZE_KEY = "NeighborhoodSize";
    const QString STOPPING_EPSILON_KEY = "StoppingEpsilon";
    const QString LAMBDA_KEY = "Lambda";

    const uint DEFAULT_NEIGHBORHOOD_SIZE = 2;
    const double DEFAULT_STOPPING_EPSILON = 1E-5;
    const double DEFAULT_LAMBDA = 1;
}

Config::Config(QSettings &settings)
    : _settings(settings)
    , _neighborhoodSize(settings.value(NEIGHBORHOOD_SIZE_KEY, DEFAULT_NEIGHBORHOOD_SIZE).toUInt())
    , _stoppingEpsilon(settings.value(STOPPING_EPSILON_KEY, DEFAULT_STOPPING_EPSILON).toDouble())
    , _lambda(settings.value(LAMBDA_KEY, DEFAULT_LAMBDA).toDouble())
{
}

Config::~Config()
{
    save();
}

size_t Config::getNeighborhoodSize() const
{
    return _neighborhoodSize;
}

void Config::setNeighborhoodSize(size_t neighborhoodSize)
{
    _neighborhoodSize = neighborhoodSize;
}

double Config::getStoppingEpsilon() const
{
    return _stoppingEpsilon;
}

void Config::setStoppingEpsilon(double stoppingEpsilon)
{
    _stoppingEpsilon = stoppingEpsilon;
}

double Config::getLambda() const
{
    return _lambda;
}

void Config::setLambda(double lambda)
{
    _lambda = lambda;
}

void Config::save()
{
    _settings.setValue(NEIGHBORHOOD_SIZE_KEY, QVariant::fromValue((uint) _neighborhoodSize));
    _settings.setValue(STOPPING_EPSILON_KEY, QVariant::fromValue(_stoppingEpsilon));
    _settings.setValue(LAMBDA_KEY, QVariant::fromValue(_lambda));
}

