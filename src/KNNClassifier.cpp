#include "KNNClassifier.h"

// Static data initialization - AKAN DIISI DARI PYTHON TRAINING
// Normalization parameters (update dengan hasil training Python terbaru)
float KNNClassifier::featureMeans[5] = {13.902490, 2436.190751, 81.895197, 28.418784, 109.445007};
float KNNClassifier::featureStds[5] = {1.334927, 1071.361925, 25.958078, 31.846698, 36.056869};

// Training dataset - ARRAY KOSONG YANG AKAN DIISI DARI PYTHON
TrainingData KNNClassifier::trainingSet[200] = {
  {14.2, 3186, 100.0, 45.3, 111.3, 0},  // Sample 1, Class: Normal
  {14.2, 2755, 87.4, 50.1, 123.3, 0},  // Sample 2, Class: Normal
  {14.6, 3182, 89.5, 38.3, 122.3, 0},  // Sample 3, Class: Normal
  {14.6, 2990, 78.7, 62.1, 129.0, 0},  // Sample 4, Class: Normal
  {14.7, 3017, 90.1, 67.9, 126.1, 0},  // Sample 5, Class: Normal
  {14.5, 2939, 81.0, 32.6, 117.3, 0},  // Sample 6, Class: Normal
  {14.5, 3287, 80.1, 52.6, 113.6, 0},  // Sample 7, Class: Normal
  {14.7, 2817, 86.9, 45.2, 110.6, 0},  // Sample 8, Class: Normal
  {14.6, 3062, 90.2, 25.8, 107.0, 0},  // Sample 9, Class: Normal
  {14.5, 2949, 88.6, 62.4, 108.9, 0},  // Sample 10, Class: Normal
  {14.3, 2990, 100.0, 35.6, 141.7, 0},  // Sample 11, Class: Normal
  {14.3, 3560, 91.8, 27.1, 127.9, 0},  // Sample 12, Class: Normal
  {14.7, 2748, 85.5, 50.0, 131.7, 0},  // Sample 13, Class: Normal
  {14.4, 2531, 95.1, 43.8, 117.8, 0},  // Sample 14, Class: Normal
  {14.7, 3366, 86.3, 67.4, 121.8, 0},  // Sample 15, Class: Normal
  {14.4, 2868, 99.6, 47.9, 132.4, 0},  // Sample 16, Class: Normal
  {14.4, 3066, 90.0, 68.7, 118.9, 0},  // Sample 17, Class: Normal
  {14.7, 2440, 93.5, 20.8, 114.3, 0},  // Sample 18, Class: Normal
  {14.4, 2890, 84.9, 31.2, 139.4, 0},  // Sample 19, Class: Normal
  {14.2, 3161, 97.5, 55.8, 132.0, 0},  // Sample 20, Class: Normal
  {14.7, 2692, 89.1, 34.5, 134.3, 0},  // Sample 21, Class: Normal
  {14.6, 2400, 86.0, 34.8, 108.0, 0},  // Sample 22, Class: Normal
  {13.7, 2836, 91.1, 22.8, 122.2, 0},  // Sample 23, Class: Normal
  {14.6, 3104, 82.6, 48.5, 123.5, 0},  // Sample 24, Class: Normal
  {14.7, 2616, 100.0, 13.8, 140.4, 0},  // Sample 25, Class: Normal
  {14.6, 2862, 81.2, 57.5, 109.7, 0},  // Sample 26, Class: Normal
  {14.4, 2713, 91.4, 75.9, 107.2, 0},  // Sample 27, Class: Normal
  {13.9, 3617, 79.2, 41.7, 116.7, 0},  // Sample 28, Class: Normal
  {14.4, 3065, 95.0, 52.4, 121.8, 0},  // Sample 29, Class: Normal
  {14.5, 3013, 86.8, 59.5, 146.5, 0},  // Sample 30, Class: Normal
  {14.7, 2663, 75.7, 64.2, 124.0, 0},  // Sample 31, Class: Normal
  {14.7, 2978, 96.0, 50.4, 112.3, 0},  // Sample 32, Class: Normal
  {14.1, 3063, 75.0, 25.1, 122.4, 0},  // Sample 33, Class: Normal
  {13.8, 3687, 76.9, 20.3, 132.3, 0},  // Sample 34, Class: Normal
  {14.6, 3721, 87.5, 48.0, 132.6, 0},  // Sample 35, Class: Normal
  {14.7, 3148, 89.5, 32.1, 128.4, 0},  // Sample 36, Class: Normal
  {14.6, 3461, 87.7, 68.5, 90.0, 0},  // Sample 37, Class: Normal
  {14.1, 3262, 93.2, 43.5, 142.2, 0},  // Sample 38, Class: Normal
  {14.4, 2822, 81.1, 45.7, 110.0, 0},  // Sample 39, Class: Normal
  {14.3, 3564, 76.4, 12.0, 125.3, 0},  // Sample 40, Class: Normal
  {14.7, 2711, 93.5, 60.9, 98.9, 0},  // Sample 41, Class: Normal
  {14.7, 3051, 87.1, 40.5, 102.3, 0},  // Sample 42, Class: Normal
  {14.5, 3454, 93.0, 29.6, 142.2, 0},  // Sample 43, Class: Normal
  {14.4, 3006, 90.8, 36.9, 110.7, 0},  // Sample 44, Class: Normal
  {14.6, 3247, 100.0, 41.3, 111.0, 0},  // Sample 45, Class: Normal
  {14.1, 2449, 92.1, 28.4, 94.2, 0},  // Sample 46, Class: Normal
  {14.7, 3341, 95.6, 54.8, 116.2, 0},  // Sample 47, Class: Normal
  {14.3, 3315, 82.4, 23.9, 101.3, 0},  // Sample 48, Class: Normal
  {14.1, 2459, 92.3, 56.4, 113.1, 0},  // Sample 49, Class: Normal
  {14.4, 2400, 79.9, 16.9, 115.8, 0},  // Sample 50, Class: Normal
  {13.8, 1389, 55.3, 0.0, 95.0, 1},  // Sample 51, Class: Startup
  {14.0, 1112, 33.5, 0.0, 59.4, 1},  // Sample 52, Class: Startup
  {13.8, 965, 57.4, 0.0, 72.7, 1},  // Sample 53, Class: Startup
  {14.4, 1316, 45.7, 0.0, 81.7, 1},  // Sample 54, Class: Startup
  {13.6, 1611, 31.5, 0.0, 82.7, 1},  // Sample 55, Class: Startup
  {15.0, 1535, 25.5, 0.0, 85.0, 1},  // Sample 56, Class: Startup
  {13.2, 1054, 44.5, 0.0, 64.1, 1},  // Sample 57, Class: Startup
  {14.2, 998, 32.5, 0.0, 81.3, 1},  // Sample 58, Class: Startup
  {14.6, 1519, 54.8, 0.0, 61.3, 1},  // Sample 59, Class: Startup
  {15.0, 1226, 31.5, 0.0, 59.0, 1},  // Sample 60, Class: Startup
  {14.7, 1083, 54.6, 0.0, 72.7, 1},  // Sample 61, Class: Startup
  {14.5, 1249, 44.7, 0.0, 70.3, 1},  // Sample 62, Class: Startup
  {14.3, 1278, 65.0, 0.0, 87.9, 1},  // Sample 63, Class: Startup
  {14.6, 1353, 29.7, 0.0, 72.1, 1},  // Sample 64, Class: Startup
  {14.2, 1269, 65.0, 0.0, 59.3, 1},  // Sample 65, Class: Startup
  {14.2, 800, 44.8, 0.0, 66.7, 1},  // Sample 66, Class: Startup
  {13.8, 1386, 42.1, 0.0, 74.2, 1},  // Sample 67, Class: Startup
  {13.9, 1124, 24.8, 0.0, 65.2, 1},  // Sample 68, Class: Startup
  {12.8, 1075, 37.1, 0.0, 91.7, 1},  // Sample 69, Class: Startup
  {14.2, 1426, 31.4, 0.0, 93.2, 1},  // Sample 70, Class: Startup
  {14.1, 1312, 21.4, 0.0, 61.5, 1},  // Sample 71, Class: Startup
  {14.5, 1117, 62.6, 0.0, 79.4, 1},  // Sample 72, Class: Startup
  {15.2, 1606, 65.0, 0.0, 73.1, 1},  // Sample 73, Class: Startup
  {14.0, 868, 38.8, 0.0, 78.1, 1},  // Sample 74, Class: Startup
  {14.6, 1254, 52.6, 0.0, 95.0, 1},  // Sample 75, Class: Startup
  {14.0, 1196, 53.2, 0.0, 75.1, 1},  // Sample 76, Class: Startup
  {13.6, 1430, 60.0, 0.0, 67.0, 1},  // Sample 77, Class: Startup
  {12.8, 1246, 65.0, 0.0, 89.9, 1},  // Sample 78, Class: Startup
  {13.8, 1098, 30.1, 0.0, 82.8, 1},  // Sample 79, Class: Startup
  {13.7, 949, 48.1, 0.0, 75.8, 1},  // Sample 80, Class: Startup
  {15.2, 917, 52.1, 0.0, 63.9, 1},  // Sample 81, Class: Startup
  {14.5, 999, 58.4, 0.0, 67.4, 1},  // Sample 82, Class: Startup
  {15.2, 970, 20.0, 0.0, 71.6, 1},  // Sample 83, Class: Startup
  {14.4, 1073, 44.6, 0.0, 55.0, 1},  // Sample 84, Class: Startup
  {13.1, 1474, 37.8, 0.0, 64.6, 1},  // Sample 85, Class: Startup
  {13.2, 1387, 28.9, 0.0, 77.9, 1},  // Sample 86, Class: Startup
  {14.5, 970, 38.7, 0.0, 71.5, 1},  // Sample 87, Class: Startup
  {14.1, 1411, 44.3, 0.0, 58.1, 1},  // Sample 88, Class: Startup
  {15.4, 1396, 44.7, 0.0, 71.8, 1},  // Sample 89, Class: Startup
  {14.6, 1095, 20.0, 0.0, 77.4, 1},  // Sample 90, Class: Startup
  {14.1, 1600, 53.4, 0.0, 71.5, 1},  // Sample 91, Class: Startup
  {13.9, 1530, 65.0, 0.0, 67.8, 1},  // Sample 92, Class: Startup
  {14.5, 1458, 35.1, 0.0, 85.3, 1},  // Sample 93, Class: Startup
  {13.7, 994, 65.0, 0.0, 89.9, 1},  // Sample 94, Class: Startup
  {14.5, 980, 65.0, 0.0, 68.6, 1},  // Sample 95, Class: Startup
  {14.2, 962, 38.9, 0.0, 83.2, 1},  // Sample 96, Class: Startup
  {12.9, 1201, 49.9, 0.0, 86.1, 1},  // Sample 97, Class: Startup
  {14.4, 1297, 44.2, 0.0, 89.7, 1},  // Sample 98, Class: Startup
  {14.7, 1722, 21.2, 0.0, 55.0, 1},  // Sample 99, Class: Startup
  {14.0, 1347, 21.0, 0.0, 80.5, 1},  // Sample 100, Class: Startup
  {15.0, 1647, 79.5, 0.0, 76.2, 2},  // Sample 101, Class: Maintenance
  {14.9, 2191, 93.9, 0.0, 78.8, 2},  // Sample 102, Class: Maintenance
  {14.6, 1200, 84.4, 0.0, 84.0, 2},  // Sample 103, Class: Maintenance
  {14.4, 1745, 78.1, 0.0, 84.5, 2},  // Sample 104, Class: Maintenance
  {14.7, 1926, 79.4, 0.0, 104.2, 2},  // Sample 105, Class: Maintenance
  {15.8, 1423, 95.3, 0.0, 93.9, 2},  // Sample 106, Class: Maintenance
  {13.6, 1650, 97.8, 0.0, 92.7, 2},  // Sample 107, Class: Maintenance
  {15.5, 2028, 88.6, 0.0, 90.8, 2},  // Sample 108, Class: Maintenance
  {14.2, 1927, 100.8, 0.0, 60.0, 2},  // Sample 109, Class: Maintenance
  {15.4, 1966, 90.3, 0.0, 81.3, 2},  // Sample 110, Class: Maintenance
  {15.8, 1901, 75.9, 0.0, 68.9, 2},  // Sample 111, Class: Maintenance
  {14.3, 2152, 76.2, 0.0, 92.3, 2},  // Sample 112, Class: Maintenance
  {14.5, 1911, 86.8, 0.0, 91.9, 2},  // Sample 113, Class: Maintenance
  {15.9, 2292, 105.0, 0.0, 68.1, 2},  // Sample 114, Class: Maintenance
  {16.1, 2257, 95.4, 0.0, 110.0, 2},  // Sample 115, Class: Maintenance
  {15.8, 1468, 84.3, 0.0, 82.7, 2},  // Sample 116, Class: Maintenance
  {13.7, 1905, 75.0, 0.0, 85.7, 2},  // Sample 117, Class: Maintenance
  {14.9, 1616, 89.4, 0.0, 108.8, 2},  // Sample 118, Class: Maintenance
  {13.9, 1852, 94.7, 0.0, 107.9, 2},  // Sample 119, Class: Maintenance
  {14.8, 1901, 96.9, 0.0, 85.1, 2},  // Sample 120, Class: Maintenance
  {13.9, 1200, 79.9, 0.0, 99.4, 2},  // Sample 121, Class: Maintenance
  {14.1, 1829, 75.4, 0.0, 60.0, 2},  // Sample 122, Class: Maintenance
  {15.3, 1634, 98.0, 0.0, 77.5, 2},  // Sample 123, Class: Maintenance
  {15.3, 1579, 75.0, 0.0, 88.3, 2},  // Sample 124, Class: Maintenance
  {16.3, 1568, 75.0, 0.0, 77.9, 2},  // Sample 125, Class: Maintenance
  {15.2, 1552, 89.5, 0.0, 90.5, 2},  // Sample 126, Class: Maintenance
  {16.5, 2492, 86.2, 0.0, 83.4, 2},  // Sample 127, Class: Maintenance
  {15.8, 1661, 83.9, 0.0, 102.3, 2},  // Sample 128, Class: Maintenance
  {16.0, 1935, 97.6, 0.0, 81.4, 2},  // Sample 129, Class: Maintenance
  {14.7, 1531, 96.4, 0.0, 79.5, 2},  // Sample 130, Class: Maintenance
  {15.3, 1837, 86.9, 0.0, 65.5, 2},  // Sample 131, Class: Maintenance
  {14.9, 2099, 75.0, 0.0, 87.4, 2},  // Sample 132, Class: Maintenance
  {13.0, 1688, 77.6, 0.0, 60.5, 2},  // Sample 133, Class: Maintenance
  {15.2, 1771, 105.0, 0.0, 98.3, 2},  // Sample 134, Class: Maintenance
  {13.6, 1200, 91.9, 0.0, 83.6, 2},  // Sample 135, Class: Maintenance
  {15.0, 1471, 79.2, 0.0, 110.0, 2},  // Sample 136, Class: Maintenance
  {14.0, 1610, 89.9, 0.0, 89.5, 2},  // Sample 137, Class: Maintenance
  {14.2, 2386, 82.7, 0.0, 87.7, 2},  // Sample 138, Class: Maintenance
  {14.5, 1676, 83.2, 0.0, 66.2, 2},  // Sample 139, Class: Maintenance
  {14.8, 1505, 99.2, 0.0, 110.0, 2},  // Sample 140, Class: Maintenance
  {16.3, 1516, 79.7, 0.0, 96.5, 2},  // Sample 141, Class: Maintenance
  {13.8, 2116, 93.7, 0.0, 74.4, 2},  // Sample 142, Class: Maintenance
  {14.2, 2057, 104.5, 0.0, 101.1, 2},  // Sample 143, Class: Maintenance
  {16.1, 1703, 101.3, 0.0, 63.9, 2},  // Sample 144, Class: Maintenance
  {14.4, 1761, 104.7, 0.0, 70.9, 2},  // Sample 145, Class: Maintenance
  {13.3, 1485, 103.0, 0.0, 110.0, 2},  // Sample 146, Class: Maintenance
  {15.0, 1200, 93.5, 0.0, 76.8, 2},  // Sample 147, Class: Maintenance
  {14.0, 2289, 85.2, 0.0, 108.9, 2},  // Sample 148, Class: Maintenance
  {13.4, 2248, 88.4, 0.0, 91.6, 2},  // Sample 149, Class: Maintenance
  {13.5, 1643, 80.6, 0.0, 95.8, 2},  // Sample 150, Class: Maintenance
  {11.5, 3267, 116.3, 74.2, 130.0, 3},  // Sample 151, Class: Critical
  {11.6, 4081, 108.2, 53.3, 173.9, 3},  // Sample 152, Class: Critical
  {11.4, 4148, 98.0, 100.0, 157.9, 3},  // Sample 153, Class: Critical
  {11.9, 3925, 106.8, 89.9, 173.1, 3},  // Sample 154, Class: Critical
  {11.1, 4154, 127.9, 59.6, 152.6, 3},  // Sample 155, Class: Critical
  {11.4, 3728, 130.0, 57.3, 184.3, 3},  // Sample 156, Class: Critical
  {11.0, 4648, 124.1, 84.7, 191.8, 3},  // Sample 157, Class: Critical
  {11.6, 4076, 103.0, 55.1, 130.0, 3},  // Sample 158, Class: Critical
  {11.5, 3479, 111.5, 97.1, 158.6, 3},  // Sample 159, Class: Critical
  {11.8, 3200, 111.9, 63.9, 174.6, 3},  // Sample 160, Class: Critical
  {11.3, 4426, 110.3, 72.2, 154.4, 3},  // Sample 161, Class: Critical
  {11.8, 4654, 128.0, 53.2, 184.9, 3},  // Sample 162, Class: Critical
  {11.1, 3540, 111.0, 95.6, 177.5, 3},  // Sample 163, Class: Critical
  {11.2, 4015, 95.0, 78.3, 200.0, 3},  // Sample 164, Class: Critical
  {11.7, 4305, 107.2, 79.6, 187.5, 3},  // Sample 165, Class: Critical
  {11.5, 4213, 130.0, 96.5, 151.3, 3},  // Sample 166, Class: Critical
  {11.7, 4366, 95.0, 88.5, 138.0, 3},  // Sample 167, Class: Critical
  {11.5, 3771, 99.8, 60.1, 148.0, 3},  // Sample 168, Class: Critical
  {11.9, 3640, 95.0, 91.4, 175.6, 3},  // Sample 169, Class: Critical
  {11.6, 5031, 95.0, 76.9, 156.9, 3},  // Sample 170, Class: Critical
  {11.7, 3700, 116.6, 76.6, 191.3, 3},  // Sample 171, Class: Critical
  {11.4, 3660, 122.5, 79.2, 140.4, 3},  // Sample 172, Class: Critical
  {11.6, 3200, 95.0, 96.4, 145.7, 3},  // Sample 173, Class: Critical
  {11.8, 3306, 95.0, 44.0, 181.6, 3},  // Sample 174, Class: Critical
  {11.7, 3405, 123.8, 59.8, 155.6, 3},  // Sample 175, Class: Critical
  {11.3, 5200, 119.8, 57.4, 200.0, 3},  // Sample 176, Class: Critical
  {11.0, 4448, 130.0, 100.0, 178.0, 3},  // Sample 177, Class: Critical
  {11.4, 3771, 118.2, 60.0, 133.8, 3},  // Sample 178, Class: Critical
  {11.1, 3200, 106.8, 40.0, 130.0, 3},  // Sample 179, Class: Critical
  {11.9, 3964, 117.4, 80.8, 130.0, 3},  // Sample 180, Class: Critical
  {11.8, 3761, 128.6, 72.5, 179.7, 3},  // Sample 181, Class: Critical
  {11.6, 3200, 98.8, 84.2, 172.2, 3},  // Sample 182, Class: Critical
  {11.4, 3997, 100.1, 69.9, 140.8, 3},  // Sample 183, Class: Critical
  {11.5, 4725, 109.5, 68.2, 194.2, 3},  // Sample 184, Class: Critical
  {11.5, 5200, 99.7, 100.0, 174.6, 3},  // Sample 185, Class: Critical
  {11.2, 3200, 130.0, 72.7, 156.4, 3},  // Sample 186, Class: Critical
  {11.6, 4211, 96.9, 64.2, 163.2, 3},  // Sample 187, Class: Critical
  {11.8, 3317, 115.5, 88.1, 187.1, 3},  // Sample 188, Class: Critical
  {12.0, 3997, 127.7, 47.5, 178.0, 3},  // Sample 189, Class: Critical
  {11.5, 3668, 129.6, 40.0, 141.2, 3},  // Sample 190, Class: Critical
  {11.1, 4723, 112.7, 87.5, 158.1, 3},  // Sample 191, Class: Critical
  {11.8, 5045, 130.0, 40.3, 170.5, 3},  // Sample 192, Class: Critical
  {11.1, 3872, 127.7, 86.9, 130.0, 3},  // Sample 193, Class: Critical
  {11.1, 3200, 106.7, 71.1, 178.5, 3},  // Sample 194, Class: Critical
  {11.4, 4161, 97.6, 64.6, 198.1, 3},  // Sample 195, Class: Critical
  {11.4, 3925, 110.2, 79.4, 153.8, 3},  // Sample 196, Class: Critical
  {11.1, 4597, 117.9, 65.7, 130.0, 3},  // Sample 197, Class: Critical
  {11.5, 3280, 120.5, 66.8, 184.6, 3},  // Sample 198, Class: Critical
  {11.4, 4466, 109.2, 47.2, 191.0, 3},  // Sample 199, Class: Critical
  {11.2, 3806, 95.0, 79.0, 175.0, 3}  // Sample 200, Class: Critical
};


KNNClassifier::KNNClassifier() {
    // Constructor
}

KNNClassifier::~KNNClassifier() {
    // Destructor
}

void KNNClassifier::initialize() {
    Serial.println("=== KNN 4-Class Classifier Initialized ===");
    Serial.printf("Training samples: %d\n", 200);
    Serial.printf("K-value: %d\n", 3);
    Serial.println("Features: AFR, RPM, temp,, TPS, MAP");
    Serial.println();
    Serial.println("Classification System (4 Classes):");
    Serial.println("  Class 0: Normal Operation");
    Serial.println("           - Warm engine (temp >= 75°C)");
    Serial.println("           - TPS > 0% (throttle active)");
    Serial.println("           - Standard operation parameters");
    Serial.println();
    Serial.println("  Class 1: Normal Startup");
    Serial.println("           - Cold engine (temp < 65°C)");
    Serial.println("           - TPS = 0% (normal at startup)");
    Serial.println("           - Engine warming up");
    Serial.println();
    Serial.println("  Class 2: Maintenance Required");
    Serial.println("           - Warm engine (temp >= 75°C)");
    Serial.println("           - TPS = 0% (throttle stuck/issue)");
    Serial.println("           - Requires immediate attention");
    Serial.println();
    Serial.println("  Class 3: Critical Condition");
    Serial.println("           - AFR 11.0-12.0 (very rich mixture)");
    Serial.println("           - Risk of engine damage");
    Serial.println("           - Emergency condition");
    Serial.println();
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
    // Sort to find K=3 nearest neighbors using selection sort
    for (int i = 0; i < 3; i++) {
        int minIdx = i;
        for (int j = i + 1; j < size; j++) {
            if (distances[j].distance < distances[minIdx].distance) {
                minIdx = j;
            }
        }
        // Swap
        Distance temp = distances[i];
        distances[i] = distances[minIdx];
        distances[minIdx] = temp;
    }
}

int KNNClassifier::runKNNClassification(float afr, float rpm, float temp, float tps, float mapValue) {
    float features[5] = {afr, rpm, temp, tps, mapValue};
    normalizeFeatures(features);

    Distance distances[200];

    // Calculate distances to all training samples
    for (int i = 0; i < 200; i++) {
        float trainFeatures[5] = {
            trainingSet[i].afr, 
            trainingSet[i].rpm, 
            trainingSet[i].temp,
            trainingSet[i].tps, 
            trainingSet[i].map_value
        };
        normalizeFeatures(trainFeatures);

        distances[i].distance = calculateEuclideanDistance(features, trainFeatures);
        distances[i].classification = trainingSet[i].classification;
    }

    findKNearest(distances, 200);

    // Vote among K=3 nearest neighbors
    int votes[4] = {0, 0, 0, 0};  // 4 classes
    for (int i = 0; i < 3; i++) {
        if (distances[i].classification >= 0 && distances[i].classification <= 3) {
            votes[distances[i].classification]++;
        }
    }

    // Find class with most votes
    int maxVotes = 0;
    int result = 0;
    for (int i = 0; i < 4; i++) {
        if (votes[i] > maxVotes) {
            maxVotes = votes[i];
            result = i;
        }
    }

    // Handle tie by using closest neighbor
    if (maxVotes == 1) {
        result = distances[0].classification;
    }

    return result;
}

// HYBRID CLASSIFICATION - Main classification function with rule-based preprocessing
int KNNClassifier::classifyEngineCondition(float afr, float rpm, float temp, float tps, float mapValue) {
    // Rule 1: TPS = 0 context-aware classification
    if (tps == 0.0 || tps < 1.0) {  // Allow small tolerance for sensor noise
        if (temp < 65.0) {
            return 1;  // Normal Startup (cold engine)
        } else if (temp >= 75.0) {
            return 2;  // Maintenance Required (warm engine, throttle stuck)
        }
        // For borderline temp,s (65-75°C), use KNN
    }
    
    // Rule 2: Critical AFR detection
    if (afr >= 11.0 && afr <= 12.0) {
        return 3;  // Critical Condition (very rich mixture)
    }
    
    // Rule 3: Use KNN for normal operation and borderline cases
    return runKNNClassification(afr, rpm, temp, tps, mapValue);
}

// Main classify function using hybrid approach
int KNNClassifier::classify(const SensorData& data) {
    return classifyEngineCondition(data.afr, data.rpm, data.temp, data.tps, data.map_value);
}

String KNNClassifier::getClassificationText(int classification) {
    switch (classification) {
        case 0: return "Normal Operation";
        case 1: return "Normal Startup";
        case 2: return "Maintenance Required";
        case 3: return "Critical Condition";
        default: return "Unknown";
    }
}

uint16_t KNNClassifier::getClassificationColor(int classification) {
    switch (classification) {
        case 0: return ST77XX_GREEN;    // Normal Operation
        case 1: return ST77XX_BLUE;     // Normal Startup
        case 2: return ST77XX_YELLOW;   // Maintenance Required
        case 3: return ST77XX_RED;      // Critical Condition
        default: return ST77XX_WHITE;
    }
}

void KNNClassifier::logClassificationResult(const SensorData& data, int classification, unsigned long executionTime) {
    Serial.printf("KNN_AI: %s (AFR:%.1f RPM:%.0f TEMP:%.1f TPS:%.1f MAP:%.1f) - %lums\n",
                  getClassificationText(classification).c_str(),
                  data.afr, data.rpm, data.temp, data.tps, data.map_value,
                  executionTime);
    
    // Enhanced status logging based on classification
    switch(classification) {
        case 0:
            Serial.println("STATUS: All systems normal - Engine operating properly");
            break;
            
        case 1:
            Serial.println("STATUS: Engine warming up - Normal startup detected");
            Serial.printf("INFO: Cold engine (%.1f°C), TPS=%.1f%% - Allow warm-up time\n", 
                         data.temp, data.tps);
            break;
            
        case 2:
            Serial.println("STATUS: MAINTENANCE REQUIRED - Throttle issue detected!");
            Serial.printf("ALERT: Warm engine (%.1f°C) but TPS=%.1f%% - Check throttle system\n", 
                         data.temp, data.tps);
            Serial.println("ACTION: Inspect throttle body, idle air control, and TPS sensor");
            break;
            
        case 3:
            Serial.println("STATUS: CRITICAL CONDITION - Very rich mixture!");
            Serial.printf("CRITICAL: AFR=%.1f (very rich) - IMMEDIATE ATTENTION REQUIRED\n", 
                         data.afr);
            Serial.println("ACTION: Check fuel injectors, AFR sensor, and fuel pressure regulator");
            break;
            
        default:
            Serial.println("STATUS: Unknown classification - Check sensor readings");
            break;
    }
}

void KNNClassifier::printTrainingDataSample() {
    Serial.println("=== Training Data Sample (First 5 entries) ===");
    for (int i = 0; i < 5 && i < 200; i++) {
        Serial.printf("Sample %d: AFR=%.1f RPM=%.0f TEMP=%.1f TPS=%.1f MAP=%.1f Class=%d\n",
                     i+1, trainingSet[i].afr, trainingSet[i].rpm, trainingSet[i].temp,
                     trainingSet[i].tps, trainingSet[i].map_value, trainingSet[i].classification);
    }
    Serial.println();
}

void KNNClassifier::printNormalizationParams() {
    Serial.println("=== Normalization Parameters ===");
    const char* featureNames[] = {"AFR", "RPM", "temp,", "TPS", "MAP"};
    
    Serial.println("Feature Means:");
    for (int i = 0; i < 5; i++) {
        Serial.printf("  %s: %.6f\n", featureNames[i], featureMeans[i]);
    }
    
    Serial.println("Feature Standard Deviations:");
    for (int i = 0; i < 5; i++) {
        Serial.printf("  %s: %.6f\n", featureNames[i], featureStds[i]);
    }
    Serial.println();
}
