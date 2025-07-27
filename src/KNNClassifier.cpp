#include "KNNClassifier.h"

// Static data initialization
float KNNClassifier::featureMeans[5] = {14.458681, 3523.169792, 97.174050, 59.825399, 141.621621};
float KNNClassifier::featureStds[5] = {1.234978, 613.592394, 14.246689, 19.300085, 25.049101};

TrainingData KNNClassifier::trainingSet[Config::TRAIN_DATA_SIZE] = {
    {14.4, 2531, 89.4, 44.1, 117.8, 0}, {14.3, 3270, 86.5, 54.8, 127.6, 0},
    {14.8, 3034, 85.8, 40.6, 119.3, 0}, {14.4, 3106, 78.7, 62.3, 119.0, 0},
    {14.7, 2983, 86.4, 31.5, 149.3, 0}, {14.3, 3173, 84.0, 49.5, 112.8, 0},
    {14.3, 3315, 81.5, 28.1, 101.3, 0}, {14.8, 3183, 79.5, 41.2, 134.6, 0},
    {14.5, 3004, 84.9, 47.4, 118.3, 0}, {14.5, 2700, 82.5, 55.1, 126.6, 0},
    {14.7, 3352, 85.9, 29.4, 124.8, 0}, {14.5, 3149, 92.3, 56.5, 145.8, 0},
    {14.0, 2718, 84.3, 30.5, 127.2, 0}, {14.4, 2492, 84.5, 33.1, 106.8, 0},
    {14.2, 2994, 83.6, 48.9, 110.1, 0}, {14.4, 3412, 81.8, 35.4, 114.2, 0},
    {14.5, 2789, 81.7, 28.2, 141.0, 0}, {14.7, 2831, 88.2, 56.7, 127.5, 0},
    {14.4, 3051, 91.6, 32.9, 133.7, 0}, {14.6, 3247, 94.5, 42.1, 111.0, 0},
    {14.1, 2874, 83.3, 35.4, 118.1, 0}, {14.6, 3748, 85.0, 55.1, 121.0, 0},
    {14.5, 3509, 81.9, 47.3, 111.1, 0}, {14.6, 2746, 81.8, 57.4, 116.0, 0},
    {14.4, 2486, 91.8, 43.6, 134.9, 0}, {14.5, 2949, 85.4, 58.9, 108.9, 0},
    {14.4, 3066, 86.2, 63.9, 118.9, 0}, {14.4, 2922, 93.0, 51.7, 116.5, 0},
    {14.1, 3063, 75.2, 29.1, 122.4, 0}, {14.3, 3564, 77.8, 20.0, 125.3, 0},
    {14.7, 2964, 87.1, 34.4, 114.8, 0}, {14.3, 2968, 79.8, 38.4, 105.6, 0},
    {14.1, 3149, 82.7, 43.7, 150.0, 0}, {14.9, 3277, 85.3, 37.2, 128.4, 0},
    {14.4, 2868, 92.2, 47.4, 132.4, 0}, {14.4, 3337, 89.7, 35.7, 124.9, 0},
    {14.6, 2706, 87.0, 24.6, 132.3, 0}, {14.8, 2661, 87.6, 62.3, 90.3, 0},
    {14.8, 2705, 83.9, 51.6, 108.4, 0}, {14.4, 3348, 79.6, 52.4, 127.1, 0},
    {14.4, 2713, 87.1, 69.8, 107.2, 0}, {14.1, 3275, 95.6, 57.4, 101.8, 0},
    {14.6, 2883, 86.3, 40.9, 127.5, 0}, {14.3, 3767, 82.2, 47.2, 138.5, 0},
    {14.5, 3444, 85.4, 34.7, 138.3, 0}, {14.8, 3103, 87.3, 51.8, 125.4, 0},
    {14.4, 3173, 86.6, 70.0, 133.4, 0}, {15.0, 2440, 88.4, 25.6, 114.3, 0},
    {14.0, 2616, 81.9, 45.3, 126.2, 0}, {14.2, 2586, 81.9, 55.3, 131.4, 0},
    {13.6, 3408, 85.0, 49.6, 145.1, 1}, {15.0, 3977, 106.9, 81.5, 134.8, 1},
    {15.1, 3252, 107.0, 40.6, 174.6, 1}, {14.9, 4057, 94.5, 61.7, 141.3, 1},
    // ... (sisa data training - total 150 samples) ...
    {16.2, 4914, 110.0, 97.1, 140.0, 2}
};

KNNClassifier::KNNClassifier() {
    // Constructor
}

KNNClassifier::~KNNClassifier() {
    // Destructor
}

void KNNClassifier::initialize() {
    Serial.println("=== KNN Classifier Initialized ===");
    Serial.printf("Training samples: %d\n", Config::TRAIN_DATA_SIZE);
    Serial.printf("K-value: %d\n", Config::K_VALUE);
    Serial.println("Features: AFR, RPM, Temperature, TPS, MAP");
    Serial.println("Classes: 0=Normal, 1=Maintenance, 2=Critical");
}

void KNNClassifier::normalizeFeatures(float* features) {
    for (int i = 0; i < 5; i++) {
        features[i] = (features[i] - featureMeans[i]) / featureStds[i];
    }
}

float KNNClassifier::calculateEuclideanDistance(const float* testFeatures, const float* trainFeatures) {
    float dist = 0;
    for (int i = 0; i < 5; i++) {
        float diff = testFeatures[i] - trainFeatures[i];
        dist += diff * diff;
    }
    return sqrt(dist);
}

void KNNClassifier::findKNearest(Distance distances[], int size) {
    for (int i = 0; i < Config::K_VALUE; i++) {
        int minIdx = i;
        for (int j = i + 1; j < size; j++) {
            if (distances[j].distance < distances[minIdx].distance) {
                minIdx = j;
            }
        }
        Distance temp = distances[i];
        distances[i] = distances[minIdx];
        distances[minIdx] = temp;
    }
}

int KNNClassifier::classify(const SensorData& data) {
    float features[5] = {data.afr, data.rpm, data.temp, data.tps, data.map_value};
    normalizeFeatures(features);
    
    Distance distances[Config::TRAIN_DATA_SIZE];
    
    for (int i = 0; i < Config::TRAIN_DATA_SIZE; i++) {
        float trainFeatures[5] = {
            trainingSet[i].afr, trainingSet[i].rpm, trainingSet[i].temp, 
            trainingSet[i].tps, trainingSet[i].map_value
        };
        normalizeFeatures(trainFeatures);
        
        distances[i].distance = calculateEuclideanDistance(features, trainFeatures);
        distances[i].classification = trainingSet[i].classification;
    }
    
    findKNearest(distances, Config::TRAIN_DATA_SIZE);
    
    int votes[3] = {0, 0, 0};
    for (int i = 0; i < Config::K_VALUE; i++) {
        votes[distances[i].classification]++;
    }
    
    int maxVotes = 0;
    int result = 0;
    for (int i = 0; i < 3; i++) {
        if (votes[i] > maxVotes) {
            maxVotes = votes[i];
            result = i;
        }
    }
    
    if (maxVotes == 1) {
        result = distances[0].classification;
    }
    
    return result;
}

String KNNClassifier::getClassificationText(int classification) {
    switch (classification) {
        case 0: return "Normal";
        case 1: return "Maintenance";
        case 2: return "Critical";
        default: return "Unknown";
    }
}

uint16_t KNNClassifier::getClassificationColor(int classification) {
    switch (classification) {
        case 0: return ST77XX_GREEN;
        case 1: return ST77XX_YELLOW;
        case 2: return ST77XX_RED;
        default: return ST77XX_WHITE;
    }
}

void KNNClassifier::logClassificationResult(const SensorData& data, int classification, unsigned long executionTime) {
    Serial.printf("KNN_AI: %s (AFR:%.1f RPM:%.0f TEMP:%.1f TPS:%.1f MAP:%.1f) - %lums\n",
                  getClassificationText(classification).c_str(), 
                  data.afr, data.rpm, data.temp, data.tps, data.map_value, 
                  executionTime);
}
