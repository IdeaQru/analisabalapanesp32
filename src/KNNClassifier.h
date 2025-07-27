#ifndef KNN_CLASSIFIER_H
#define KNN_CLASSIFIER_H

#include "DataStructures.h"

class KNNClassifier {
private:
    static float featureMeans[5];
    static float featureStds[5];
    static TrainingData trainingSet[Config::TRAIN_DATA_SIZE];
    
    void normalizeFeatures(float* features);
    float calculateEuclideanDistance(const float* testFeatures, const float* trainFeatures);
    void findKNearest(Distance distances[], int size);

public:
    KNNClassifier();
    ~KNNClassifier();
    
    void initialize();
    int classify(const SensorData& data);
    String getClassificationText(int classification);
    uint16_t getClassificationColor(int classification);
    void logClassificationResult(const SensorData& data, int classification, unsigned long executionTime);
    
    // Static methods untuk akses global
    static KNNClassifier& getInstance() {
        static KNNClassifier instance;
        return instance;
    }
};

#endif // KNN_CLASSIFIER_H
