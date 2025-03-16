#include <vector>
#include <iostream>
#include <random>
#include <algorithm>
#include <fstream>
#include <cassert>

// TODO: Make a vector printing function
// TODO: Encapsulate in a class
std::random_device rand_dev;
std::mt19937 gen(rand_dev());

int intervalBinSearch(double value, const std::vector<double>& partial_sums){
    if (value < partial_sums.front() || value > partial_sums.back())
        throw std::invalid_argument("[ERROR]: intervalBinSeach: Argument value = " + std::to_string(value) + " should be between " + std::to_string(partial_sums.front()) + ", " + std::to_string(partial_sums.back()));
    
    if (partial_sums.size() < 2)
        throw std::invalid_argument("[ERROR]: intervalBinSearch: Argument partial_sums must have at least 2 elements");

    int i = 0, j = partial_sums.size() - 1;

    while (i < j){
        int mid = (i + j) / 2;
        if (value <= partial_sums[mid])
            if (value >= partial_sums[mid - 1])
                return mid - 1;
            else
                j = mid - 1;
        else if (value <= partial_sums[mid + 1])
                return mid;
            else
                i = mid + 1;
    }

    return -1;
}

std::vector<bool> toBits(int n, int nr_bits);

int fromBits(const std::vector<bool>& bits);

class Function{
private:
    std::vector<double> func_coef;
    std::pair<double, double> func_domain;

    double interval_step;
public:
    Function(){}
    Function(const std::vector<double>& func_coef, double a, double b, double step)
        : func_coef(func_coef), func_domain(a, b), interval_step(step){}
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
    std::ifstream fin;
    std::ofstream fout;

    std::vector<Chromosome> chromosomes;
    Function func;
    size_t precision;

    double crossover_prob;
    double mutation_prob;

    size_t stage_count;
    size_t curr_stage = 0;

    // Should be used only once
    void initializePopulation(size_t population_size, size_t nr_bits);

    std::vector<double> getSelPartialSums();
    // std::vector<double> getSelProb() const;
public:
    GeneticAlgo(const std::string& in_file_name, const std::string& out_file_name);
    GeneticAlgo(const std::string& out_file_name, std::vector<Chromosome>& chromosomes, Function func, size_t precision, double crossover_prob, double mutation_prob, size_t stage_count)
        : fout(out_file_name), chromosomes(chromosomes), func(func), precision(precision), crossover_prob(crossover_prob), mutation_prob(mutation_prob), stage_count(stage_count){}

    // Determines and sets the new generation of chromosomes
    void naturalSelection();

    void crossover();
    void mutation();

    friend std::ostream& operator<<(std::ostream& out, const GeneticAlgo& g_algo);
    friend std::istream& operator>>(std::istream& in, GeneticAlgo& g_algo);

    void print(){fout << *this;}
};

int main(){
    std::string in_file_name = "data.in"; 
    std::string out_file_name = "data.out";

    GeneticAlgo test_algo(in_file_name, out_file_name);
    test_algo.naturalSelection();

    // std::cout << test_algo.func.fitness(-0.914592) << '\n';

}

GeneticAlgo::GeneticAlgo(const std::string& in_file_name, const std::string& out_file_name)
    : fin(in_file_name), fout(out_file_name){
    if (fin)
        fin >> *this;
    else
        throw std::runtime_error("[ERROR] GeneticAlgo: could not open file " + in_file_name);
}

// Determines and sets the new generation of chromosomes
void GeneticAlgo::naturalSelection(){
    std::uniform_real_distribution<double> dist(0, 1);

    std::vector<Chromosome> new_generation(chromosomes.size());
    std::vector<double> partial_sums = getSelPartialSums();

    fout << "Partial sums:";
    // Print partial sums
    for (int i = 0; i < partial_sums.size(); ++i){
        if (i % 6 == 0)
            fout << '\n';
        fout << partial_sums[i] << " ";
    }
    fout << '\n';

    // Generate random numbers in the interval (0, 1)
    // Determine the partial sum interval it is from and the index of the interval is the selected chromo
    for (auto& chromo : new_generation){
        double gen_val = dist(gen);
        int chosen_interval = intervalBinSearch(gen_val, partial_sums);

        fout << "u= " << gen_val << " selecting chromosome " << chosen_interval << '\n';

        chromo = chromosomes[chosen_interval];
    }
    // Set new generation and print to file
    this->chromosomes = move(new_generation);
    fout << "New generation:\n";
    print();
}

// std::vector<double> GeneticAlgo::getSelProb() const{
//     // Get the sum of the chromosomes values
//     double chromo_sum = 0;
//     for (const auto& chromo : chromosomes)
//         chromo_sum += chromo.getValue();
    
//     // Get the probability of selection for each chromosome
//     std::vector<double> result(chromosomes.size());
//     for (int i = 0; i < result.size(); ++i)
//         result[i] = chromosomes[i].getValue() / chromo_sum;
    
//     return result;
// }

std::vector<double> GeneticAlgo::getSelPartialSums(){
    // Get the sum of the chromosomes values
    double chromo_sum = 0;
    for (const auto& chromo : chromosomes)
        chromo_sum += func.fitness(chromo.getValue());
    
    // Determine the selection intervals(partial sums)
    std::vector<double> partial_sums(chromosomes.size() + 1);

    fout << "Selection probabilities:\n";
    for (int i = 1; i < partial_sums.size(); ++i){
        double chromo_sel_prob = func.fitness(chromosomes[i - 1].getValue()) / chromo_sum;
        fout << "Chromosome " << i << ": "<< chromo_sel_prob << '\n';

        partial_sums[i] = partial_sums[i - 1] + chromo_sel_prob;
    }

    assert(partial_sums.front() == 0.0);
    assert(std::abs(partial_sums.back() - 1.0) < 1e-9);

    return partial_sums;
}

// TODO: Static testing for toBits, fromBits, fitness function and selection
bool verify(){
    std::vector<std::vector<bool>> bits = {
        {0,0,0,0,0,1,1,1,0,1,0,0,1,0,0,1,1,1,0,0,0,1},
        {0,0,1,0,1,0,0,1,0,0,1,1,1,0,1,1,1,1,1,1,0,0},
        {0,1,0,0,0,0,0,0,1,0,1,0,0,1,0,1,1,0,1,1,1,1},
        {1,1,0,1,0,0,1,1,1,0,0,0,1,1,0,1,1,0,1,1,0,1},
        {1,0,0,1,1,1,0,0,1,0,0,1,1,1,0,0,1,1,0,0,0,1},
        {1,0,1,1,1,1,1,0,0,1,0,0,0,0,1,1,0,0,0,1,0,0},
        {0,1,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,1,1,1,1},
        {0,0,0,0,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,0,1,0},
        {0,1,0,1,1,1,0,1,1,1,1,0,1,0,0,1,0,0,1,1,1,1},
        {0,0,1,1,1,0,1,0,1,0,1,1,0,1,1,0,0,0,1,0,0,1},
        {1,1,0,0,1,1,0,1,1,1,0,1,0,0,1,0,0,1,1,1,1,1},
        {0,1,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0},
        {0,1,1,1,0,1,0,0,0,0,0,0,0,1,0,1,1,0,0,1,0,0},
        {1,1,0,0,0,0,0,0,0,1,1,1,0,1,1,1,0,0,0,1,0,1},
        {1,0,1,1,0,1,0,1,0,1,0,0,0,0,0,0,1,0,1,1,0,1},
        {1,1,0,1,0,1,1,1,0,1,0,1,1,0,1,1,0,1,0,1,0,1},
        {1,1,0,1,1,0,1,1,1,0,1,0,0,0,1,0,0,1,1,1,1,1},
        {0,0,1,0,0,1,0,1,0,1,1,0,0,1,0,1,1,0,1,1,1,0},
        {1,0,1,1,1,0,1,1,0,0,0,0,0,0,0,1,0,0,1,1,1,1}
    };
    
    std::vector<double> x_values = {
        -0.914592, -0.516787, -0.246207, 1.480791, 0.835307, 1.229633, 0.133068, 
        -0.897179, 0.100578, -0.311975, 1.411980, 0.404924, 1.954865, 0.359503, 
        1.255452, 1.124764, 1.527482, 1.573845, -0.562311, 1.191435
    };
    
    std::vector<double> f_values = {
        0.24892945287918744, 1.2161433551687346, 1.69317538311855, 1.288048141479274, 
        2.1375690008662946, 1.7176358177971365, 2.115360642895274, 0.29789055630549277, 
        2.0904619502816644, 1.590697249227337, 1.4182933275624994, 2.240960465910953, 
        0.13336723106734194, 2.230260491735224, 1.6792930285557333, 1.8596696522000606, 
        1.1942809038290045, 1.0968573281925882, 1.1214962076226662, 1.771918100644858
    };

    Function test_func(std::vector<double>({-1, 1, 2}), -1, 2, 0);
}

double Function::fitness(double x) const{
    double result = 0;
    size_t coef_count = func_coef.size();

    for (int i = 0; i < coef_count; ++i)
        result += func_coef[i] * pow(x, coef_count - i - 1);
    return result;
}

void GeneticAlgo::initializePopulation(size_t population_size, size_t nr_bits){
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
    
    return out;
}

std::ostream& operator<<(std::ostream& out, const Chromosome& chromo){
    chromo.printBits(out) << " value: " << chromo.value;

    return out;
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
    g_algo.func.setIntervalStep(nr_bits);
    g_algo.initializePopulation(population_size, nr_bits);

    in >> g_algo.crossover_prob >> g_algo.mutation_prob;
    in >> g_algo.stage_count;

    return in;
}

int fromBits(const std::vector<bool>& bits){
    int res = 0;
    for (int i = 0; i < bits.size(); ++i)
        res += bits[i] << bits.size() - i - 1;
    return res;
}

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