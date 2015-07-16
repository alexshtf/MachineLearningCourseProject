#ifndef SCRIBBLEMEDIATOR_H
#define SCRIBBLEMEDIATOR_H

#include <QObject>

class ScribbleMediator : public QObject
{
    Q_OBJECT
public:
    explicit ScribbleMediator(QObject *parent = 0);

signals:

public slots:
};

#endif // SCRIBBLEMEDIATOR_H
