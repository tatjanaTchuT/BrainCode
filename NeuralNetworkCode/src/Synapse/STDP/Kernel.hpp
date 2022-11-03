//
// Created by saiftyfirst on 09.12.21.
//

#ifndef NEURALNETWORK_KERNEL_H
#define NEURALNETWORK_KERNEL_H


struct Kernel {

    Kernel() = default;
    virtual double responseLTP(double) = 0;
    virtual double responseLTD(double) = 0;
    virtual std::string getIdentifier() = 0;
    virtual void save(std::ofstream *stream, std::string id_str) = 0;

};

struct ExponentialKernel: public Kernel {
    double tauLTP, tauLTD;

    ExponentialKernel() = delete;
    ExponentialKernel(ExponentialKernel&) = delete;
    ExponentialKernel(double tauLTP, double tauLTD): Kernel() {
        this->tauLTP = tauLTP;
        this->tauLTD = tauLTD;
    }

    double responseLTP(double deltaT) override {
        return deltaT < 0.0 ? 0.0 : std::exp(-deltaT / this->tauLTP);
    }

    double responseLTD(double deltaT) override {
        return deltaT >= 0.0 ? 0.0 : -std::exp(deltaT / this->tauLTD);
    }

    std::string getIdentifier() override {
        return str_ExponentialSTDPKernel;
    }

    void save(std::ofstream *stream, std::string id_str) override {
        *stream << id_str << "kernel\t" << getIdentifier() << "\n";
        *stream << id_str << "tau_LTP\t" << this->tauLTP << "\ts\n";
        *stream << id_str << "tau_LTD\t" << this->tauLTD << "\ts\n";
    }

};

struct GaussianKernel: public Kernel {
    double muLTP, sigmaLTP, muLTD, sigmaLTD;

    GaussianKernel() = delete;
    GaussianKernel(GaussianKernel&) = delete;
    GaussianKernel(double muLTP, double sigmaLTP, double muLTD, double sigmaLTD): Kernel() {
        this->muLTP = muLTP;
        this->sigmaLTP = sigmaLTP;
        this->muLTD = muLTD;
        this->sigmaLTD = sigmaLTD;
    }

    double responseLTP(double deltaT) override {
        if (deltaT < 0.0) {
            return 0.0;
        }
        double denominator = 2 * this->sigmaLTP * this->sigmaLTP;
        double numerator = this->muLTP - deltaT;
        numerator *= numerator;
        return exp(-numerator / denominator);
    }

    double responseLTD(double deltaT) override {
        if (deltaT >= 0.0) {
            return 0.0;
        }
        double denominator = 2 * this->sigmaLTD * this->sigmaLTD;
        double numerator = -this->muLTD - deltaT;
        numerator *= numerator;
        return -exp(-numerator / denominator);
    }

    std::string getIdentifier() override {
        return str_ExponentialSTDPKernel;
    }

    void save(std::ofstream *stream, std::string id_str) override {
        *stream << id_str << "kernel\t" << getIdentifier() << "\n";
        *stream << id_str << "sigma_LTP\t" << this->sigmaLTP << "\ts\n";
        *stream << id_str << "mu_LTP\t" << this->muLTP << "\ts\n";
        *stream << id_str << "sigma_LTD\t" << this->sigmaLTD << "\ts\n";
        *stream << id_str << "mu_LTD\t" << this->muLTD << "\ts\n";
    }

};

#endif //NEURALNETWORK_KERNEL_H
