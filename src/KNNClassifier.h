#ifndef KNN_CLASSIFIER_H
#define KNN_CLASSIFIER_H

#include <Arduino.h>
#include "Config.h"
#include "DataStructures.h"
#include <Adafruit_ST7735.h>

// Training data structure for 4-class system


class KNNClassifier {
private:
    // Static arrays untuk training data (akan diisi dari Python)
    static float featureMeans[5];
    static float featureStds[5];
    static TrainingData trainingSet[200];  // Fixed size 200 for 4-class training
    
    // Private methods
    void normalizeFeatures(float* features);
    float calculateEuclideanDistance(const float* testFeatures, const float* trainFeatures);
    void findKNearest(Distance distances[], int size);
    int runKNNClassification(float afr, float rpm, float temp, float tps, float mapValue);

public:
    KNNClassifier();
    ~KNNClassifier();
    
    void initialize();
    
    // Main classification methods
    int classify(const SensorData& data);  // Uses hybrid approach
    int classifyEngineCondition(float afr, float rpm, float temp, float tps, float mapValue);
    
    // Utility methods
    String getClassificationText(int classification);
    uint16_t getClassificationColor(int classification);
    void logClassificationResult(const SensorData& data, int classification, unsigned long executionTime);
    
    // Debug methods
    void printTrainingDataSample();
    void printNormalizationParams();
    
    // Static instance for global access
    static KNNClassifier& getInstance() {
        static KNNClassifier instance;
        return instance;
    }
};

#endif // KNN_CLASSIFIER_H
