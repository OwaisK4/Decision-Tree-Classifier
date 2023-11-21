#ifndef NODE_HPP
#define NODE_HPP
#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <typeinfo>
#include <vector>
using namespace std;
using DataFrame = vector<vector<int>>;

class bestSplitReturn {
public:
    float resultantGini;
    int feature;
    int category;
};

class Node {
public:
    Node *leftChild;
    Node *rightChild;
    DataFrame trainingData;
    bestSplitReturn bestSplit; // column index of split feature
    Node(const DataFrame &data);
    ~Node();
    static float getGiniImpurity(const vector<int> &outcomes);
    static DataFrame getSplitTargets(const DataFrame &dataP, int feature, int category);
    static bestSplitReturn getBestSplit(const DataFrame &dataP);
};

Node::Node(const DataFrame &data) {
    this->leftChild = NULL;
    this->rightChild = NULL;
    const vector<int> outputsBefore = data[data.size() - 1]; // Survival attribute for constructing tree
    this->trainingData = data;
    // calculate best feature to split on
    this->bestSplit = Node::getBestSplit(data);
}

Node::~Node() {
    delete leftChild;
    delete rightChild;
}

float Node::getGiniImpurity(const vector<int> &outcomes) {
    // gini impurity is only calculated for the target class
    if (outcomes.size() == 0) {
        return 0.0;
    }
    float giniSum = 0.0;
    // initialize vector with length equal to number of classes with all counts equal to zero
    int max = *max_element(outcomes.begin(), outcomes.end());
    int *counts = new int[max + 1];
    memset(counts, 0, sizeof(counts));
    // count instances of each class in counts array
    for (int i : outcomes) {
        counts[i] += 1;
    }

    for (int j = 0; j <= max; j++) {
        float pClass = (float)counts[j] / (float)outcomes.size();
        giniSum += pClass * (1 - pClass);
    }
    return giniSum;
};

// split targets based on feature and category then return resultant split of outcomes
// returns a DataFrame with the first row containing target categories where feature
// category, second row contains target categories where feature category was absent
DataFrame Node::getSplitTargets(const DataFrame &dataP, int feature, int category) {
    vector<int> trueSplit;  // indexes where category present
    vector<int> falseSplit; // indexes where absent present

    int idx = 0;
    for (int i : dataP[feature]) {

        int val = dataP[feature][idx];

        if (val == category) {
            // value is category, add index to trueSplit
            trueSplit.push_back(idx);
        } else {
            // cout << "false";
            falseSplit.push_back(idx);
        };
        idx += 1;
    }
    // replace inplace indexes with values of associated outcomes
    vector<int> allOutcomes = dataP[dataP.size() - 1];
    vector<int> trueOutcomes;  // indexes where category present
    vector<int> falseOutcomes; // indexes where absent present

    for (auto it : falseSplit) {
        falseOutcomes.push_back(allOutcomes[it]);
    }

    for (auto it : trueSplit) {
        trueOutcomes.push_back(allOutcomes[it]);
    }

    DataFrame returnData{
        trueOutcomes,
        falseOutcomes};
    return returnData;
};

// return the best feature and category to split on and the resultant Gini
bestSplitReturn Node::getBestSplit(const DataFrame &dataP) {
    // giniImpurity before split
    const vector<int> outputsBefore = dataP[dataP.size() - 1];
    const float giniBefore = Node::getGiniImpurity(outputsBefore);
    int features = dataP.size() - 1;
    bestSplitReturn bestChoice; // resultant GiniImpurity, feature
    bestChoice.resultantGini = giniBefore;
    bestChoice.feature = 0;
    bestChoice.category = 0;
    for (int feature = 0; feature < features; feature++) {
        vector<int> featureVec = dataP[feature];
        int cats = *max_element(featureVec.begin(), featureVec.end()) + 1;
        // calculate Gini Impurity for splitting every feature by every category
        for (int cat = 0; cat < cats; cat++) {
            DataFrame split = Node::getSplitTargets(dataP, feature, cat);
            float trueGini = Node::getGiniImpurity(split[0]);
            float weightedTrueGini = trueGini * split[0].size() / outputsBefore.size();
            float falseGini = Node::getGiniImpurity(split[1]);
            float weightedFalseGini = falseGini * split[1].size() / outputsBefore.size();
            float weightedGiniSum = weightedTrueGini + weightedFalseGini;
            if (weightedGiniSum < bestChoice.resultantGini) {
                // splitting here improves on current gini impurity
                bestChoice.resultantGini = weightedGiniSum;
                bestChoice.feature = feature;
                bestChoice.category = cat;
            }
        }
    }
    return bestChoice;
}

#endif /* NODE_HPP */
