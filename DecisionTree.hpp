#ifndef DECISIONTREE_HPP
#define DECISIONTREE_HPP
#include "CSVReader.hpp"
#include "Node.hpp"
using namespace std;

class TwoDataFrame {
public:
    DataFrame pres;
    DataFrame abs;
};

class DecisionTree {
public:
    Node *rootP;
    DecisionTree(string trainPath);
    ~DecisionTree();

    void traverseTree() const;
    static TwoDataFrame splitData(const DataFrame &dataBefore, int feature, int category);
    static void constructTree(Node *nodeP);
    vector<int> predict(string path);
    void deleteChildren(Node *nodeP);
    static int recursivelyPredict(Node *nodeP, vector<int> observations);
};

DecisionTree::DecisionTree(string trainPath) {
    FileReader reader;
    DataFrame trainingData = reader.read(trainPath);
    Node *rootP = new Node(trainingData);
    constructTree(rootP);
    this->rootP = rootP;
};

// construct DataFrame for children following split
// return two dataframes, one with split cat, one without
TwoDataFrame DecisionTree::splitData(const DataFrame &dataBefore, int feature, int category) {
    vector<int> presSplit; // indexes where category present
    vector<int> absSplit;  // indexes where category absent

    for (int idx = 0; idx < dataBefore[feature].size(); idx++) {
        int val = dataBefore[feature][idx];
        if (val == category) {
            // value is category, add index to trueSplit
            presSplit.push_back(idx);
        } else {
            absSplit.push_back(idx);
        };
    }

    int numRows = dataBefore.size();
    DataFrame presData;
    vector<int> rowBuffer;
    for (int row = 0; row < numRows; row++) {
        int i = 0;
        for (int col : presSplit) {

            rowBuffer.push_back(dataBefore[row][col]);
            i += 1;
        }
        presData.push_back(rowBuffer);
        rowBuffer.clear();
    }

    DataFrame absData;
    for (int row = 0; row < numRows; row++) {
        int i = 0;
        for (int col : absSplit) {
            rowBuffer.push_back(dataBefore[row][col]);
            i += 1;
        }
        absData.push_back(rowBuffer);
        rowBuffer.clear();
    }

    TwoDataFrame returnObj;
    returnObj.pres = presData;
    returnObj.abs = absData;
    return returnObj;
}

void DecisionTree::constructTree(Node *nodeP) {
    const vector<int> outputsBefore = (nodeP->trainingData)[(nodeP->trainingData).size() - 1];
    // if resultant gini from best split less than current gini recurse on children
    float giniBefore = Node::getGiniImpurity(outputsBefore);
    if (nodeP->bestSplit.resultantGini < giniBefore) {
        // recurse on children
        TwoDataFrame childrensData = DecisionTree::splitData(nodeP->trainingData, nodeP->bestSplit.feature, nodeP->bestSplit.category);
        // if data present in split data then build a child node
        if (childrensData.pres[0].size() > 0) {
            Node *newRightChild = new Node(childrensData.pres);
            nodeP->rightChild = newRightChild;
            DecisionTree::constructTree(nodeP->rightChild);
        }
        if (childrensData.abs[0].size() > 0) {
            Node *newLeftChild = new Node(childrensData.abs);
            nodeP->leftChild = newLeftChild;
            DecisionTree::constructTree(nodeP->leftChild);
        }
    };
}

void DecisionTree::deleteChildren(Node *nodeP) {
    if (nodeP->rightChild != NULL) {
        DecisionTree::deleteChildren(nodeP->rightChild);
    }
    if (nodeP->leftChild != NULL) {
        DecisionTree::deleteChildren(nodeP->leftChild);
    }
    delete nodeP;
};

// traverse tree with observations until reach leaf and return prediction
int DecisionTree::recursivelyPredict(Node *nodeP, vector<int> observations) {
    int obsCat = observations[nodeP->bestSplit.feature];
    // initialize predictClass
    int predictClass = -1;

    if ((nodeP->leftChild == NULL) && (nodeP->rightChild == NULL)) {
        // leaf node
        // return outcome category of all data
        predictClass = (nodeP->trainingData)[(nodeP->trainingData).size() - 1][0];
    } else if (obsCat == nodeP->bestSplit.category) {
        predictClass = DecisionTree::recursivelyPredict(nodeP->rightChild, observations);
    } else {
        predictClass = DecisionTree::recursivelyPredict(nodeP->leftChild, observations);
    }
    return predictClass;
}

vector<int> DecisionTree::predict(string path) {
    FileReader reader;
    DataFrame testData = reader.read(path);
    vector<int> predictions;
    int numObs = testData[0].size();
    int numFeatures = testData.size();
    for (int obs = 0; obs < numObs; obs++) {
        vector<int> observations;
        for (int feature = 0; feature < numFeatures; feature++) {
            observations.push_back(testData[feature][obs]);
        }
        predictions.push_back(DecisionTree::recursivelyPredict(this->rootP, observations));
    }
    return predictions;
};

DecisionTree::~DecisionTree() {
    DecisionTree::deleteChildren(rootP);
};

#endif /* DECISIONTREE_HPP */
