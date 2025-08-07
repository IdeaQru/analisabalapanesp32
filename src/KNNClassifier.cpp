#include "KNNClassifier.h"

// Static data initialization
float KNNClassifier::featureMeans[5] = {13.857951, 2830.976945, 90.309151, 37.198405, 119.460709};

float KNNClassifier::featureStds[5] = {1.444298, 928.185272, 14.293388, 31.700491, 33.515851};

TrainingData KNNClassifier::trainingSet[Config::TRAIN_DATA_SIZE]= {
  {14.8, 2705, 83.9, 51.6, 108.4, 0},  // Sample 1
  {14.1, 3093, 88.2, 50.0, 117.8, 0},  // Sample 2
  {14.6, 2985, 83.8, 34.1, 113.1, 0},  // Sample 3
  {14.3, 2703, 84.4, 45.7, 133.1, 0},  // Sample 4
  {14.7, 3150, 80.1, 46.2, 129.0, 0},  // Sample 5
  {14.4, 3539, 88.2, 38.1, 126.9, 0},  // Sample 6
  {14.4, 3066, 86.2, 63.9, 118.9, 0},  // Sample 7
  {14.4, 3202, 94.5, 43.4, 108.3, 0},  // Sample 8
  {14.4, 3330, 89.1, 54.8, 135.7, 0},  // Sample 9
  {14.5, 2989, 88.6, 45.6, 128.8, 0},  // Sample 10
  {14.6, 2529, 79.4, 30.7, 121.7, 0},  // Sample 11
  {14.5, 2655, 86.9, 37.8, 116.5, 0},  // Sample 12
  {14.8, 3356, 88.2, 31.3, 139.6, 0},  // Sample 13
  {15.1, 3527, 83.8, 56.7, 127.7, 0},  // Sample 14
  {14.8, 2794, 96.3, 56.8, 116.1, 0},  // Sample 15
  {14.7, 2727, 92.0, 28.2, 127.0, 0},  // Sample 16
  {14.7, 2629, 78.4, 51.3, 123.6, 0},  // Sample 17
  {14.5, 3245, 92.0, 51.7, 120.1, 0},  // Sample 18
  {14.4, 3067, 82.6, 60.1, 109.3, 0},  // Sample 19
  {13.9, 2864, 72.9, 26.0, 129.1, 0},  // Sample 20
  {14.5, 2789, 81.7, 28.2, 141.0, 0},  // Sample 21
  {14.8, 3485, 78.8, 37.9, 119.7, 0},  // Sample 22
  {14.6, 2971, 82.3, 49.8, 119.5, 0},  // Sample 23
  {14.2, 2820, 74.4, 38.7, 110.9, 0},  // Sample 24
  {14.6, 2993, 80.5, 48.9, 105.9, 0},  // Sample 25
  {14.6, 2400, 83.8, 36.8, 108.0, 0},  // Sample 26
  {14.4, 3051, 91.6, 32.9, 133.7, 0},  // Sample 27
  {14.0, 3379, 80.5, 37.2, 112.9, 0},  // Sample 28
  {14.4, 3413, 84.2, 46.5, 117.9, 0},  // Sample 29
  {14.7, 3142, 84.6, 34.8, 101.8, 0},  // Sample 30
  {14.1, 3149, 82.7, 43.7, 150.0, 0},  // Sample 31
  {14.4, 3257, 86.1, 30.1, 122.1, 0},  // Sample 32
  {14.3, 2833, 81.8, 59.3, 137.0, 0},  // Sample 33
  {14.7, 2900, 90.9, 49.4, 118.7, 0},  // Sample 34
  {14.3, 2694, 88.5, 47.9, 113.2, 0},  // Sample 35
  {14.2, 2994, 83.6, 48.9, 110.1, 0},  // Sample 36
  {14.6, 3461, 84.8, 63.8, 90.0, 0},  // Sample 37
  {14.4, 2799, 90.2, 37.7, 141.9, 0},  // Sample 38
  {14.7, 2831, 88.2, 56.7, 127.5, 0},  // Sample 39
  {13.8, 3687, 78.1, 25.3, 132.3, 0},  // Sample 40
  {14.3, 2589, 84.6, 70.0, 110.4, 0},  // Sample 41
  {14.4, 3098, 78.7, 56.1, 117.8, 0},  // Sample 42
  {14.7, 2400, 90.7, 24.1, 115.7, 0},  // Sample 43
  {14.8, 3196, 77.1, 62.7, 136.6, 0},  // Sample 44
  {14.1, 2874, 83.3, 35.4, 118.1, 0},  // Sample 45
  {14.7, 3218, 87.6, 37.3, 125.2, 0},  // Sample 46
  {14.4, 3348, 79.6, 52.4, 127.1, 0},  // Sample 47
  {14.3, 2696, 86.6, 34.1, 103.1, 0},  // Sample 48
  {14.7, 2854, 95.8, 37.7, 128.9, 0},  // Sample 49
  {14.3, 3056, 81.2, 37.7, 103.1, 0},  // Sample 50
  {15.7, 1631, 75.5, 0.0, 86.2, 1},  // Sample 51
  {14.3, 1863, 91.6, 0.0, 77.6, 1},  // Sample 52
  {13.3, 2102, 76.5, 0.0, 96.9, 1},  // Sample 53
  {14.7, 2001, 82.8, 0.0, 104.4, 1},  // Sample 54
  {14.9, 1200, 85.7, 0.0, 70.3, 1},  // Sample 55
  {13.7, 2033, 74.0, 0.0, 73.7, 1},  // Sample 56
  {15.8, 1722, 84.8, 0.0, 76.4, 1},  // Sample 57
  {14.9, 1859, 77.1, 0.0, 80.3, 1},  // Sample 58
  {14.1, 1678, 74.0, 0.0, 94.8, 1},  // Sample 59
  {14.8, 2071, 74.7, 0.0, 107.8, 1},  // Sample 60
  {13.8, 1833, 92.6, 0.0, 89.7, 1},  // Sample 61
  {15.3, 1458, 90.3, 0.0, 83.9, 1},  // Sample 62
  {15.1, 1895, 85.8, 0.0, 62.0, 1},  // Sample 63
  {16.1, 1831, 74.8, 0.0, 65.0, 1},  // Sample 64
  {14.0, 1513, 84.8, 0.0, 84.3, 1},  // Sample 65
  {13.3, 1864, 82.0, 0.0, 72.7, 1},  // Sample 66
  {15.5, 1935, 91.5, 0.0, 67.3, 1},  // Sample 67
  {15.0, 1625, 65.0, 0.0, 83.0, 1},  // Sample 68
  {14.6, 1573, 90.4, 0.0, 109.8, 1},  // Sample 69
  {15.3, 2117, 84.9, 0.0, 87.2, 1},  // Sample 70
  {15.4, 1865, 86.1, 0.0, 110.0, 1},  // Sample 71
  {14.6, 1785, 95.0, 0.0, 68.3, 1},  // Sample 72
  {15.7, 1659, 81.3, 0.0, 104.9, 1},  // Sample 73
  {16.4, 1801, 90.1, 0.0, 105.1, 1},  // Sample 74
  {15.5, 2066, 75.9, 0.0, 85.6, 1},  // Sample 75
  {14.1, 2136, 82.0, 0.0, 84.9, 1},  // Sample 76
  {13.0, 1801, 84.6, 0.0, 98.9, 1},  // Sample 77
  {14.9, 2265, 72.1, 0.0, 63.0, 1},  // Sample 78
  {14.4, 1434, 77.9, 0.0, 82.8, 1},  // Sample 79
  {16.1, 2096, 89.0, 0.0, 78.2, 1},  // Sample 80
  {14.2, 1825, 85.7, 0.0, 106.8, 1},  // Sample 81
  {14.3, 2227, 95.0, 0.0, 99.7, 1},  // Sample 82
  {13.9, 1294, 67.6, 0.0, 64.7, 1},  // Sample 83
  {16.2, 1800, 72.5, 0.0, 91.7, 1},  // Sample 84
  {16.0, 2099, 84.2, 0.0, 110.0, 1},  // Sample 85
  {14.3, 1745, 77.2, 0.0, 60.0, 1},  // Sample 86
  {14.8, 1499, 87.9, 0.0, 77.3, 1},  // Sample 87
  {14.4, 1861, 72.4, 0.0, 79.1, 1},  // Sample 88
  {15.3, 2081, 69.1, 0.0, 73.6, 1},  // Sample 89
  {13.6, 1200, 78.4, 0.0, 93.3, 1},  // Sample 90
  {14.9, 1558, 75.4, 0.0, 92.8, 1},  // Sample 91
  {15.2, 1724, 81.0, 0.0, 89.9, 1},  // Sample 92
  {14.4, 1755, 81.4, 0.0, 78.2, 1},  // Sample 93
  {14.4, 1656, 84.4, 0.0, 91.9, 1},  // Sample 94
  {14.9, 2500, 84.9, 0.0, 80.3, 1},  // Sample 95
  {16.4, 1461, 85.8, 0.0, 71.1, 1},  // Sample 96
  {13.9, 2373, 71.8, 0.0, 87.8, 1},  // Sample 97
  {14.5, 1977, 69.2, 0.0, 91.9, 1},  // Sample 98
  {15.3, 1978, 79.2, 0.0, 76.4, 1},  // Sample 99
  {15.1, 1331, 82.8, 0.0, 81.0, 1},  // Sample 100
  {11.4, 3854, 110.0, 96.4, 196.8, 2},  // Sample 101
  {11.6, 3707, 122.3, 42.2, 168.0, 2},  // Sample 102
  {11.3, 4343, 99.4, 44.2, 161.4, 2},  // Sample 103
  {11.1, 4100, 116.3, 86.2, 160.6, 2},  // Sample 104
  {11.6, 3280, 95.0, 92.2, 144.3, 2},  // Sample 105
  {11.3, 3448, 102.6, 67.3, 172.2, 2},  // Sample 106
  {11.4, 4194, 109.1, 85.0, 151.7, 2},  // Sample 107
  {11.4, 3771, 118.2, 60.0, 133.8, 2},  // Sample 108
  {11.6, 4756, 127.2, 55.5, 190.2, 2},  // Sample 109
  {11.2, 3924, 97.6, 59.8, 200.0, 2},  // Sample 110
  {11.5, 3790, 125.7, 81.8, 156.6, 2},  // Sample 111
  {11.4, 4575, 110.8, 79.6, 191.9, 2},  // Sample 112
  {11.2, 4403, 130.0, 92.8, 184.0, 2},  // Sample 113
  {11.4, 4873, 122.1, 72.9, 153.7, 2},  // Sample 114
  {11.5, 3538, 95.0, 78.9, 193.6, 2},  // Sample 115
  {11.1, 4597, 117.9, 65.7, 130.0, 2},  // Sample 116
  {11.6, 3310, 115.2, 100.0, 130.0, 2},  // Sample 117
  {11.7, 3724, 95.0, 62.7, 169.9, 2},  // Sample 118
  {11.6, 4043, 95.0, 61.1, 142.0, 2},  // Sample 119
  {11.2, 4189, 130.0, 51.6, 178.0, 2},  // Sample 120
  {11.8, 4184, 97.6, 80.5, 194.9, 2},  // Sample 121
  {11.4, 3200, 130.0, 92.9, 158.6, 2},  // Sample 122
  {12.0, 4066, 100.6, 84.9, 130.0, 2},  // Sample 123
  {11.5, 4227, 115.7, 63.1, 172.9, 2},  // Sample 124
  {11.8, 4633, 107.2, 61.5, 137.6, 2},  // Sample 125
  {11.2, 5011, 125.0, 100.0, 148.0, 2},  // Sample 126
  {11.3, 3661, 116.3, 81.6, 168.5, 2},  // Sample 127
  {11.4, 3782, 110.7, 73.8, 152.4, 2},  // Sample 128
  {11.6, 4094, 125.4, 78.5, 131.6, 2},  // Sample 129
  {11.9, 3890, 104.6, 98.6, 149.3, 2},  // Sample 130
  {11.5, 4213, 130.0, 96.5, 151.3, 2},  // Sample 131
  {11.7, 4827, 108.7, 63.2, 186.1, 2},  // Sample 132
  {11.7, 4564, 96.5, 43.5, 134.5, 2},  // Sample 133
  {11.5, 3200, 109.5, 87.8, 168.1, 2},  // Sample 134
  {11.3, 3200, 108.5, 63.0, 140.8, 2},  // Sample 135
  {11.7, 3982, 101.8, 86.9, 159.2, 2},  // Sample 136
  {11.8, 3516, 95.0, 86.2, 169.7, 2},  // Sample 137
  {11.9, 4737, 109.8, 100.0, 153.5, 2},  // Sample 138
  {11.2, 4696, 124.6, 59.4, 170.1, 2},  // Sample 139
  {11.8, 3306, 95.0, 44.0, 181.6, 2},  // Sample 140
  {11.6, 3944, 106.3, 51.5, 162.4, 2},  // Sample 141
  {11.4, 3660, 122.5, 79.2, 140.4, 2},  // Sample 142
  {11.1, 3803, 122.0, 52.8, 193.7, 2},  // Sample 143
  {11.3, 3668, 130.0, 99.0, 172.1, 2},  // Sample 144
  {11.5, 4419, 106.2, 100.0, 175.0, 2},  // Sample 145
  {12.0, 4302, 104.8, 65.1, 179.9, 2},  // Sample 146
  {11.9, 3725, 107.6, 67.6, 131.3, 2},  // Sample 147
  {11.8, 4096, 118.6, 90.7, 180.5, 2},  // Sample 148
  {11.2, 4670, 130.0, 70.0, 200.0, 2},  // Sample 149
  {11.6, 5031, 95.0, 76.9, 156.9, 2}  // Sample 150
};

KNNClassifier::KNNClassifier()
{
    // Constructor
}

KNNClassifier::~KNNClassifier()
{
    // Destructor
}

void KNNClassifier::initialize()
{
    Serial.println("=== KNN Classifier Initialized ===");
    Serial.printf("Training samples: %d\n", Config::TRAIN_DATA_SIZE);
    Serial.printf("K-value: %d\n", Config::K_VALUE);
    Serial.println("Features: AFR, RPM, Temperature, TPS, MAP");
    Serial.println("Classes: 0=Normal, 1=Maintenance, 2=Critical");
}

void KNNClassifier::normalizeFeatures(float *features)
{
    for (int i = 0; i < 5; i++)
    {
        features[i] = (features[i] - featureMeans[i]) / featureStds[i];
    }
}

float KNNClassifier::calculateEuclideanDistance(const float *testFeatures, const float *trainFeatures)
{
    float dist = 0;
    for (int i = 0; i < 5; i++)
    {
        float diff = testFeatures[i] - trainFeatures[i];
        dist += diff * diff;
    }
    return sqrt(dist);
}

void KNNClassifier::findKNearest(Distance distances[], int size)
{
    for (int i = 0; i < Config::K_VALUE; i++)
    {
        int minIdx = i;
        for (int j = i + 1; j < size; j++)
        {
            if (distances[j].distance < distances[minIdx].distance)
            {
                minIdx = j;
            }
        }
        Distance temp = distances[i];
        distances[i] = distances[minIdx];
        distances[minIdx] = temp;
    }
}

int KNNClassifier::classify(const SensorData &data)
{
    float features[5] = {data.afr, data.rpm, data.temp, data.tps, data.map_value};
    normalizeFeatures(features);

    Distance distances[Config::TRAIN_DATA_SIZE];

    for (int i = 0; i < Config::TRAIN_DATA_SIZE; i++)
    {
        float trainFeatures[5] = {
            trainingSet[i].afr, trainingSet[i].rpm, trainingSet[i].temp,
            trainingSet[i].tps, trainingSet[i].map_value};
        normalizeFeatures(trainFeatures);

        distances[i].distance = calculateEuclideanDistance(features, trainFeatures);
        distances[i].classification = trainingSet[i].classification;
    }

    findKNearest(distances, Config::TRAIN_DATA_SIZE);

    int votes[3] = {0, 0, 0};
    for (int i = 0; i < Config::K_VALUE; i++)
    {
        votes[distances[i].classification]++;
    }

    int maxVotes = 0;
    int result = 0;
    for (int i = 0; i < 3; i++)
    {
        if (votes[i] > maxVotes)
        {
            maxVotes = votes[i];
            result = i;
        }
    }

    if (maxVotes == 1)
    {
        result = distances[0].classification;
    }

    return result;
}

String KNNClassifier::getClassificationText(int classification)
{
    switch (classification)
    {
    case 0:
        return "Normal";
    case 1:
        return "Maintenance";
    case 2:
        return "Critical";
    default:
        return "Unknown";
    }
}

uint16_t KNNClassifier::getClassificationColor(int classification)
{
    switch (classification)
    {
    case 0:
        return ST77XX_GREEN;
    case 1:
        return ST77XX_YELLOW;
    case 2:
        return ST77XX_RED;
    default:
        return ST77XX_WHITE;
    }
}

void KNNClassifier::logClassificationResult(const SensorData &data, int classification, unsigned long executionTime)
{
    Serial.printf("KNN_AI: %s (AFR:%.1f RPM:%.0f TEMP:%.1f TPS:%.1f MAP:%.1f) - %lums\n",
                  getClassificationText(classification).c_str(),
                  data.afr, data.rpm, data.temp, data.tps, data.map_value,
                  executionTime);
}
