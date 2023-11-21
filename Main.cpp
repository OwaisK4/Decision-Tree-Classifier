#include "DecisionTree.hpp"
#include <fstream>
using namespace std;

int main() {
    fstream input("trained_input.txt", ios::out);
    fstream output("output.txt", ios::out);
    //	char* trainPath = "titanicTrain.csv";
    //	char* testPath = "titanicTest.csv";
    string trainPath = "titanicTrain.csv";
    string testPath = "titanicTest.csv";
    FileReader reader;

    DecisionTree *treeP = new DecisionTree(trainPath);
    vector<int> predictions = treeP->predict(testPath);
    int predictionsSize = predictions.size();
    int size = predictionsSize;
    int *predictionsArray = new int[predictionsSize];
    for (int i = 0; i < predictions.size(); i++) {
        predictionsArray[i] = predictions[i];
    }

    DataFrame TrainData = reader.read(trainPath);
    input << "Passenger no.\tPclass\tSex\tSibSp\tParch\tEmbarked\tSurvived\n";
    for (int i = 0; i < size; i++) {
        int j;
        input << i << "\t\t";
        for (j = 0; j < TrainData.size() - 1; j++) {
            input << TrainData[j][i] << "\t";
        }
        input << "\t" << TrainData[j][i] << "\t";
        input << "\n";
    }

    DataFrame TestData = reader.read(testPath);
    output << "Passenger no.\tPclass\tSex\tSibSp\tParch\tEmbarked\tSurvived\n";
    for (int i = 0; i < size; i++) {
        output << i << "\t\t";
        for (int j = 0; j < TestData.size(); j++) {
            output << TestData[j][i] << "\t"; // Printing categorical attributes.
        }
        output << "\t" << predictionsArray[i] << "\n";
        //		cout << predictionsArray[i] << "\n";
    }
    cout << "Successfully classified test data through Decision tree.\n";
    cout << "Training data is stored in trained_input.txt and Predicted data is stored in output.txt.";

    input.close();
    output.close();
    return 0;
}

//		output << TestData[0][i] << "\t";
//		output << TestData[1][i] << "\t";
//		output << TestData[2][i] << "\t";
//		output << TestData[3][i] << "\t";
//		output << TestData[4][i] << "\t\t";
//		output << treeP->rootP->trainingData[0][i] << "\t";
//		output << treeP->rootP->trainingData[1][i] << "\t";
//		output << treeP->rootP->trainingData[2][i] << "\t";
//		output << treeP->rootP->trainingData[3][i] << "\t";
//		output << treeP->rootP->trainingData[4][i] << "\t\t";
