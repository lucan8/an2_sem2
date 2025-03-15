#include <vector>
#include <iostream>
#include <random>
#include <algorithm>


std::vector<bool> toBits(int n, int nr_bits){
    std::vector<bool> result;
    result.reserve(10);

    while (n){
        result.push_back(n % 2);
        n = n >> 1;
    }

    while (result.size() != nr_bits)
        result.push_back(0);

    std::reverse(result.begin(), result.end());

    return result;
}

int fromBits(const std::vector<bool>& bits){
    int res = 0;
    for (int i = 0; i < bits.size(); ++i)
        res += bits[i] << bits.size() - i - 1;
    return res;
}

class Function{
private:
    std::vector<double> func_coef;
    std::pair<double, double> func_domain;

    double interval_step;
public:
    double fitness(double value) const;

    const std::pair<double, double>& getFuncDomain() const {return func_domain;}
    double getDomainLength() const {return func_domain.second - func_domain.first;}

    size_t getIntervalIndex(double value) const {
        return (value - func_domain.first) / interval_step;
    }

    double getIntervalStep() const {return interval_step;}
    void setIntervalStep(size_t nr_bits){ this->interval_step = getDomainLength() / (1 << nr_bits);}
    
    // Returns the interval value associated with the bit representation
    double fromBits(const std::vector<bool>& bits) const;

    friend std::istream& operator>>(std::istream& in, Function& func);
};

class Chromosome{
private:
    std::vector<bool> bits;
    double value;

    std::ostream& printBits(std::ostream& out) const;
public:
    Chromosome(){}
    Chromosome(double value, const std::vector<bool>& bits): value(value), bits(bits){}

    double getValue() const {return value;}
    void mutation();
    
    // This changes both chromosomes
    friend void crossover(Chromosome& chromo1, Chromosome& chromo2);

    friend std::ostream& operator<<(std::ostream& out, const Chromosome& chromo);
};

class GeneticAlgo{
private:
    std::vector<Chromosome> chromosomes;
    Function func;
    size_t precision;

    double crossover_prob;
    double mutation_prob;

    size_t stage_count;
    size_t curr_stage;

    // Should be used only once
    void initializePopulation(size_t population_size, size_t nr_bits);


public:
    void naturalSelection();
    void crossover();
    void mutation();

    friend std::ostream& operator<<(std::ostream& out, const GeneticAlgo& g_algo);
    friend std::istream& operator>>(std::istream& in, GeneticAlgo& g_algo);
};

double Function::fitness(double x) const{
    double result = 0;
    for (double coef : func_coef)
        result += coef * x;
    return result;
}

void GeneticAlgo::initializePopulation(size_t population_size, size_t nr_bits){
    std::random_device rand_dev;
    std::mt19937 gen(rand_dev());
    std::uniform_real_distribution dist(func.getFuncDomain().first, func.getFuncDomain().second);

    chromosomes.resize(population_size);
    for (auto& chromo : chromosomes){
        double chromo_value = dist(gen);
        chromo = Chromosome(chromo_value, toBits(func.getIntervalIndex(chromo_value), nr_bits));
    }
}

// Returns the interval value associated with the bit representation
double Function::fromBits(const std::vector<bool>& bits) const{
    double interval_index = ::fromBits(bits);
    return func_domain.first + interval_index * interval_step;
}

std::ostream& operator<<(std::ostream& out, const GeneticAlgo& g_algo){
    for (int i = 0; i < g_algo.chromosomes.size(); ++i)
        out << i << ": " << g_algo.chromosomes[i] 
            << " f(value): " << g_algo.func.fitness(g_algo.chromosomes[i].getValue()) << '\n';
}

std::ostream& operator<<(std::ostream& out, const Chromosome& chromo){
    chromo.printBits(out) << " value: " << chromo.value;
}

std::ostream& Chromosome::printBits(std::ostream& out) const{
    for (auto b : bits)
        out << b;
    return out; 
}

std::istream& operator>>(std::istream& in, Function& func){
    // First line: grade of the function
    int func_grade;
    in >> func_grade;

    func.func_coef.resize(func_grade);
    for (int i = 0; i < func_grade; ++i)
        in >> func.func_coef[i];
    
    // Next line: function domain
    in >> func.func_domain.first >> func.func_domain.second;

    return in;
}

std::istream& operator>>(std::istream& in, GeneticAlgo& g_algo){
    in >> g_algo.func;
    in >> g_algo.precision;

    int population_size;
    in >> population_size;
    
    int nr_bits = log2(g_algo.func.getDomainLength() * pow(10, g_algo.precision)) + 1;
    g_algo.initializePopulation(population_size, nr_bits);

    in >> g_algo.crossover_prob >> g_algo.mutation_prob;
    in >> g_algo.stage_count;
}