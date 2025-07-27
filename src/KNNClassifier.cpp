#include "KNNClassifier.h"

// Static data initialization
float KNNClassifier::featureMeans[5] = {14.458681, 3523.169792, 97.174050, 59.825399, 141.621621};
float KNNClassifier::featureStds[5] = {1.234978, 613.592394, 14.246689, 19.300085, 25.049101};

TrainingData KNNClassifier::trainingSet[Config::TRAIN_DATA_SIZE] = {
    {14.4, 2531, 89.4, 44.1, 117.8, 0},   // Sample 1
    {14.3, 3270, 86.5, 54.8, 127.6, 0},   // Sample 2
    {14.8, 3034, 85.8, 40.6, 119.3, 0},   // Sample 3
    {14.4, 3106, 78.7, 62.3, 119.0, 0},   // Sample 4
    {14.7, 2983, 86.4, 31.5, 149.3, 0},   // Sample 5
    {14.3, 3173, 84.0, 49.5, 112.8, 0},   // Sample 6
    {14.3, 3315, 81.5, 28.1, 101.3, 0},   // Sample 7
    {14.8, 3183, 79.5, 41.2, 134.6, 0},   // Sample 8
    {14.5, 3004, 84.9, 47.4, 118.3, 0},   // Sample 9
    {14.5, 2700, 82.5, 55.1, 126.6, 0},   // Sample 10
    {14.7, 3352, 85.9, 29.4, 124.8, 0},   // Sample 11
    {14.5, 3149, 92.3, 56.5, 145.8, 0},   // Sample 12
    {14.0, 2718, 84.3, 30.5, 127.2, 0},   // Sample 13
    {14.4, 2492, 84.5, 33.1, 106.8, 0},   // Sample 14
    {14.2, 2994, 83.6, 48.9, 110.1, 0},   // Sample 15
    {14.4, 3412, 81.8, 35.4, 114.2, 0},   // Sample 16
    {14.5, 2789, 81.7, 28.2, 141.0, 0},   // Sample 17
    {14.7, 2831, 88.2, 56.7, 127.5, 0},   // Sample 18
    {14.4, 3051, 91.6, 32.9, 133.7, 0},   // Sample 19
    {14.6, 3247, 94.5, 42.1, 111.0, 0},   // Sample 20
    {14.1, 2874, 83.3, 35.4, 118.1, 0},   // Sample 21
    {14.6, 3748, 85.0, 55.1, 121.0, 0},   // Sample 22
    {14.5, 3509, 81.9, 47.3, 111.1, 0},   // Sample 23
    {14.6, 2746, 81.8, 57.4, 116.0, 0},   // Sample 24
    {14.4, 2486, 91.8, 43.6, 134.9, 0},   // Sample 25
    {14.5, 2949, 85.4, 58.9, 108.9, 0},   // Sample 26
    {14.4, 3066, 86.2, 63.9, 118.9, 0},   // Sample 27
    {14.4, 2922, 93.0, 51.7, 116.5, 0},   // Sample 28
    {14.1, 3063, 75.2, 29.1, 122.4, 0},   // Sample 29
    {14.3, 3564, 77.8, 20.0, 125.3, 0},   // Sample 30
    {14.7, 2964, 87.1, 34.4, 114.8, 0},   // Sample 31
    {14.3, 2968, 79.8, 38.4, 105.6, 0},   // Sample 32
    {14.1, 3149, 82.7, 43.7, 150.0, 0},   // Sample 33
    {14.9, 3277, 85.3, 37.2, 128.4, 0},   // Sample 34
    {14.4, 2868, 92.2, 47.4, 132.4, 0},   // Sample 35
    {14.4, 3337, 89.7, 35.7, 124.9, 0},   // Sample 36
    {14.6, 2706, 87.0, 24.6, 132.3, 0},   // Sample 37
    {14.8, 2661, 87.6, 62.3, 90.3, 0},    // Sample 38
    {14.8, 2705, 83.9, 51.6, 108.4, 0},   // Sample 39
    {14.4, 3348, 79.6, 52.4, 127.1, 0},   // Sample 40
    {14.4, 2713, 87.1, 69.8, 107.2, 0},   // Sample 41
    {14.1, 3275, 95.6, 57.4, 101.8, 0},   // Sample 42
    {14.6, 2883, 86.3, 40.9, 127.5, 0},   // Sample 43
    {14.3, 3767, 82.2, 47.2, 138.5, 0},   // Sample 44
    {14.5, 3444, 85.4, 34.7, 138.3, 0},   // Sample 45
    {14.8, 3103, 87.3, 51.8, 125.4, 0},   // Sample 46
    {14.4, 3173, 86.6, 70.0, 133.4, 0},   // Sample 47
    {15.0, 2440, 88.4, 25.6, 114.3, 0},   // Sample 48
    {14.0, 2616, 81.9, 45.3, 126.2, 0},   // Sample 49
    {14.2, 2586, 81.9, 55.3, 131.4, 0},   // Sample 50
    {13.6, 3408, 85.0, 49.6, 145.1, 1},   // Sample 51
    {15.0, 3977, 106.9, 81.5, 134.8, 1},  // Sample 52
    {15.1, 3252, 107.0, 40.6, 174.6, 1},  // Sample 53
    {14.9, 4057, 94.5, 61.7, 141.3, 1},   // Sample 54
    {15.4, 3733, 102.3, 44.7, 127.3, 1},  // Sample 55
    {14.0, 3692, 92.8, 85.0, 169.0, 1},   // Sample 56
    {13.9, 3843, 102.2, 63.0, 161.6, 1},  // Sample 57
    {13.9, 3575, 101.8, 77.7, 133.3, 1},  // Sample 58
    {15.5, 3246, 85.0, 63.1, 162.1, 1},   // Sample 59
    {15.1, 3270, 92.2, 85.0, 145.0, 1},   // Sample 60
    {14.4, 4135, 89.0, 74.0, 128.5, 1},   // Sample 61
    {13.7, 3771, 95.3, 35.0, 131.4, 1},   // Sample 62
    {13.7, 3311, 96.8, 82.0, 149.3, 1},   // Sample 63
    {15.1, 3416, 104.9, 78.3, 146.1, 1},  // Sample 64
    {15.5, 3599, 108.2, 41.2, 147.0, 1},  // Sample 65
    {15.3, 3267, 103.4, 68.3, 128.4, 1},  // Sample 66
    {13.5, 4066, 88.0, 35.0, 157.8, 1},   // Sample 67
    {14.3, 3770, 98.2, 50.5, 153.8, 1},   // Sample 68
    {14.0, 3000, 99.1, 51.3, 149.2, 1},   // Sample 69
    {14.4, 4047, 102.4, 74.5, 173.8, 1},  // Sample 70
    {13.9, 4026, 97.2, 55.8, 140.1, 1},   // Sample 71
    {15.3, 3988, 88.4, 82.7, 131.9, 1},   // Sample 72
    {14.9, 3348, 89.9, 48.4, 132.6, 1},   // Sample 73
    {15.8, 3314, 93.1, 85.0, 145.5, 1},   // Sample 74
    {14.8, 3651, 96.2, 44.4, 159.0, 1},   // Sample 75
    {15.3, 3530, 103.3, 63.8, 168.3, 1},  // Sample 76
    {15.2, 3481, 88.6, 61.7, 139.0, 1},   // Sample 77
    {14.0, 3000, 92.7, 69.5, 133.1, 1},   // Sample 78
    {13.5, 3584, 99.5, 44.1, 162.7, 1},   // Sample 79
    {13.5, 3310, 96.4, 53.9, 130.8, 1},   // Sample 80
    {13.5, 4223, 89.3, 54.1, 120.0, 1},   // Sample 81
    {14.0, 3293, 109.8, 35.0, 137.5, 1},  // Sample 82
    {14.8, 4155, 98.5, 46.8, 130.3, 1},   // Sample 83
    {16.0, 3995, 87.2, 74.1, 157.2, 1},   // Sample 84
    {15.5, 3714, 90.1, 46.8, 167.5, 1},   // Sample 85
    {14.0, 4207, 100.9, 35.0, 180.0, 1},  // Sample 86
    {14.0, 3271, 107.0, 62.0, 144.8, 1},  // Sample 87
    {13.6, 4265, 99.7, 71.7, 161.1, 1},   // Sample 88
    {12.7, 4500, 85.0, 74.1, 120.0, 1},   // Sample 89
    {13.4, 3557, 106.0, 85.0, 158.6, 1},  // Sample 90
    {15.4, 3780, 99.6, 35.0, 144.7, 1},   // Sample 91
    {15.1, 3629, 99.7, 75.0, 146.9, 1},   // Sample 92
    {15.1, 3713, 110.5, 58.9, 121.3, 1},  // Sample 93
    {14.2, 3457, 92.8, 75.4, 141.7, 1},   // Sample 94
    {13.4, 3206, 105.9, 60.3, 121.8, 1},  // Sample 95
    {14.7, 4218, 104.5, 35.8, 120.0, 1},  // Sample 96
    {12.6, 4375, 96.8, 35.8, 149.9, 1},   // Sample 97
    {15.3, 3847, 96.3, 73.3, 122.2, 1},   // Sample 98
    {14.9, 3359, 92.9, 77.6, 173.9, 1},   // Sample 99
    {15.7, 4185, 85.0, 57.9, 120.0, 1},   // Sample 100
    {12.9, 4780, 126.7, 66.4, 175.1, 2},  // Sample 101
    {15.9, 3807, 116.9, 56.1, 167.9, 2},  // Sample 102
    {12.3, 3681, 123.0, 66.1, 146.2, 2},  // Sample 103
    {15.5, 3847, 121.3, 78.6, 140.0, 2},  // Sample 104
    {12.1, 4195, 100.0, 100.0, 146.9, 2}, // Sample 105
    {16.6, 5003, 104.2, 75.3, 201.7, 2},  // Sample 106
    {17.2, 3896, 122.1, 90.2, 178.9, 2},  // Sample 107
    {10.9, 4216, 123.3, 100.0, 187.3, 2}, // Sample 108
    {15.9, 4943, 101.7, 79.5, 164.6, 2},  // Sample 109
    {12.4, 4309, 109.6, 78.0, 179.2, 2},  // Sample 110
    {11.8, 3677, 120.7, 87.8, 193.9, 2},  // Sample 111
    {16.5, 5180, 120.4, 83.7, 170.9, 2},  // Sample 112
    {16.7, 3922, 107.1, 68.0, 187.7, 2},  // Sample 113
    {12.8, 4064, 129.8, 65.9, 174.7, 2},  // Sample 114
    {16.4, 4197, 107.1, 76.9, 145.8, 2},  // Sample 115
    {12.3, 3500, 139.3, 100.0, 150.5, 2}, // Sample 116
    {16.5, 4009, 133.1, 100.0, 164.2, 2}, // Sample 117
    {16.5, 4555, 115.2, 79.2, 159.4, 2},  // Sample 118
    {14.4, 3860, 100.0, 85.9, 197.3, 2},  // Sample 119
    {16.6, 3720, 121.4, 100.0, 196.0, 2}, // Sample 120
    {12.4, 3504, 107.7, 82.8, 190.1, 2},  // Sample 121
    {12.4, 4438, 105.4, 78.0, 140.0, 2},  // Sample 122
    {11.7, 4261, 121.0, 50.0, 173.9, 2},  // Sample 123
    {16.8, 4529, 114.2, 58.3, 196.4, 2},  // Sample 124
    {12.1, 4225, 122.6, 100.0, 161.8, 2}, // Sample 125
    {16.2, 4654, 110.1, 55.3, 162.9, 2},  // Sample 126
    {11.9, 4639, 112.6, 88.2, 186.5, 2},  // Sample 127
    {12.0, 3872, 105.3, 78.0, 149.7, 2},  // Sample 128
    {14.8, 4235, 137.8, 60.7, 165.8, 2},  // Sample 129
    {10.8, 5070, 106.5, 63.2, 164.8, 2},  // Sample 130
    {13.0, 4354, 139.8, 54.0, 200.1, 2},  // Sample 131
    {12.8, 4770, 140.0, 50.0, 146.9, 2},  // Sample 132
    {16.1, 4243, 134.1, 84.4, 207.9, 2},  // Sample 133
    {16.5, 4345, 104.2, 94.5, 180.0, 2},  // Sample 134
    {13.1, 4982, 131.8, 89.5, 156.5, 2},  // Sample 135
    {15.8, 4093, 128.4, 100.0, 146.8, 2}, // Sample 136
    {13.2, 4545, 120.1, 85.3, 220.0, 2},  // Sample 137
    {11.4, 3983, 135.7, 66.7, 154.9, 2},  // Sample 138
    {12.5, 4415, 100.1, 87.8, 220.0, 2},  // Sample 139
    {11.3, 3800, 111.8, 60.9, 196.6, 2},  // Sample 140
    {12.7, 4381, 126.6, 89.2, 140.0, 2},  // Sample 141
    {12.2, 4670, 129.9, 88.6, 173.5, 2},  // Sample 142
    {12.6, 4862, 113.1, 62.6, 184.0, 2},  // Sample 143
    {13.2, 4823, 100.0, 51.6, 147.2, 2},  // Sample 144
    {16.4, 3917, 101.6, 98.3, 220.0, 2},  // Sample 145
    {17.6, 3913, 119.0, 73.9, 140.0, 2},  // Sample 146
    {16.5, 3500, 103.4, 73.9, 150.1, 2},  // Sample 147
    {13.3, 3500, 111.2, 100.0, 174.8, 2}, // Sample 148
    {16.7, 4678, 116.7, 62.5, 140.0, 2},  // Sample 149
    {16.2, 4914, 110.0, 97.1, 140.0, 2}   // Sample 150

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
