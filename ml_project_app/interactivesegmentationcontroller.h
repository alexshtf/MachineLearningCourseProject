#ifndef SEGMENTATIONENGINE_H
#define SEGMENTATIONENGINE_H

#include "scribblemaskgenerator.h"
#include "PixelsLabelsArray.h"
#include "metatypes.h"

#include <QPainterPath>
#include <QList>
#include <QMap>
#include <boost/multi_array.hpp>

// Contains the logic of the interaction segmentation process. The process includes the following parts:
//   Initialization:
//       A call to reset(). The process starts given a new image. From now on the user can add scribbles
//       and recompute the segmentation of this image.
//
//   Actions:
//      addScribble()
//          adds a new path scribble by the user for a given label. This scribble will be used for the next
//          computation
//      recompute()
//          Computes a segmentation given the current set of scribbles
//
//   Queries:
//      getLabels()
//          Gets the list of labels for which scribbles exist
//      getMaskOf()
//          Gets a mask of pixels that are labeled with the specified label. '1' for the pixels that belong to this
//          segment and '0' otherwise.
//
//  Others:
//      saveSimilarity()
//          Saves the 3D array of size width x height x labels, of the unary potentials of the MRF. We call them
//          'similarity' since they measure how similar is each pixel with each label. Used for debugging purposes
//          by loading this image into MATLAB (using the multibandread command).
class InteractiveSegmentationController : public QObject
{
    Q_OBJECT
public:
    InteractiveSegmentationController(const class Config& config, class ComputeSegmentationWorker* worker, QObject* parent = nullptr);
    ~InteractiveSegmentationController();

    void reset(QImage image);
    void addScribble(QPainterPath path, int labelId);
    void recompute();

    QList<int> getLabels();
    QBitmap getMaskOf(int labelId);

    void saveSimilarity(const QString& fileName);

signals:
    // signals for the UI
    void startedRecompute();
    void trainedSVM();
    void computedSimilarity();
    void createdMRF();
    void mapInitialized();
    void iterationFinished(uint num, double dual);
    void recomputeDone();

    // signal to the worker - to start computing
    void compute(const QImage& image, const Common::PixelsLabelsArray& descriptors, QMap<int, QVector<QPoint>> scribbles);

private slots:
    void onRecomputeDone(boost::multi_array<size_t, 2> segmentation);

private:
    const class Config& _config;
    class ComputeSegmentationWorker* _worker;

    QImage _image;
    Common::PixelsLabelsArray _descriptors;
    QMap<int, QList<QPainterPath>> _scribbles;
    QMap<int, ScribbleMaskGenerator> _generators;
    boost::multi_array<size_t, 2> _segmentation;
};

#endif // SEGMENTATIONENGINE_H
