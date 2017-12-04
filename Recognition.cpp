#include "Recognition.h"

Recognition::Recognition(const char *directoryPath, const char *noisyPath) {
    loadTemplates(directoryPath);
    this->Y = MatrixClass(readFromFile(noisyPath));
    std::cout << "Image to recognize: " << std::endl;
    Y.beautifulVisualization(N); std::cout << std::endl;
    this->W = MatrixClass(N * N, N * N);
    showImages();
    calculateWeights();
    recognition();
}

void Recognition::loadTemplates(const char *directoryPath) {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (directoryPath)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            std::string file = ent->d_name;
            if (file.substr(file.find_last_of('.') + 1) == TXT_EXTENSION) {
                std::string filePath = directoryPath;
                filePath += "/" + file;
                std::vector<double> tmp = readFromFile(filePath.c_str());
                MatrixClass pattern(tmp);
                templates.push_back(pattern);
            }
        }
        closedir (dir);
    } else {
        perror ("could not open directory");
        exit(EXIT_FAILURE);
    }
}

std::vector<double> Recognition::readFromFile(const char *file) {
    std::vector<double> vector;
    std::ifstream input(file);
    if (!input.is_open()) {
        std::logic_error wrongFileName("File reading error");
        exit(EXIT_FAILURE);
    } else {
        double number;
        while (input >> number) {
            vector.push_back(number);
        }
        input.close();
    }
    return vector;
}

void Recognition::calculateWeights() {
    for (MatrixClass image : templates) {
        W = W + image.transpose() * image;
    }
    W.nullifyMainDiagonal();
}

void Recognition::recognition() {
    bool didRecognize;
    int iteration = 0;
    do {
        MatrixClass Y_ = Y * W;
        Y = Y_.activationFunction();
        didRecognize = didFindAnswer(Y);
        iteration++;
        //std::cout<< "Iteration: " << iteration << std::endl;
    } while(!didRecognize);
    std::cout<< "After " << iteration << "iteration(s) recognized image is: " << std::endl;
    Y.beautifulVisualization(N);
}

bool Recognition::didFindAnswer(const MatrixClass &matrix) {
    for (const MatrixClass &image : templates) {
        if (matrix == image) {
            return true;
        }
    }
    return false;
}

void Recognition::showImages() {
    std::cout << "Images-templates are: " << std::endl;
    for (MatrixClass image : templates) {
        image.beautifulVisualization(N);
        std::cout << std::endl;
    }
}